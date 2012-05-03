#!/bin/bash

# mantis run script

for RATE in $(seq 500 100 1000)
do
    echo "invoking mantis at ${RATE}"
    ./Mantis -r ${RATE} -o /data/Test${RATE}.mantis -d 900 -c 630 >> Test.log
    echo "sleeping"
    sleep 300;
done
