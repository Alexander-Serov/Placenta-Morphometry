//#include <fstream>
//#include <iostream>
//#include <sys/types.h>
//#include <errno.h>
#include <string>
// #include <algorithm>
// #include <utility>  
// #include <ctime>
// #include "CImg.h"
#include "Global_variables.h"
#include "Extract_contours.h"
#include "Output_statistics.h"
#include "Log_steps.h"
#include "Population.h"
#include "Small_functions.h"


/*
// Platform-specific commands
#ifdef _WIN32
	#include"direntvc.h"
#elif __linux__
	#include"dirent.h"
#endif
	*/




// Functions declarations
//void perimeter(pop::VecF64 &, pop::Mat2UI32 &);







void statistics_placenta(// std::ofstream& file_txt, 
						 //time_t& tbegin, time_t& tend, 
						 const pop::Mat2UI8& img_3_objects, 
						 //char *resultDir, 
						 float& perIVS, 
						 //std::string& img_name_withoutExtension, 
						 //pop::Mat2RGBUI8& subImg, 
						 //const pop::Mat2UI8& img_final, 
						 std::string& lock, 
						 char *tmpStr
						 )
{

	
	int size_i=img_3_objects.sizeI();
	int size_j=img_3_objects.sizeJ();


	// *** start quantitative analysis ***	//	
	
	
	log_steps("Preparing villous regions statistics");
		
	// Labeling villous regions
	//{ // Limitting variables scope
		// Preparing a binary image for labelin
	pop::Mat2UI8 bl_img_complete_villous_regions(size_i, size_j);
	float villi_density=0;
	for (int i=0; i<size_i; i++)
		for (int j=0; j<size_j; j++)
		{
			bl_img_complete_villous_regions(i,j) = img_3_objects (i,j) <= int_fetal_RBCs_pixel_color;
			// On the way, calculating the final villi density
			villi_density+=bl_img_complete_villous_regions(i,j);
		}
	villi_density/=(size_i*size_j);

	pop::Mat2UI32 label_img_segment (pop::Processing::clusterToLabel(bl_img_complete_villous_regions, 0));
	
	// Calculating the area of each region
	pop::VecF64 varea_img_segment (pop::Analysis::areaByLabel(label_img_segment));

	// Initializing a new vector with the number of regions - 1
	pop::VecF64 vperimeter_img_segment(pop::Analysis::maxValue(label_img_segment));
	// Calculating the perimeter of each region (??)	
	perimeter(vperimeter_img_segment, label_img_segment);


	// Painting different labels in different colors
	pop::Mat2RGBUI8 rgb_img_segment(size_i,size_j);
	labelToRGB(rgb_img_segment, label_img_segment);
	
	// Marking label numbers on the colored image. The function labelObject is extremely inefficient
	labelObjet(rgb_img_segment, label_img_segment, 3, 0);
	
	// Saving the labeled picture
	saveFileColor(rgb_img_segment, resultDir, str_img_name_withoutExtension+string_output_picutre_number()+"_Villous_regions_labeled.png");
	
	rgb_img_segment.clear();
	//}

	// Next step
	
	

	// I don't know what this does...
	std::vector <std::vector <int> > perimeter_img_segment_frame( vperimeter_img_segment.size(), std::vector<int>() );									
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			if((label_img_segment(i,j)!=0) && ((i==0) || (i==size_i-1) || (j==0) || (j==size_j-1)))	
			{
				perimeter_img_segment_frame[label_img_segment(i,j)-1].push_back(1);
			}
		}
	}


	log_steps();		// End: "Preparing villous regions statistics"

	
	// Labeling fetal capillaries inside each region
	log_steps("Preparing RBCs statistics");
	
	// Preparing a binary image for labeling
	pop::Mat2UI8 bl_img_fetal_capillaries(size_i,size_j);
	for(int i=0;i<size_i;i++)
		for(int j=0;j<size_j;j++)
			bl_img_fetal_capillaries(i,j) = img_3_objects(i,j) == int_fetal_RBCs_pixel_color;
			/*if(img_3_objects(i,j)==int_fetal_RBCs_pixel_color)	
			{
				bl_img_fetal_capillaries(i,j)=int_white_pixel_color;
			}*/
	//img_final.clear();

	// Labeling regions
	pop::Mat2UI32 label_GR_segment = pop::Processing::clusterToLabel(bl_img_fetal_capillaries,0);

	// Calculating their areas
	pop::VecF64 varea_GR_segment = pop::Analysis::areaByLabel(label_GR_segment);

	// Calculating their perimeters
	pop::VecF64 vperimeter_GR_segment(pop::Analysis::maxValue(label_GR_segment));
	perimeter(vperimeter_GR_segment, label_GR_segment);

	// Painting the regions in random RGB colors
	pop::Mat2RGBUI8 rgb_GR_segment(label_GR_segment.sizeI(),label_GR_segment.sizeJ());
	labelToRGB(rgb_GR_segment, label_GR_segment);	

	// Marking label numbers on the colored image. The function labelObject is extremely inefficient
	labelObjet(rgb_GR_segment, label_GR_segment, 3, 0);
	
	// Saving the labeled picture
	saveFileColor(rgb_GR_segment, resultDir, str_img_name_withoutExtension+string_output_picutre_number()+"_Fetal_capillaries_labeled.png");
	
	
	bl_img_fetal_capillaries.clear(); 
	rgb_GR_segment.clear();
	

	
	// Next step?

	// I don't know what this does.
	// Probably detecting which capillaries lie within which villi
	std::vector <std::vector <int> > numberGRinsideRegion(pop::Analysis::maxValue(label_img_segment),std::vector <int>());
	std::vector <std::vector <int> > numberIVSinside (pop::Analysis::maxValue(label_img_segment),std::vector <int>());
	bool isPresent=false;
	for(int j=0;j<size_j;j++)
	{
		for(int i=0;i<size_i;i++)
		{
			if(label_img_segment(i,j)!=0 && label_GR_segment(i,j)!=0)
			{
				isPresent=find(numberGRinsideRegion[label_img_segment(i,j)-1].begin(), numberGRinsideRegion[label_img_segment(i,j)-1].end(), label_GR_segment(i,j))!= numberGRinsideRegion[label_img_segment(i,j)-1].end();
				if(!isPresent)
				{
					numberGRinsideRegion[label_img_segment(i,j)-1].push_back(label_GR_segment(i,j));
				}
			}
		}
	}
	label_GR_segment.clear();


	log_steps();		// End: "Preparing RBCs statistics"

	// IVS inside each region and outside region
	log_steps("Preparing IVS statistics");
	//file_txt<<"start: statistics of IVS region."<<endl<<"Past time: "<<time(NULL)-tbegin<<endl<<endl;
	//time_t tIVS=time(NULL);
	pop::Mat2UI8 bl_img_inside_IVS_regions(size_i,size_j);
	pop::Mat2UI8 bl_img_outside_IVS_regions(size_i,size_j);
	

	// The following code separates IVS regions which completely lie inside IVS regions from thos which touch the image boundary

	// Creating complete villi binary image
	for (int i=0; i<size_i; i++)
		for (int j=0; j<size_j; j++)
			bl_img_complete_villous_regions(i,j) = img_3_objects (i,j)<=int_fetal_RBCs_pixel_color;
	//saveFileBinary (bl_img_complete_villous_regions, resultDir, img_name_withoutExtension+string_output_picutre_number()+"_Complete_villous_regions.png");

	// Filling holes in this image (which correspond to "internal" IVS regions)
	pop::Mat2UI8 bl_img_complete_villous_regions_filled = pop::Processing::holeFilling (bl_img_complete_villous_regions,0);
	// Converting to bool since hole Filling returns 255 as white (why??)
	bl_img_complete_villous_regions_filled/=255;

	//saveFileBinary (bl_img_complete_villous_regions_filled, resultDir, img_name_withoutExtension+string_output_picutre_number()+"_Complete_villous_regions_filled.png");
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			// The pixels which stayed 0 are the external IVS
			bl_img_outside_IVS_regions(i,j) = !bl_img_complete_villous_regions_filled(i,j);
			
			// The pixels which were filled by the hole filling are the internal IVS
			bl_img_inside_IVS_regions(i,j)=bl_img_complete_villous_regions_filled(i,j) - bl_img_complete_villous_regions(i,j);

			/*if(bl_img_complete_villous_regions_filled(i,j)==0)
			{
				bl_img_outside_IVS_regions(i,j)=int_white_pixel_color;
			}*/
		}
	}

	// Checking if the outside and the inside IVS regions have been correctly identified
	//saveFileBinary (bl_img_outside_IVS_regions, resultDir, img_name_withoutExtension+string_output_picutre_number()+"_Outside_IVS_regions.png");
	//saveFileBinary (bl_img_inside_IVS_regions, resultDir, img_name_withoutExtension+string_output_picutre_number()+"_Inside_IVS_regions.png");
	
	bl_img_complete_villous_regions.clear();
	bl_img_complete_villous_regions_filled.clear();


	// Performing measurements on inside IVS regions
	// Labeling inside IVS regions
	pop::Mat2UI32 label_IVS_inside_segment = pop::Processing::clusterToLabel(bl_img_inside_IVS_regions,0);

	// Painting the regions
	pop::Mat2RGBUI8 rgb_IVS_inside_segment(label_IVS_inside_segment.sizeI(),label_IVS_inside_segment.sizeJ());
	labelToRGB(rgb_IVS_inside_segment, label_IVS_inside_segment);

	// Measuring their areas
	pop::VecF64 varea_IVS_inside_segment = pop::Analysis::areaByLabel(label_IVS_inside_segment);

	// Measuring their perimeter
	pop::VecF64 vperimeter_IVS_inside_segment(pop::Analysis::maxValue(label_IVS_inside_segment));
	perimeter(vperimeter_IVS_inside_segment, label_IVS_inside_segment);

	// I don't understand this. Why would I fill holes in IVS regions which are already inside villi. 
	// It's only for the case if there were other villi inside...
	pop::Mat2UI8 bl_img_IVS_inside_tmp=pop::Processing::holeFilling(bl_img_inside_IVS_regions,0);
	pop::Mat2UI32 label_img_segment_tmp (label_img_segment);
	
	
	// I don't know exactly what is happenning in the next two cycles
	pop::Mat2UI8 bl_img_GR_insideIVSinside(size_i,size_j); // GR in the IVS which is inside region
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			if(bl_img_IVS_inside_tmp(i,j)!=bl_img_inside_IVS_regions(i,j))
			{
				label_img_segment_tmp(i,j)=0;
				bl_img_GR_insideIVSinside(i,j)=1;
			}
		}
	}
	isPresent=false;
	for(int j=1;j<size_j-1;j++)
	{
		for(int i=1;i<size_i-1;i++)
		{
			if(bl_img_inside_IVS_regions(i,j)>0 && label_img_segment_tmp(i-1,j)!=0)
			{
				isPresent=find(numberIVSinside[label_img_segment(i-1,j)-1].begin(), numberIVSinside[label_img_segment(i-1,j)-1].end(), 
					label_IVS_inside_segment(i,j))!= numberIVSinside[label_img_segment(i-1,j)-1].end();
				if(!isPresent)
				{
					numberIVSinside[label_img_segment(i-1,j)-1].push_back(label_IVS_inside_segment(i,j));
				}
			}
		}
	}
	label_img_segment.clear();
	label_img_segment_tmp.clear(); 


	// Labeling fetal capillaries inside villi (??)
	pop::Mat2UI32 label_GR_insideIVSinside = pop::Processing::clusterToLabel(bl_img_GR_insideIVSinside,0); // GR in the IVS which is inside region
	
	//pop::Mat2RGBUI8 rgb_GR_insideIVSinside(label_GR_insideIVSinside.sizeI(),label_GR_insideIVSinside.sizeJ());
	//// Why label if it will be cleared in the next line?
	//labelToRGB(rgb_GR_insideIVSinside, label_GR_insideIVSinside);
	//rgb_GR_insideIVSinside.clear();

	// Calculating areas
	pop::VecF64 varea_GR_insideIVSinside (pop::Analysis::areaByLabel(label_GR_insideIVSinside));

	// Calculating perimeters
	pop::VecF64 vperimeter_GR_insideIVSinside(pop::Analysis::maxValue(label_GR_insideIVSinside));
	perimeter(vperimeter_GR_insideIVSinside, label_GR_insideIVSinside);

	// I don't know what we do here
	std::vector <std::vector <int> > numberGR_insideIVSinside(pop::Analysis::maxValue(label_IVS_inside_segment),std::vector <int>());
	isPresent=false;
	for(int j=1;j<size_j;j++)
	{
		for(int i=1;i<size_i;i++)
		{
			if(bl_img_GR_insideIVSinside(i,j)>0 && label_IVS_inside_segment(i-1,j)!=0)
			{
				isPresent=find(numberGR_insideIVSinside[label_IVS_inside_segment(i-1,j)-1].begin(), numberGR_insideIVSinside[label_IVS_inside_segment(i-1,j)-1].end(), label_GR_insideIVSinside(i,j))!= numberGR_insideIVSinside[label_IVS_inside_segment(i-1,j)-1].end();
				if(!isPresent)
				{
					numberGR_insideIVSinside[label_IVS_inside_segment(i-1,j)-1].push_back(label_GR_insideIVSinside(i,j));
				}
			}
		}
	}
	bl_img_GR_insideIVSinside.clear(); 
	label_GR_insideIVSinside.clear(); 
	label_IVS_inside_segment.clear();


	// IVS outside Region
	// Labeling
	pop::Mat2UI32 label_IVS_outside_segment = pop::Processing::clusterToLabel(bl_img_outside_IVS_regions,0);
	
	pop::Mat2RGBUI8 rgb_IVS_outside_segment(size_i,size_j);
	// Painting. Why? rgb_IVS_outside_segment will be used later...
	labelToRGB(rgb_IVS_outside_segment, label_IVS_outside_segment);

	// Calculating areas
	pop::VecF64 varea_IVS_outside_segment = pop::Analysis::areaByLabel(label_IVS_outside_segment);

	// Calculating perimeters
	pop::VecF64 vperimeter_IVS_outside_segment(pop::Analysis::maxValue(label_IVS_outside_segment));
	perimeter(vperimeter_IVS_outside_segment, label_IVS_outside_segment);

	// Once again I don't know what this is
	std::vector <std::vector <int> > perimeter_IVS_outside_frame(vperimeter_IVS_outside_segment.size(), std::vector<int>());									
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			if((label_IVS_outside_segment(i,j)!=0) && ((i==0) || (i==size_i-1) || (j==0) || (j==size_j-1)))	
			{
				perimeter_IVS_outside_frame[label_IVS_outside_segment(i,j)-1].push_back(1);
			}
		}
	}

	pop::Mat2UI8 bl_img_GR_insideIVSoutside(size_i,size_j);

	// Removing IVS inside?? Why??
	pop::Mat2UI8 GR_insideIVSoutside_tmp=pop::Processing::holeFilling(bl_img_outside_IVS_regions,0);
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			if(GR_insideIVSoutside_tmp(i,j)!=bl_img_outside_IVS_regions(i,j))
			{
				bl_img_GR_insideIVSoutside(i,j)=1;
			}
		}
	}
	GR_insideIVSoutside_tmp.clear();

	// I don't know what we are doing here
	pop::Mat2UI32 label_GR_insideIVSoutside = pop::Processing::clusterToLabel(bl_img_GR_insideIVSoutside,0); // GR in the IVS which is outside region
	pop::Mat2RGBUI8 rgb_GR_insideIVSoutside(label_GR_insideIVSoutside.sizeI(),label_GR_insideIVSoutside.sizeJ());
	labelToRGB(rgb_GR_insideIVSoutside, label_GR_insideIVSoutside);
	rgb_GR_insideIVSoutside.clear();
	pop::VecF64 varea_GR_insideIVSoutside (pop::Analysis::areaByLabel(label_GR_insideIVSoutside));
	pop::VecF64 vperimeter_GR_insideIVSoutside(pop::Analysis::maxValue(label_GR_insideIVSoutside));
	perimeter(vperimeter_GR_insideIVSoutside, label_GR_insideIVSoutside);
	std::vector <std::vector <int> > numberGR_insideIVSoutside;
	for(int i=0;i<pop::Analysis::maxValue(label_IVS_outside_segment);i++)
	{
		numberGR_insideIVSoutside.push_back(std::vector<int>());
	}
	isPresent=false;
	for(int j=1;j<size_j;j++)
	{
		for(int i=1;i<size_i;i++)
		{
			if(bl_img_GR_insideIVSoutside(i,j)>0 && label_IVS_outside_segment(i-1,j)!=0)
			{
				isPresent=find(numberGR_insideIVSoutside[label_IVS_outside_segment(i-1,j)-1].begin(), numberGR_insideIVSoutside[label_IVS_outside_segment(i-1,j)-1].end(), label_GR_insideIVSoutside(i,j))!= numberGR_insideIVSoutside[label_IVS_outside_segment(i-1,j)-1].end();
				if(!isPresent)
				{
					numberGR_insideIVSoutside[label_IVS_outside_segment(i-1,j)-1].push_back(label_GR_insideIVSoutside(i,j));
				}
			}
		}
	}
	bl_img_GR_insideIVSoutside.clear(); 
	label_GR_insideIVSoutside.clear();

	// Label outside IVS regions
	labelObjet(rgb_IVS_outside_segment, label_IVS_outside_segment, 3, 0);
	//rgb_IVS_inside_segment=labelObjet(rgb_IVS_inside_segment, label_IVS_inside_segment, (255,255,255), 2, Analysis::maxValue(label_IVS_outside_segment));
	label_IVS_outside_segment.clear(); 
	

	
	// Preparing a binary image with all IVS regions
	pop::Mat2UI8 bl_img_IVS_segment(size_i,size_j);
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			bl_img_IVS_segment(i,j) = bl_img_outside_IVS_regions(i,j)>0 || 
				bl_img_inside_IVS_regions(i,j)>0;
			/*if(bl_img_outside_IVS_regions(i,j)>0 || bl_img_inside_IVS_regions(i,j)>0)
			{
				bl_img_IVS_segment(i,j)=1;
			}*/
		}
	}
	//bl_img_inside_IVS_regions.clear();

	// Labeling the regions
	pop::Mat2UI32 label_IVS_segment (pop::Processing::clusterToLabel(bl_img_IVS_segment,0));
	bl_img_IVS_segment.clear();

	// Painting the regions
	pop::Mat2RGBUI8 rgb_IVS_segment(size_i,size_j);
	labelToRGB(rgb_IVS_segment, label_IVS_segment);

	label_IVS_segment.clear();
	
	// The following code is copying the labels for inside and outside regions.
	// Inside and outside IVS regions can have same labels. Strange...
	// Actually, the code shows that internal regions have not be labeled with digits. Is it OK? Even more strange...
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			if(rgb_IVS_outside_segment(i,j).r()==255 && rgb_IVS_outside_segment(i,j).g()==255 && rgb_IVS_outside_segment(i,j).b()==255)
			{
				rgb_IVS_segment(i,j).r()=255;
				rgb_IVS_segment(i,j).g()=255;
				rgb_IVS_segment(i,j).b()=255;
			}
			if(rgb_IVS_inside_segment(i,j).r()==255 && rgb_IVS_inside_segment(i,j).g()==255 && rgb_IVS_inside_segment(i,j).b()==255)
			{
				rgb_IVS_segment(i,j).r()=255;
				rgb_IVS_segment(i,j).g()=255;
				rgb_IVS_segment(i,j).b()=255;
			}
		}
	}
	saveFileColor(rgb_IVS_segment, resultDir, str_img_name_withoutExtension+string_output_picutre_number()+"_IVS_regions_labeled.png");
	rgb_IVS_outside_segment.clear(); 
	rgb_IVS_inside_segment.clear(); 
	rgb_IVS_segment.clear();
	//file_txt<<"It takes "<<time(NULL)-tIVS<<"s to get the statistics of IVS region."<<endl;
	//file_txt<<"end: statistics of IVS region."<<endl<<"Past time: "<<time(NULL)-tbegin<<endl<<endl;
					

	log_steps();		// End: "Preparing IVS statistics"


	if (bl_extract_contours) 
		extract_contours(img_3_objects, bl_img_outside_IVS_regions, bl_img_inside_IVS_regions, bl_img_IVS_inside_tmp,
			resultDir, str_img_name_withoutExtension);
	
	// Cleaning up
	bl_img_inside_IVS_regions.clear();
	bl_img_outside_IVS_regions.clear();
	bl_img_IVS_inside_tmp;

	
	// Outputting statistics
	// Calling a function with a frightening number of parameters.
	// Apparently their must have been a smarter way to make these calculations
	output_statistics(size_i, size_j, perIVS, villi_density, varea_img_segment, 
		vperimeter_img_segment, varea_IVS_inside_segment, vperimeter_IVS_inside_segment, varea_GR_segment, vperimeter_GR_segment,
		varea_IVS_outside_segment, vperimeter_IVS_outside_segment, varea_GR_insideIVSoutside, vperimeter_GR_insideIVSoutside,
		varea_GR_insideIVSinside, vperimeter_GR_insideIVSinside, perimeter_img_segment_frame, numberIVSinside,
		numberGRinsideRegion, perimeter_IVS_outside_frame, numberGR_insideIVSoutside, numberGR_insideIVSinside);
	
	
							
	/*if (!bl_extract_contours)
	{

	}*/
	//file_txt<<"It takes "<<time(NULL)-tStatistics<<"s to output statistics."<<endl;
	//file_txt<<"end: output statistcs."<<endl<<"Past time: "<<time(NULL)-tbegin<<endl<<endl;
	//file_txt.close();

	

}