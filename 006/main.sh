#!/bin/bash

CDM_LINE="-i bond1 host 120.240.14.234 or host 120.198.124.167"
DURATION_SECOND=3600

while [ 1==1 ]
do
OUTPUT_FILE=file.pcap.`date | awk '{print $1"-"$2"-"$3"-"$4"-"$5"-"$6}'`
(nohup /usr/sbin/tcpdump -nns0 $CDM_LINE -w $OUTPUT_FILE)& 
PID=$!
sleep $DURATION_SECOND
kill -9 $PID
done
