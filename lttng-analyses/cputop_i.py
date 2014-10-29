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
import json
import random
import os.path
from babeltrace import *
from LTTngAnalyzes.common import *
from LTTngAnalyzes.sched import *
from analyzes import *
from multiprocessing import Queue
import psutil as ps

class CPUTop():
    def __init__(self, traces, handle, activeComps, usage_q):
        self.activeComps = activeComps
        self.handle = handle
        self.usage_q = usage_q
        self.trace_start_ts = 0
        self.trace_end_ts = 0
        self.traces = traces
        self.tids = {}
        self.cpus = {}

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
            #return self.output(args, self.trace_start_ts, self.trace_end_ts, final=1)
            self.output(args, self.trace_start_ts, self.trace_end_ts, final=1)
        else:
            # stats only for the last segment
            self.compute_stats()
            #return self.output(args, self.start_ns, self.trace_end_ts, final=1)
            self.output(args, self.start_ns, self.trace_end_ts, final=1)

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
        usage_dict = self.tids
        
        #print('processing usage dict between %s to %s' % (ns_to_asctime(begin_ns), ns_to_asctime(end_ns)))
        to_send = {
        'msg' : '', 
        'from' : os.uname()[1],
        'time' : str(ns_to_asctime(begin_ns)) + " to " + str(ns_to_asctime(end_ns)),
        'nstime' : end_ns,
        'component_info' : self.activeComps,
        'cpu_core_usages' : []
        }
        if len(self.activeComps) != 0:
            for component in self.activeComps:
                pid = int(self.activeComps[component]['PID'])
                if os.path.exists("/proc/"+str(pid)):
                    if pid in usage_dict:
                        pc = float("%0.02f" % ((usage_dict[pid].cpu_ns * 100) / total_ns))
                        self.activeComps[component]['cpu_usage'] = pc
                    else:
                        self.activeComps[component]['cpu_usage'] = ps.Process(pid).cpu_percent(interval=0.5)
                else:
                    self.activeComps[component]['cpu_usage'] = None

        to_send['component_info'] = self.activeComps
        for component in self.activeComps:
            if not self.activeComps[component]['cpu_usage']:
                del to_send['component_info'][component]

        nb_cpu = len(self.cpus.keys())
        for cpu in sorted(self.cpus.values(),
                key=operator.attrgetter('cpu_ns'), reverse=True):
            cpu_total_ns = cpu.cpu_ns
            cpu_pc = float("%0.02f" % cpu.cpu_pc)
            to_send["cpu_core_usages"].append(cpu_pc)
        self.usage_q.put(to_send)


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


def cputop_init(path, activeComps, usage_q ,**kwargs):
    args = argparse.Namespace()
    args.path = path
    args.refresh = 5 if kwargs.get('refresh')==None else kwargs.get('refresh')
    args.top = 10 if kwargs.get('top')==None else kwargs.get('top')
    args.proc_list = []
    traces = TraceCollection()
    handle = traces.add_trace(args.path, "ctf")
    if handle is None:
        sys.exit(1)
    c = CPUTop(traces, handle ,activeComps, usage_q)
    try:
        return c.run(args)
    except KeyboardInterrupt:
        print("\n'KeyboardInterrupt' received. Stopping trace processing.")
    except:
        print("\nUnknown Exception")
        raise
    traces.remove_trace(handle)