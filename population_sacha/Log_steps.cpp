#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>		// std::stringstream
#include <math.h>
#include "Global_variables.h"
//#include "Population.h"

//using namespace std;

//// Global variables - Externally defined
//extern std::string extension;
//extern std::string str_img_name_withoutExtension;
//extern char *mainDir;
//extern char *resultDir;

// Local constants
const std::string general_log_filename="General_execution_log.txt";
const std::string log_filename_end="_execution-steps.txt";

// Local functions
std::string string_of_arrows(int length);



struct Log_record
{
	std::string step_description;
	time_t start_time;
};





int log_steps(std::string step_description, std::string log_type)

{
	// Local variables
	//static bool bl_started=false;
	static bool bl_clear_general_log=true;
	time_t current_time;
	double elapsed_time;
	std::ofstream file_stream;
	std::string log_filename;
	//char * log_filename_pointer;
	//static std::string last_step_description;
	std::string output_string;
	//static time_t last_call_time = time(NULL);
	std::string str_current_time;
	char string_buffer[80];
	struct tm *temp_time_structure;
	const int Max_records=50;
	static int records_number=0;

	// Initializing an array of pointers to the records
	static Log_record* records_array [Max_records];


	//// Getting time

	current_time=time(NULL);
	// Calculating normal current time
	temp_time_structure=localtime(&current_time);
	// Formatting time
	strftime(string_buffer, 80,"%H:%M:%S", temp_time_structure);
	str_current_time=string_buffer;
	// Calculating time difference from the start
	// elapsed_time=difftime(current_time, last_call_time);

	
	//Output to the file and to the console
	if (log_type=="image")
	// For image log
	{
		log_filename=resultDir+str_img_name_withoutExtension+log_filename_end;
		const char* log_filename_pointer = log_filename.c_str();
		file_stream.open(log_filename_pointer, std::ios::app);	
	

		// If a non-empty log description provided, starting a new step
		if (step_description!="" && records_number < Max_records)
		{
			// Starting a new step

			// Saving a new log record
			records_array[records_number] = new Log_record;
			records_array[records_number]->step_description = step_description;
			records_array[records_number]->start_time = current_time;
			records_number++;
			
			
			// Saving to file and displaying on screen
			std::string output_string = str_current_time + "\t" + string_of_arrows(records_number-1) + "START:\t" + 
				step_description + ".";
			file_stream << output_string << std::endl<<std::endl;
			std::cout << output_string << std::endl<<std::endl;
			
			

			//last_step_description=step_description;
			//last_call_time=current_time;
			//bl_started=true;
			
		}
		else if (records_number > 0)
		{
			// Else: ending the previous step (if existing)

			// Calculating the elapsed time
			time_t start_time = records_array [records_number-1]->start_time;
			time_t end_time = time(NULL);
			time_t elapsed_time=difftime(end_time, start_time);
			// Formatting time
			float flt_elapsed_time = (float) elapsed_time;
			int mins_number = floor (flt_elapsed_time/60);
			int secs_number = floor (flt_elapsed_time - mins_number * 60);
			std::string str_elapsed_time;
			if (mins_number > 0)
				sprintf (string_buffer, "%i min(s) %i s", mins_number, secs_number);
			else
				sprintf (string_buffer, "%i s", secs_number);
			str_elapsed_time=string_buffer;
			
			// Saving to file and displaying on screen
			std::string output_string = str_current_time + "\t" + string_of_arrows(records_number-1) + "END:\t" + 
				records_array[records_number-1]->step_description + ".\tElapsed time: " + str_elapsed_time;
			file_stream<< output_string << std::endl<<std::endl;
			std::cout << output_string << std::endl<<std::endl;

			// Deleting the last record and reducing the records number

			delete records_array[records_number-1];
			records_number--;
			
		}
				
		// Closing file
		file_stream.close();

	}
	else if (log_type=="general")
	{
		log_filename=mainDir+general_log_filename;
		const char* log_filename_pointer = log_filename.c_str();

		if (bl_clear_general_log)
		{
			file_stream.open(log_filename_pointer, std::ios::trunc);
			file_stream.close();
			bl_clear_general_log=false;
		}

		file_stream.open(log_filename_pointer, std::ios::app);	
	
		// If a non-empty log description provided, starting a new step
		if (step_description!="")
		{
			// Starting a new step
			file_stream<<str_current_time<<"\t "<<step_description<<"."<<std::endl<<std::endl;
			std::cout<<str_current_time<<"\t "<<step_description<<"."<<std::endl<<std::endl;
			//// Saving current parameters
			//last_step_description=step_description;
			//last_call_time=current_time;
			//bl_started=true;
		}	
		// Closing file
		file_stream.close();
	}
	

	return(1);
}




/*
// Overloading the function for a string stream
int log_steps(stringstream step_description, string log_type)
{
	return log_steps(step_description.str(), log_type);
}
*/



// Creating tabulation characters for inlaid execution list elements
std::string string_of_arrows(int length)
{
	std::string output_string (" ");

	for (int i=0; i<length; i++)
		output_string+="> ";

	return output_string;
}