#! /bin/bash

# Clean (to comment later)
# rm -rf *_tiles;

## Constants
CutSize=5000	# px
ResizeScale="50%"	# px
MemoryLimit="50GB"
ThreadLimit=11

# Set java machine memory limit
# java -Xmx51200m > /dev/null

for image in ./*.ndpi
do
	# Extract name without path
	filename=$(basename "$image")

	# Extract working folder
	folder=`pwd`
	
	# Extract name without extension
	name_no_ext="${filename%.*}"
	tiles_folder="${folder}/${name_no_ext}_tiles"
	tif_folder="${folder}/tif"

	# Create a tif folder if it doesn't exist
	if [ ! -d "${tif_folder}" ]
	then
		mkdir "$tif_folder"
	fi


	# Create tiles folder if it does not exist
	# echo $tiles_folder
	if [ ! -d "${tiles_folder}" ]
	then
		mkdir "$tiles_folder"
		

		#convert -verbose -limit memory ${MemoryLimit} -limit thread ${ThreadLimit} ${filename} -crop ${CutSize}x${CutSize} -set filename:tile "%[fx:page.y/${CutSize}+1]_%[fx:page.x/${CutSize}+1]" ^ +adjoin "${tiles_folder}/${name_no_ext}_tile_%[filename:tile].tif"

		# Create a tif file using bfconvert
		bfconvert ${filename} -series 0 -compression LZW  "${tif_folder}/${name_no_ext}.tif"

		# Create tiles using bfconvert
		bfconvert ${filename} -series 0 -tilex ${CutSize} -tiley ${CutSize} -compression LZW  "${tiles_folder}/${name_no_ext}_tile_%y_%x.tif"

		# -crop 0,0,${CutSize},${CutSize}

		# Resize the resulting images with Image Magick
		for tile in ${tiles_folder}/*.tif
		do
			convert -limit memory ${MemoryLimit} -resize ${ResizeScale} ${tile} ${tile}
		done

		echo "The file was cut and resized successfully!"
	else
		echo "The file has been processed before. Changing nothing."
	fi

	# echo $folder $filename $name_no_ext
done





















