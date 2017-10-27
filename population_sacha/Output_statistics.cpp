

#include <fstream>		// std::ofstream
#include <vector>		// std::vector
#include "Log_steps.h"
#include "Global_variables.h"
#include "Population.h"
#include "Small_functions.h"







void output_statistics(//const char *resultDir, 
					   //const std::string& str_img_name_withoutExtension,
					   const int size_i,
					   const int size_j,
					   const float percentage_of_IVS,
					   const float villi_density,
					   const pop::VecF64& varea_img_segment,
					   const pop::VecF64& vperimeter_img_segment,
					   const pop::VecF64& varea_IVS_inside_segment,
					   const pop::VecF64& vperimeter_IVS_inside_segment,
					   const pop::VecF64& varea_GR_segment,
					   const pop::VecF64& vperimeter_GR_segment,
					   const pop::VecF64& varea_IVS_outside_segment,
					   const pop::VecF64& vperimeter_IVS_outside_segment,
					   const pop::VecF64& varea_GR_insideIVSoutside,
					   const pop::VecF64& vperimeter_GR_insideIVSoutside,
					   const pop::VecF64& varea_GR_insideIVSinside,
					   const pop::VecF64& vperimeter_GR_insideIVSinside,
					   const std::vector <std::vector <int> >& perimeter_img_segment_frame,
					   const std::vector <std::vector <int> >& numberIVSinside,
					   const std::vector <std::vector <int> >& numberGRinsideRegion,
					   const std::vector <std::vector <int> >& perimeter_IVS_outside_frame,
					   const std::vector <std::vector <int> >& numberGR_insideIVSoutside,
					   const std::vector <std::vector <int> >& numberGR_insideIVSinside
					   )
{
	log_steps("Outputting statistics");
	//file_txt<<"start: output statistics."<<endl<<"Past time: "<<time(NULL)-tbegin<<endl<<endl;
	//time_t tStatistics=time(NULL);ion+string_output_picutre_number()+"_O
	std::string str_general_statistics_filename=resultDir+str_img_name_withoutExtension + string_output_picutre_number() + 
		"_General_statistics.csv";
	std::string str_villous_regions_statistics_filename=resultDir+str_img_name_withoutExtension + string_output_picutre_number() + 
		"_Villous_regions_statistics.csv";
	std::string str_IVS_statistics_filename = resultDir+str_img_name_withoutExtension + string_output_picutre_number() + 
		"_IVS_statistics.csv";
	std::string str_fetal_capillaries_statistics_filename=resultDir+str_img_name_withoutExtension + string_output_picutre_number() + 
		"_Fetal_capillaries_statistics.csv";
	
	//std::string str_villous_regions_statistics_filename_total=resultDir+str_img_name_withoutExtension+"_statistic-region-total.csv";
	//std::string str_fetal_capillaries_statistics_filename_total=resultDir+str_img_name_withoutExtension+"_statistic-GR-total.csv";
	//std::string str_IVS_statistics_filename_total=resultDir+str_img_name_withoutExtension+"_statistic-IVS-total.csv";


	std::ofstream file_csv;
	std::vector <float> perimeter_img_segment, area_img_segment,perimeter_GR_segment, area_GR_segment,perimeter_IVSinsideRegion_segment, area_IVSinsideRegion_segment, perimeter_IVS_outside_segment, area_IVS_outside_segment, perimeter_IVS_inside_segment, area_IVS_inside_segment;
	float tmp_perimeter=0, tmp_area=0;
	char *file_tmp = new char[FILENAME_LENGTH];
							
	//image
	copy(str_general_statistics_filename.begin(), str_general_statistics_filename.end(), file_tmp);
	file_tmp[str_general_statistics_filename.size()] = '\0';
	file_csv.open(file_tmp, std::ios::app);
	file_csv<<"Image height (µm);Image width (µm);Image area (µm²);Villi density (before processing);Villi density (after processing)"
		<<std::endl;
	file_csv<<size_i*Image_resolution<<";"<<size_j*Image_resolution<<";"<<
		size_i*size_j*Image_resolution*Image_resolution<<";"<<1-percentage_of_IVS << villi_density <<std::endl;
	file_csv.close();

	//Region and GR and IVS inside
	copy(str_villous_regions_statistics_filename.begin(), str_villous_regions_statistics_filename.end(), file_tmp);
	file_tmp[str_villous_regions_statistics_filename.size()] = '\0';
	file_csv.open(file_tmp, std::ios::app);
	file_csv<<"Villous region number;Surface (µm²);Perimeter (µm);Perimeter_contact (µm);Perimeter_frame (µm);Nb_IVS_Inside_Region;IVS_Area_Inside_Region (µm²);IVS_Perimeter_Inside_Region (µm);Nb_Fetal_Capillaries_Inside;Fetal_Capillaries_Area_Inside (µm²);FetalCapillariesPerimeter (µm)"
		<<std::endl;
	for(int i=0;i<varea_img_segment.size();i++)
	{
		area_img_segment.push_back(varea_img_segment(i)*Image_resolution*Image_resolution);
		perimeter_img_segment.push_back(vperimeter_img_segment(i)*Image_resolution);
		file_csv<<i+1<<";"<<varea_img_segment(i)*Image_resolution*Image_resolution<<";"<<vperimeter_img_segment(i)*Image_resolution<<";";
		file_csv<<(vperimeter_img_segment(i)-perimeter_img_segment_frame[i].size())*Image_resolution<<";"<<perimeter_img_segment_frame[i].size()*Image_resolution<<";";
		tmp_perimeter=0, tmp_area=0;
		for(int j=0;j<numberIVSinside[i].size();j++)
		{
			tmp_area+=varea_IVS_inside_segment(numberIVSinside[i][j]-1);
			tmp_perimeter+=vperimeter_IVS_inside_segment(numberIVSinside[i][j]-1);
		}
		file_csv<<numberIVSinside[i].size()<<";"<<tmp_area*Image_resolution*Image_resolution<<";"<<tmp_perimeter*Image_resolution<<";";
		tmp_perimeter=0, tmp_area=0;
		for(int j=0;j<numberGRinsideRegion[i].size();j++)
		{
			tmp_area+=varea_GR_segment(numberGRinsideRegion[i][j]-1);
			tmp_perimeter+=vperimeter_GR_segment(numberGRinsideRegion[i][j]-1);
		}
		file_csv<<numberGRinsideRegion[i].size()<<";"<<tmp_area*Image_resolution*Image_resolution<<";"<<tmp_perimeter*Image_resolution
			<<std::endl;
								
	}
	file_csv.close();



	// IVS outside: consider IVS inside villi
	copy(str_IVS_statistics_filename.begin(), str_IVS_statistics_filename.end(), file_tmp);
	file_tmp[str_IVS_statistics_filename.size()] = '\0';
	file_csv.open(file_tmp, std::ios::app);  
	file_csv<<"IVS region number;Surface (µm²);Perimeter (µm);Perimeter_contact (µm);Perimeter_frame (µm);Nb_Villi_Inside_Region;Surface_Villi_Inside_IVS (µm²);Perimeter_Villi_Inside_Region (µm)"
		<<std::endl;
	for(int i=0;i<varea_IVS_outside_segment.size();i++)
	{
		area_IVS_outside_segment.push_back(varea_IVS_outside_segment(i)*Image_resolution*Image_resolution);
		perimeter_IVS_outside_segment.push_back(vperimeter_IVS_outside_segment(i)*Image_resolution);
		file_csv<<i+1<<";"<<varea_IVS_outside_segment(i)*Image_resolution*Image_resolution<<";"<<vperimeter_IVS_outside_segment(i)*Image_resolution<<";";
		file_csv<<(vperimeter_IVS_outside_segment(i)-perimeter_IVS_outside_frame[i].size())*Image_resolution<<";"<<perimeter_IVS_outside_frame[i].size()*Image_resolution<<";";
		tmp_perimeter=0, tmp_area=0;
		for(int j=0;j<numberGR_insideIVSoutside[i].size();j++)
		{
			tmp_area+=varea_GR_insideIVSoutside(numberGR_insideIVSoutside[i][j]-1);
			tmp_perimeter+=vperimeter_GR_insideIVSoutside(numberGR_insideIVSoutside[i][j]-1);
		}
		file_csv<<numberGR_insideIVSoutside[i].size()<<";"<<tmp_area*Image_resolution*Image_resolution<<";"<<
			tmp_perimeter*Image_resolution<<std::endl;
	}
	for(int i=0;i<varea_IVS_inside_segment.size();i++)
	{
		area_IVS_inside_segment.push_back(varea_IVS_inside_segment(i)*Image_resolution*Image_resolution);
		perimeter_IVS_inside_segment.push_back(vperimeter_IVS_inside_segment(i)*Image_resolution);
		file_csv<<i+1+varea_IVS_outside_segment.size()<<";"<<varea_IVS_inside_segment(i)*Image_resolution*Image_resolution<<";"<<vperimeter_IVS_inside_segment(i)*Image_resolution<<";"<<vperimeter_IVS_inside_segment(i)*Image_resolution<<";"<<"0;";
		tmp_perimeter=0, tmp_area=0;
		for(int j=0;j<numberGR_insideIVSinside[i].size();j++)
		{
			tmp_area+=varea_GR_insideIVSinside(numberGR_insideIVSinside[i][j]-1);
			tmp_perimeter+=vperimeter_GR_insideIVSinside(numberGR_insideIVSinside[i][j]-1);
		}
		file_csv<<numberGR_insideIVSinside[i].size()<<";"<<tmp_area*Image_resolution*Image_resolution<<";"<<
			tmp_perimeter*Image_resolution<<std::endl;
	}
	file_csv.close();


	// GR
	copy(str_fetal_capillaries_statistics_filename.begin(), str_fetal_capillaries_statistics_filename.end(), file_tmp);
	file_tmp[str_fetal_capillaries_statistics_filename.size()] = '\0';
	file_csv.open(file_tmp, std::ios::app);
	file_csv<<"Fetal capillary number;Surface (µm²);Perimeter (µm)"<<std::endl;
	for(int i=0;i<varea_GR_segment.size();i++)
	{
		area_GR_segment.push_back(varea_GR_segment(i)*Image_resolution*Image_resolution);
		perimeter_GR_segment.push_back(vperimeter_GR_segment(i)*Image_resolution);
		file_csv<<i+1<<";"<<varea_GR_segment(i)*Image_resolution*Image_resolution<<";"<<vperimeter_GR_segment(i)*Image_resolution
			<<std::endl;
	}
	file_csv.close();

	

	log_steps();		// End: "Outputting statistics"
}











