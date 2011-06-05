#include "cv_h.h"
#include <iostream>


int main(int argc, char* argv[])
{

	int index_cam,index_mode;
	char file_name[20];
	

	std::cout << "please enter the index cam"<< std::endl << "0 - web cam"<< std::endl <<"1 - video file"<< std::endl << std::endl;
	std::cin >> index_cam;
	if (index_cam) {std::cout << "please puts file name"<<std::endl << std::endl;
	std::cin >> file_name;}
					std::cout << "please enter the index mode"<< std::endl << "0 - no image"<< std::endl <<"1 - detect image"<< std::endl << "2 - normal image"<< std::endl;
	std::cin >> index_mode;
	cv_cam(index_cam,index_mode,file_name);
	
	return 0;
}

