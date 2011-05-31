// cv_dll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "cv.h"
#include "highgui.h"
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include "libcvdll.h"

//параметры слежения(в секундах)
const double MHI_DURATION = 1;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;

//число цикл. буф. кадра для детекта
const int N = 4;

//кольцевой буфер изображения
IplImage **buf = 0;
int last = 0;

//временные изображения
IplImage *mhi = 0; //история движения
IplImage *orient = 0; //ориентация
IplImage *mask = 0; //действительные маски ориентации
IplImage *segmask = 0; //карта движений
CvMemStorage* storage = 0; //временное хранилище


// Параметры:
// img - входной кадр
// dst - результирующее изображение движение 
// args - дополнительные параметры

int  update_mhi( IplImage* img, IplImage* dst, int diff_threshold )
{
    double timestamp = (double)clock()/CLOCKS_PER_SEC; //Получение текущего времени в секундах 
    CvSize size = cvSize(img->width,img->height); //Получение размера кадра
    int i, idx1 = last, idx2, motion=0;
    IplImage* silh;
    CvSeq* seq;
    CvRect comp_rect;
    double count;
    double angle;
    CvPoint center;
    double magnitude;         
    CvScalar color;
 
	//проверка размера кадра и если необходимо его изменение
	if( !mhi || mhi->width != size.width || mhi->height != size.height ) {
        if( buf == 0 ) {
            buf = (IplImage**)malloc(N*sizeof(buf[0]));
            memset( buf, 0, N*sizeof(buf[0]));
        }

        for( i = 0; i < N; i++ ) {
            cvReleaseImage( &buf[i] );
            buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
            cvZero( buf[i] );
        }

        cvReleaseImage( &mhi );
        cvReleaseImage( &orient );
        cvReleaseImage( &segmask );
        cvReleaseImage( &mask );

        mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        cvZero( mhi );
        orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
    }

    cvCvtColor( img, buf[last], CV_BGR2GRAY ); //конвертирование кадра в серый
    idx2 = (last + 1) % N; //индексирование (last-(n-1) кадра)
    last = idx2;
    silh = buf[idx2];
    cvAbsDiff( buf[idx1], buf[idx2], silh ); //получение разницы между кадрами. dst(i) = abs(src1(i) – src2(i)).
    cvThreshold( silh, silh, diff_threshold, 1, CV_THRESH_BINARY ); //и порогового значения
	cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); //обновление mhi

    //преобразование mhi в синий
	cvCvtScale( mhi, mask, 255./MHI_DURATION,(MHI_DURATION - timestamp)*255./MHI_DURATION );
    cvZero( dst );
    cvCvtPlaneToPix( mask, 0, 0, 0, dst );

    //расчет движение градиента и действующей маски ориентации
    cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );
  

    if( !storage )
        storage = cvCreateMemStorage(0);
    else
        cvClearMemStorage(storage);

    //получение последовательности движений
	//и получения их карты
    seq = cvSegmentMotion( mhi, segmask, storage, timestamp, MAX_TIME_DELTA );

    //обход полученой карты
	//i==-1 - движение всего изобрадения
    for( i = -1; i < seq->total; i++ ) {
		 if( i < 0 ) {//для всего изображения
            comp_rect = cvRect( 0, 0, size.width, size.height );
            color = CV_RGB(255,255,0);
            magnitude = 100;
        }
        else { //для i-ого компанента
            comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i ))->rect;
            if( comp_rect.width + comp_rect.height < 100 ) //для мелких детелей
                continue;
            color = CV_RGB(255,0,0);
            magnitude = 30;
        }

       //выбор компанентов ROI
        cvSetImageROI( silh, comp_rect );
        cvSetImageROI( mhi, comp_rect );
        cvSetImageROI( orient, comp_rect );
        cvSetImageROI( mask, comp_rect );

		//расчет ориентации
		angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  
        count = cvNorm( silh, 0, CV_L1, 0 ); //число точек для ROI
 

        cvResetImageROI( mhi );
        cvResetImageROI( orient );
        cvResetImageROI( mask );
        cvResetImageROI( silh );

		//проверка для малого движения
        if( count < comp_rect.width*comp_rect.height * 0.05 )
            continue;
		 motion = 1;
       //рисуем навревление движения
        center = cvPoint( (comp_rect.x + comp_rect.width/2),(comp_rect.y + comp_rect.height/2) );
 
         cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
		 cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
							cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );
    }
	return motion;
}

void save_scr(IplImage* image)
{
	time_t t;
	int fg,fl=1;
	char filename[512],filenamet[512]="";
	time(&t);
	//;
	sprintf(filename, "%s..",asctime(localtime(&t)));
	fg=1;
	while(fg!=0)
	{
		 if (filename[fg]==':') {filename[fg]='.';}
		 fg++;
		 if (filename[fg]=='\n') {filename[fg]='\0';fg=0;}
	}
	if (strcmp(filename,filenamet)!=0) 
		{
//		printf("capture... %s\n",filename);
		sprintf(filename, "%s.jpg",filename);
		cvSaveImage(filename, image);
		strcpy(filenamet,filename);
		}
}



void cv_cam(int index_cam, int show_cam, char* file_name )
{
	IplImage* motion = 0;
    CvCapture* capture = 0;
	if(index_cam == 0) capture = cvCaptureFromCAM(index_cam);
	else  capture = cvCaptureFromFile(file_name);
    if( capture )
    {
       if(show_cam) cvNamedWindow( "Motion", 1 );
        for(;;)
        {
            IplImage* image;
            if( !cvGrabFrame( capture ))
                break;
            image = cvRetrieveFrame( capture );
            if( image )
            {
                if( !motion )
                {
                    motion = cvCreateImage( cvSize(image->width,image->height), 8, 3 );
                    cvZero( motion );
                    motion->origin = image->origin;
                }
            }

			if(update_mhi( image, motion, 30 ))		
									{save_scr(image);} ; 
            if(show_cam == 1) cvShowImage( "Motion", motion ); 
			else if (show_cam == 2) cvShowImage( "Motion", image );
            if( cvWaitKey(10) >= 0 )
                break;
        }
        cvReleaseCapture( &capture );
        cvDestroyWindow( "Motion" );
    }

}

#if defined WIN32 || defined _WIN32
DLLexport void cv_start(int index_cam, int show_cam,char* file_name)
{
	cv_cam(index_cam,show_cam,file_name );
}
#else
 void cv_start(int index_cam, int show_cam,char* file_name )
{
	cv_cam(index_cam,show_cam,file_name);
}
#endif

