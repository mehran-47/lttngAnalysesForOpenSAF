#!/bin/bash
SESSION_LIST=$(lttng list)
if [ "$SESSION_LIST" == 'Currently no available tracing session' ]
then
	echo "-- No tracing session running"
else
	lttng destroy
fi
rm -rf /root/lttng-traces/*
rm -rf /home/mk/lttng-traces/$(hostname)/*