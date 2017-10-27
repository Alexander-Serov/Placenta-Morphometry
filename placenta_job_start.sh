#!/bin/bash
for i in {1..15}
do
	qsub /home/sacha/Population/placenta_job.sh
	sleep 2s
done

