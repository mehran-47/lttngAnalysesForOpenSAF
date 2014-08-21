#!/usr/bin/env python3
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

import sys
import os
import argparse
import shutil
from babeltrace import *
from LTTngAnalyzes.common import *
from LTTngAnalyzes.sched import *
from analyzes import *
from networking.connection import connection
from multiprocessing import Queue

class CPUTop():
    def __init__(self, traces):
        self.trace_start_ts = 0
        self.trace_end_ts = 0
        self.traces = traces
        self.tids = {}
        self.cpus = {}
        self.client = connection('172.16.159.1',5555)
        self.queue = Queue()
        try:
            self.client.connect(args.to.split(':')[0], 6666)
        except ConnectionRefusedError:
            print("No server found running at "+ args.to.split(':')[0] + ":6666'")
        except:
            print("Failed to connect to server")
            raise

    def run(self, args):
        """Process the trace"""
        self.current_sec = 0
        self.start_ns = 0
        self.end_ns = 0

        sched = Sched(self.cpus, self.tids)
        for event in self.traces.events:
            if self.start_ns == 0:
                self.start_ns = event.timestamp
            if self.trace_start_ts == 0:
                self.trace_start_ts = event.timestamp
            self.end_ns = event.timestamp
            self.check_refresh(args, event)
            self.trace_end_ts = event.timestamp

            if event.name == "sched_switch":
                sched.switch(event)
        if args.refresh == 0:
            # stats for the whole trace
            self.compute_stats()
            self.output(args, self.trace_start_ts, self.trace_end_ts, final=1)
        else:
            # stats only for the last segment
            self.compute_stats()
            self.output(args, self.start_ns, self.trace_end_ts,
                    final=1)

    def check_refresh(self, args, event):
        """Check if we need to output something"""
        if args.refresh == 0:
            return
        event_sec = event.timestamp / NSEC_PER_SEC
        if self.current_sec == 0:
            self.current_sec = event_sec
        elif self.current_sec != event_sec and \
                (self.current_sec + args.refresh) <= event_sec:
            self.compute_stats()
            self.output(args, self.start_ns, event.timestamp)
            self.reset_total(event.timestamp)
            self.current_sec = event_sec
            self.start_ns = event.timestamp

    def compute_stats(self):
        for cpu in self.cpus.keys():
            current_cpu = self.cpus[cpu]
            total_ns = self.end_ns - self.start_ns
            if current_cpu.start_task_ns != 0:
                current_cpu.cpu_ns += self.end_ns - current_cpu.start_task_ns
            cpu_total_ns = current_cpu.cpu_ns
            current_cpu.cpu_pc = (cpu_total_ns * 100)/total_ns
            if current_cpu.current_tid >= 0:
                self.tids[current_cpu.current_tid].cpu_ns += \
                    self.end_ns - current_cpu.start_task_ns

    def output(self, args, begin_ns, end_ns, final=0):
        count = 0
        limit = args.top
        total_ns = end_ns - begin_ns
        values = []
        usage_dict = {}
        
        print('Sent usage dict between %s to %s' % (ns_to_asctime(begin_ns), ns_to_asctime(end_ns)))
        to_send = {
        'msg' : '', 
        'from' : os.uname()[1],
        'time' : str(ns_to_asctime(begin_ns)) + " to " + str(ns_to_asctime(end_ns)),
        'pid_usages' : {},
        'cpu_usages' : [],
        'component_info' : []
        }

        for pid in args.only.split(','):
            usage_dict = self.tids
            pid = int(pid)
            if pid in usage_dict:
                pc = float("%0.02f" % ((usage_dict[pid].cpu_ns * 100) / total_ns))
                to_send["pid_usages"][pid] = pc
            else:
                to_send["pid_usages"][pid] = -1.0

        nb_cpu = len(self.cpus.keys())
        for cpu in sorted(self.cpus.values(),
                key=operator.attrgetter('cpu_ns'), reverse=True):
            cpu_total_ns = cpu.cpu_ns
            cpu_pc = float("%0.02f" % cpu.cpu_pc)
            to_send["cpu_usages"].append(cpu_pc)
            
        self.client.send(to_send)

    def reset_total(self, start_ts):
        for cpu in self.cpus.keys():
            current_cpu = self.cpus[cpu]
            current_cpu.cpu_ns = 0
            if current_cpu.start_task_ns != 0:
                current_cpu.start_task_ns = start_ts
            if current_cpu.current_tid >= 0:
                self.tids[current_cpu.current_tid].last_sched = start_ts
        for tid in self.tids.keys():
            self.tids[tid].cpu_ns = 0
            self.tids[tid].migrate_count = 0
            self.tids[tid].read = 0
            self.tids[tid].write = 0
            for syscall in self.tids[tid].syscalls.keys():
                self.tids[tid].syscalls[syscall].count = 0

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='CPU usage analysis')
    parser.add_argument('path', metavar="<path/to/trace>", help='Trace path')
    parser.add_argument('-r', '--refresh', type=int,
            help='Refresh period in seconds', default=0)
    parser.add_argument('--top', type=int, default=10,
            help='Limit to top X TIDs (default = 10)')
    #start
    parser.add_argument('--only', type=str, default="0",
            help='List of PIDs to output, separated by commas in double quotes. e.g.: "1011,2012"')
    parser.add_argument('--to', type=str, default="localhost",
            help='Send the analysis to a remote socket pair <ip:socket>. e.g.: 192.168.2.30:8080')
    #end
    args = parser.parse_args()
    args.proc_list = []
    print("Sending process-CPU usage for processes: %r" %args.only)
    traces = TraceCollection()
    handle = traces.add_trace(args.path, "ctf")
    if handle is None:
        sys.exit(1)

    c = CPUTop(traces)

    try:
        c.run(args)
    except KeyboardInterrupt:
        print("\n'KeyboardInterrupt' received. Stopping trace processing.")
    except:
        print("\nUnknown Exception")
        raise

    traces.remove_trace(handle)
