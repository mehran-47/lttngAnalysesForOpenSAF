#!/bin/bash
#SESS_OUT=$(lttng create --live)
IFS=' ' read -a SESS_OUT <<< $(lttng create --live)
lttng enable-channel -k chann1 -C $1M -W $2 && \
lttng enable-channel -u chann1 -C $1M -W $2 && \
lttng enable-event -k lttng_statedump_start,lttng_statedump_end,lttng_statedump_process_state\
,lttng_statedump_file_descriptor,lttng_statedump_vm_map,lttng_statedump_network_interface,lttng_stat\
edump_interrupt,sched_process_free,sched_switch,sched_process_fork -c chann1 ; \
lttng add-context -k -t pid -t procname -t tid -t ppid -t perf:cache-misses -t perf:major-faults -t perf:branch-load-misses ; \
lttng enable-event -u -a --loglevel=TRACE_DEBUG -c chann1
lttng start

#creating UST session
#lttng create usttrace- --live
#lttng enable-event -u -a --loglevel=TRACE_DEBUG
#lttng start

lttng list

#echo "in the echooooooo...${SESS_OUT[1]}"
CURRENT_TRACEPATH=$(realpath /root/lttng-traces/$(hostname)/${SESS_OUT[1]})
echo "kernel tracepath: $CURRENT_TRACEPATH/kernel"
echo "ust tracepath: $CURRENT_TRACEPATH/ust"
#./lttng-analyses/cputop.py $CURRENT_TRACEPATH
babeltrace -i lttng-live net://localhost
#echo ./lttng-analyses/cputop.py -r 2 --top 100 $CURRENT_TRACEPATH/kernel
echo ./lttng-analyses/ust_proc.py $CURRENT_TRACEPATH 172.16.159.1
#echo -e "vlc : $(ps -e | grep vlc) "
#echo -e "Xorg : $(ps -e | grep Xorg) "
#echo -e "firefox : $(ps -e | grep firefox) "
