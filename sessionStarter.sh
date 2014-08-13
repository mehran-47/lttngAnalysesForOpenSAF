#!/bin/bash
#SESS_OUT=$(lttng create --live)
IFS=' ' read -a SESS_OUT <<< $(lttng create --live)
lttng enable-channel -k chann1 -C $1M -W $2 && \
lttng enable-event -k lttng_statedump_start,lttng_statedump_end,lttng_statedump_process_state\
,lttng_statedump_file_descriptor,lttng_statedump_vm_map,lttng_statedump_network_interface,lttng_stat\
edump_interrupt,sched_process_free,sched_switch,sched_process_fork -c chann1 ; \
lttng add-context -k -t pid -t procname -t tid -t ppid -t perf:cache-misses -t perf:major-faults -t perf:branch-load-misses ; \
lttng start
lttng list

#echo "in the echooooooo...${SESS_OUT[1]}"
CURRENT_TRACEPATH=$(realpath /root/lttng-traces/$(hostname)/${SESS_OUT[1]})
echo "current tracepath: $CURRENT_TRACEPATH/kernel"
#./lttng-analyses/cputop.py $CURRENT_TRACEPATH

echo ./lttng-analyses/cputop.py -r 2 --top 100 $CURRENT_TRACEPATH/kernel
echo ./lttng-analyses/cputop_mod.py -r 2 --only "" $CURRENT_TRACEPATH/kernel