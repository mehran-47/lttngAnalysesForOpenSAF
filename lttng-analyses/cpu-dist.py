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
import argparse
import shutil
import time
from babeltrace import *
from LTTngAnalyzes.common import *
from LTTngAnalyzes.sched import *
from analyzes import *
from ascii_graph import Pyasciigraph

class CPUTop():
    def __init__(self, traces):
        self.trace_start_ts = 0
        self.trace_end_ts = 0
        self.traces = traces
        self.tids = {}
        self.cpus = {}
        self.history = {}

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
        # stats for the whole trace
        self.compute_stats()
        #self.output(args, self.trace_start_ts, self.trace_end_ts, final=1)
        self.graph_output(args, self.trace_start_ts, self.trace_end_ts, final=1)

    def update_history(self, args, sec):
        self.history[sec] = {}
        self.history[sec]["total_ns"] = self.end_ns - self.start_ns
        self.history[sec]["proc"] = {}
        h = self.history[sec]["proc"]
        for tid in self.tids.values():
            if not tid.comm in args.proc_list:
                continue
            if not tid.comm in h.keys():
                h[tid.comm] = tid.cpu_ns
            else:
                h[tid.comm] += tid.cpu_ns
        total_cpu_pc = 0
        for cpu in self.cpus.values():
            total_cpu_pc += cpu.cpu_pc
        total_cpu_pc = total_cpu_pc / len(self.cpus.keys())
        self.history[sec]["cpu"] = total_cpu_pc

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
            self.update_history(args, event_sec)
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
        for sec in self.history.keys():
            s = self.history[sec]
            print("sec : %lu, total_ns : %lu" % (sec, s["total_ns"]))
            for p in s["proc"].keys():
                print("%s : %lu" % (p, s["proc"][p]))

    def graph_output(self, args, begin_ns, end_ns, final=0):
        for comm in args.proc_list:
            graph = Pyasciigraph()
            values = []
            for sec in sorted(self.history.keys()):
                if not comm in self.history[sec]["proc"].keys():
                    break
                pc = float("%0.02f" % (
                    (self.history[sec]["proc"][comm] * 100) /
                        self.history[sec]["total_ns"]))
                values.append(("%s" % sec_to_hour(sec), pc))
            for line in graph.graph("%s CPU Usage" % comm, values):
                print(line)
        graph = Pyasciigraph()
        values = []
        for sec in sorted(self.history.keys()):
            pc = float("%0.02f" % (self.history[sec]["cpu"]))
            values.append(("%s" % sec_to_hour(sec), pc))
        for line in graph.graph("Total CPU Usage", values):
            print(line)

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
            help='Aggregate period in seconds (default = 1)', default=1)
    parser.add_argument('--names', type=str, default=0,
            help='Only this coma-separated list of process names')
    args = parser.parse_args()
    args.proc_list = []
    if args.names:
        args.proc_list = args.names.split(",")

    if args.refresh < 1:
        print("Refresh period must be >= 1 sec")
        sys.exit(1)

    traces = TraceCollection()
    handle = traces.add_trace(args.path, "ctf")
    if handle is None:
        sys.exit(1)

    c = CPUTop(traces)

    c.run(args)

    traces.remove_trace(handle)
