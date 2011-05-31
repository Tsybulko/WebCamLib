// cv_main.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#if defined WIN32 || defined _WIN32
#include <string>
#include <iostream>
#include <Windows.h>

static HMODULE LibraryHandle;
using namespace std;
typedef void (*cv_)(int index_cam, int show_cam, char* file_name);

int _tmain(int argc, _TCHAR* argv[])
{
	TCHAR LibraryName[] = _T("libcvdll.dll");
	int index_cam,index_mode;
	char file_name[20];
	cv_ cv_cam_;
	HMODULE LibraryHandle = LoadLibrary(LibraryName);  //указатель на dll 
	if (LibraryHandle == NULL) {
		printf("ERROR: unable to load DLL\n");
		return 1;
	}
	cv_cam_ = (cv_)GetProcAddress(LibraryHandle, "cv_start" );  
	if (cv_cam_ == NULL) {
		std::cout << "error to find the function read";
		return 1;
	}
	std::cout << "please enter the index cam"<< std::endl << "0 - web cam"<< std::endl <<"1 - video file"<< std::endl << std::endl;
	std::cin >> index_cam;
	if (index_cam) {std::cout << "please puts file name"<<std::endl << std::endl;
					//strcpy(file_name,"1.avi");
					std::cin >> file_name;
					std::cout << "please enter the index mode"<< std::endl << "0 - no image"<< std::endl <<"1 - detect image"<< std::endl << "2 - normal image"<< std::endl;
	std::cin >> index_mode;
	system("cls");
	cv_cam_(index_cam,index_mode,file_name);
	FreeLibrary(LibraryHandle);
	return 0;
}
#else
#include <stdio.h>
#include <dlfcn.h>

typedef void (*cv)(int,int,char*);

int main()
{
int index_cam,index_mode;
char* file_name;
void *libhandler;   
libhandler = dlopen("./libcvd.so",RTLD_LAZY);
if(libhandler == NULL)
        {
                printf("dlopen error");
                return 1;
        }
printf("please enter the index cam\n0 - web cam\n1 - video file\n";
	scanf("%s",index_cam);
	if (index_cam) {printf("please puts file name\n\n");
					//strcpy(file_name,"1.avi");
					scanf("%s",file_name);
printf("please enter the index model\n0 - no image\n1 - detect image\n2 - normal image\n");
scanf("%d",index_mode);
cv cv_cam = (cv)dlsym(libhandler,"cv_start");
cv_cam(index_cam,index_mode,file_name);
dlclose(libhandler);
return 0;
}
#endif
