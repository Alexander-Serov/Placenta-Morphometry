



#include "Population.h"
#include "Global_variables.h"
#include "Log_steps.h"
#include "Small_functions.h" 




void extract_contours(const pop::Mat2UI8& img_3_objects,
					  pop::Mat2UI8& IVS_outside_segment,
					  pop::Mat2UI8& IVS_inside_segment,
					  pop::Mat2UI8& IVS_inside_tmp,
					  char *resultDir, 
					  std::string& img_name_withoutExtension
					  )
{
	int size_i = img_3_objects.sizeI();
	int size_j = img_3_objects.sizeJ();
	
	//contour of regions
	log_steps("Preparing contour statistics");
	//file_txt<<"start: statistics of Contour."<<endl<<"Past time: "<<time(NULL)-tbegin<<endl<<endl;
	time_t tContour=time(NULL);
	pop::Mat2UI8 contour_total = pop::Processing::erosion(img_3_objects,1,2);
	pop::Mat2UI8 contour_region_outside_tmp = IVS_outside_segment;
	IVS_outside_segment.clear();
	binaryOpposite(contour_region_outside_tmp);
	pop::Mat2UI32 label_region_outside = pop::Processing::clusterToLabel(contour_region_outside_tmp,1);
	pop::Mat2UI8 contour_region_outside = pop::Processing::erosion(contour_region_outside_tmp,1,2);
	pop::Mat2UI8 contour_region_inside(size_i,size_j);
	conditionally_replace_color_1_by_color_2(IVS_inside_segment,int_white_pixel_color,int_black_pixel_color, 10);
	contour_region_inside=pop::Processing::dilation(IVS_inside_segment,1,2);
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			contour_total(i,j)=img_3_objects(i,j)-contour_total(i,j);
			contour_region_outside(i,j)=contour_region_outside_tmp(i,j)-contour_region_outside(i,j);
			contour_region_inside(i,j)=contour_region_inside(i,j)-IVS_inside_segment(i,j);
		}
	}
	contour_total.clear(); IVS_inside_segment.clear();

	pop::Mat2UI32 label_region_inside = pop::Processing::clusterToLabel(contour_region_inside,0);
	contour_region_inside.clear();
	std::vector <int> decide_GR_inside_IVS_insideRegion;
	for(int i=0;i<size_i;i++)
	{
		for(int j=0;j<size_j;j++)
		{
			if(label_region_inside(i,j)!=0 && IVS_inside_tmp(i,j)==int_white_pixel_color)
			{
				decide_GR_inside_IVS_insideRegion.push_back(label_region_inside(i,j));
			}
		}
	}
	IVS_inside_tmp.clear();

	for(int i=1;i<size_i-1;i++)
	{
		for(int j=1;j<size_j-1;j++)
		{
			if(contour_region_outside(i,j)==int_black_pixel_color)
			{
				label_region_outside(i,j)=0;
			}
		}
	}
	contour_region_outside.clear();	contour_region_outside_tmp.clear();

	deletePointsCross(label_region_outside);
	deletePointsCross(label_region_inside);
	deletePointsSingle(label_region_outside);
	deletePointsSingle(label_region_inside);

	// save coordinates of contour in order - counterclockwise
	std::vector <std::vector <int> > contour_region_outside_x(pop::Analysis::maxValue(label_region_outside), std::vector<int>());
	std::vector <std::vector <int> > contour_region_outside_y(pop::Analysis::maxValue(label_region_outside), std::vector<int>());
	std::vector <std::vector <int> > contour_region_inside_x(pop::Analysis::maxValue(label_region_inside), std::vector<int>());
	std::vector <std::vector <int> > contour_region_inside_y(pop::Analysis::maxValue(label_region_inside), std::vector<int>());
	ordreContour(label_region_outside, contour_region_outside_x, contour_region_outside_y);
	ordreContour(label_region_inside, contour_region_inside_x, contour_region_inside_y);

	label_region_inside.clear(); label_region_outside.clear();
	//tend = time(NULL);
	//file_txt<<"It takes "<<time(NULL)-tContour<<"s to get the statistics of Contour."<<endl;
	//file_txt<<"end: statistics of Contour."<<endl<<"Past time: "<<time(NULL)-tbegin<<endl<<endl;

		
		
		
	// Contour coordinates
	int maxSize=0;
	for(int i=0;i<contour_region_outside_x.size();i++)
	{
		if(maxSize<contour_region_outside_x[i].size())
		{
			maxSize=contour_region_outside_x[i].size();
		}
	}
	for(int i=0;i<contour_region_inside_x.size();i++)
	{
		if(maxSize<contour_region_inside_x[i].size())
		{
			maxSize=contour_region_inside_x[i].size();
		}
	}
	int j=contour_region_outside_x.size()+contour_region_inside_x.size();
	pop::Mat2F64 coordinates(2*maxSize+2,j+1);
	int k=0, length_outside, length_inside;
	coordinates(0,0)=4;
	coordinates(1,0)=0;
	coordinates(2,0)=-1;
	coordinates(3,0)=size_i+1;
	coordinates(4,0)=size_i+1;
	coordinates(5,0)=-1;
	coordinates(6,0)=-1;
	coordinates(7,0)=-1;
	coordinates(8,0)=size_j+1;
	coordinates(9,0)=size_j+1;
	for(int j=0;j<coordinates.sizeJ()-1;j++)
	{
		if(j<contour_region_outside_x.size())
		{
			length_outside=contour_region_outside_x[j].size();
			coordinates(0,j+1)=contour_region_outside_x[j].size();
			coordinates(1,j+1)=1;
			for(int i=2;i<length_outside+2;i++)
			{
				coordinates(i,j+1)=contour_region_outside_x[j][i-2];
				coordinates(i+length_outside,j+1)=contour_region_outside_y[j][i-2];
			}
		}
		else
		{
			k=j-contour_region_outside_x.size();
			length_inside=contour_region_inside_x[k].size();
			coordinates(0,j+1)=contour_region_inside_x[k].size();
			coordinates(1,j+1)=2;
			bool isPresent=find(decide_GR_inside_IVS_insideRegion.begin(), decide_GR_inside_IVS_insideRegion.end(), k+1) != 
				decide_GR_inside_IVS_insideRegion.end();
			if(isPresent)
			{
				coordinates(1,j+1)=3;
			}
			for(int i=2;i<length_inside+2;i++)
			{
				coordinates(i,j+1)=contour_region_inside_x[k][i-2];
				coordinates(i+length_inside,j+1)=contour_region_inside_y[k][i-2];
			}
		}
	}
							
	// contour matrix rotate 90° clockwise
	pop::Mat2F64 coor_tmp=coordinates;
	for(int i=2;i<coordinates.sizeI();i++)
	{
		for(int j=0;j<coordinates.sizeJ();j++)
		{
			if(i<coordinates(0,j)+2)
			{
				coor_tmp(i,j)=coordinates(i+coordinates(0,j),j)*Image_resolution/1000000;
			}
			else
			{
				coor_tmp(i,j)=(size_i-coordinates(i-coordinates(0,j),j))*Image_resolution/1000000;
			}
		}
	}
	coordinates=coor_tmp;
	coor_tmp.clear();

	// Start - Output of regions contours
	std::ofstream file_csv;
	char *file_tmp = new char[FILENAME_LENGTH];
	std::string contour_coordinates=resultDir+img_name_withoutExtension+"_region-contour-coordinates.csv";
	std::copy(contour_coordinates.begin(), contour_coordinates.end(), file_tmp);
	file_tmp[contour_coordinates.size()] = '\0';
	file_csv.open(file_tmp, std::ios::app);  
	for(int i=0;i<coordinates.sizeI();i++)
	{
		for(int j=0;j<coordinates.sizeJ();j++)
		{
			if(j==coordinates.sizeJ()-1)
			{
				file_csv<<coordinates(i,j)<<std::endl;
			}
			else
			{
				file_csv<<coordinates(i,j)<<";";
			}
		}
								
	}
	coordinates.clear();
	file_csv.close();

}