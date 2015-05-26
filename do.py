#!/usr/bin/env python3
from sys import argv
import lttngAnalyses
if __name__ == '__main__':
	if argv[0:]:
		print('usage:\n$ ./do.py start_monitoring_server <self_IP> <self_port>\
			\n$ ./do.py stop_monitoring_server\
			\n$ ./do.py start_monitoring_client <monitoring_server_IP> <monitoring_server_port>\
			\n$ ./do.py stop_monitoring_client\')