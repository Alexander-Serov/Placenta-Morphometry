

#include"Population.h"
#include <dirent.h>
#include <string>
#include "Log_steps.h"
#include "Load_placenta_picture.h" 





using namespace std;
using namespace pop;





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
					 )
{
	// Local variables
	static bool bl_first_run=true;
	static DIR *dir_input_folder;
	dirent *directory_entry; 
	string img_name;



	// Storing data for subsequent runs
	if (bl_first_run)
	{
		bl_first_run=false;
		dir_input_folder = opendir(fileInDir); 
				
	}

	log_steps("Trying to load images from folder: " + (string) fileInDir, "general");

	// Checking if the input folder exists
	if(dir_input_folder==NULL)
	{
		log_steps("Error opening the folder: " + (string) fileInDir, "general");
		return false;
	}
		
	// Looking for the next non-processed picture in the folder
	while ((directory_entry = readdir(dir_input_folder)) != NULL)
	{
		img_name = directory_entry->d_name;		// Extracting the filename

		// If the current directory entry is not a file, skip it
		if (img_name.length()<=4) continue;
		if (img_name.at(0) == '.') continue;
		
		// If the next file doesn't suit us, restart the while cycle and look for a new picture
		if( !load_placenta_picture(img_name_withoutExtension, img_name, img_In, lock, extension, fileInDir, 
			resultDir, subImg, processed_a_new_picture) )	
		{
			log_steps("Skipping file: " + img_name, "general");
			continue; 
		}
		
		log_steps("Successfully loaded: " + img_name, "general");
		return true;		// A good picture is loaded

	}
	
	// No more entries in the folder
	closedir (dir_input_folder);		// Close folder
	bl_first_run=true;					// Clear local memeory of the folder to allow restart
	
	return false;

}











