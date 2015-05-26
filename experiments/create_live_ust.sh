#!/bin/bash
lttng-sessiond -d
lttng-relayd -d
lttng create --live 2000000 -U net://localhost
lttng enable-event -u -a --loglevel=TRACE_DEBUG
lttng start
ARR_SESS_NAME=$(babeltrace -i lttng-live net://localhost)
SESS_NAME=($ARR_SESS_NAME)
#babeltrace -i lttng-live net://localhost
BAB_COMMAND="babeltrace -i lttng-live ${SESS_NAME[0]}"
echo $BAB_COMMAND
$BAB_COMMAND
#sleep 23s
#echo '['$(date +%T)'.850625231] (+0.000579686) node2 lttng_ust_tracef:event: { cpu_id = 1}, { _msg_length = 124, msg = "Dispatched CSI Set Callback for Component: safComp=AmfDemo_44,safSu=SU1,safSg=AmfDemo,safApp=AmfDemo1, 1402495444.850126" }'
