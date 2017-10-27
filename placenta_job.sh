#!/bin/bash
# Set project name
#PBS -N Human_placenta
# This option defines the name of the queue to which the process is submitted. 
# If unsure, omit this option
#PBS -q One
# Request one processor on 1 node
#PBS -l nodes=1:ppn=1
# Defines which mails to send to the e-mail address indicated with -M. 
# Codes: a - aborted job, b - beginning a job, e - exiting a job, n - do not send anything
##PBS -m a
# Request a certain amount of memory per process
#PBS -l pmem=6gb
#PBS -M sacha@pmc.polytechnique.edu


	/home/sacha/Population/bin/population /home/sacha/Population/in/

