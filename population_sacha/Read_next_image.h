#ifndef READ_NEXT_IMAGE
#define READ_NEXT_IMAGE




bool read_next_image (	char* fileInDir, 
						int& nbAllFiles, 
						string& img_name_withoutExtension, 
						string& img_In,
						string& lock,
						string& extension,
						const char *mainDir,
						const char *resultDir,
						Mat2RGBUI8& subImg,
						bool& processed_a_new_picture
					 );


#endif






