#ifndef LOAD_PLACENTA_PICTURE
#define LOAD_PLACENTA_PICTURE




#include"Population.h"
//#include"CImg.h"
#include<string>


//using namespace cimg_library;
using namespace std; //std namespace // cin/cout
using namespace pop; //Population namespace

int load_placenta_picture(string& img_name_withoutExtension,
						   const string& img_name,
						   string& img_In,
						   string& lock,
						   string& extension,
						   const char *fileInDir,
						   const char *resultDir,
						   Mat2RGBUI8& subImg,
						   bool& processed_a_new_picture);



#endif