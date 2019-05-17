#! /bin/bash

FILE_LIST=("run_me_tars.sh sbatch_one_job.sh build/bin/plac/morph")

for file in $FILE_LIST
do
	scp ./$file aserov@tars:~/placenta-morphometry/
done

echo "All files copied successfully"
echo

read -p "Press any key to continue... " -n1 -s