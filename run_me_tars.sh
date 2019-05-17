#! /bin/bash


## Constants
processes=100
logs_folder="./logs/"
output_folder="./results/"



# Create the logs folder
echo "Cleaning up the logs folder"
if [ ! -d "$logs_folder" ]
then
	mkdir $logs_folder
# Else empty the folder
else
	rm -v ${logs_folder}*
fi


# Create the output folder
echo "Cleaning up the results folder"
if [ ! -d "$output_folder" ]
then
	mkdir $output_folder
# Else empty the folder
else
	rm -v ${output_folder}*
fi


# Launch sbatch
sbatch -o /dev/null --array=1-$processes sbatch_one_job.sh

