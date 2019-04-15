#! /bin/bash

# Sbatch options
#SBATCH -J pl_morph
#SBATCH -p dedicated
#SBATCH --qos=dbc
### #SBATCH --qos=fast
#SBATCH --cpus-per-task=1 
#SBATCH --mem-per-cpu=700MB
### #SBATCH --time=8


# Constants
logs_folder="./logs/"


# Launch srun with these argument sequence
srun -o "${logs_folder}log_job_${SLURM_ARRAY_TASK_ID}.out" -e "${logs_folder}log_job_${SLURM_ARRAY_TASK_ID}.err" -J "${SLURM_ARRAY_TASK_ID}" plac_morph ./in



