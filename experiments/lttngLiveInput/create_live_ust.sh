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
#$BAB_COMMAND