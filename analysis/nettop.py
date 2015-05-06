#!/usr/bin/env python3
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the 'Software'), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# KNOWN LIMITATIONS: Does not account for net IO on sockets opened before
# start of trace

import sys
import argparse
from babeltrace import *
from progressbar import *
from LTTngAnalyzes.common import *
from LTTngAnalyzes.sched import *
from LTTngAnalyzes.syscalls import *

class NetTop():
    TOTAL_FORMAT = '{0:20} {1:<10} total: {2:10}'

    def __init__(self, traces, is_io_measured, is_connection_measured, number):
        self.traces = traces
        self.is_io_measured = is_io_measured
        self.is_connection_measured = is_connection_measured
        self.number = number
        self.cpus = {}
        self.tids = {}
        self.syscalls = {}

    def get_total_transfer(self, transfer):
        total = 0

        if is_connection_measured['ipv4']:
            if is_io_measured['up']:
                total += transfer['ipv4']['up']
            if is_io_measured['down']:
                total += transfer['ipv4']['down']
        if is_connection_measured['ipv6']:
            if is_io_measured['up']:
                total += transfer['ipv6']['up']
            if is_io_measured['down']:
                total += transfer['ipv6']['down']

        return total

    def process_event(self, event, sched, syscall):
        if event.name == 'sched_switch':
            sched.switch(event)
        elif event.name == 'sched_process_fork':
            sched.process_fork(event)
        elif event.name[0:4] == 'sys_':
            syscall.entry(event)
        elif event.name == 'exit_syscall':
            syscall.exit(event, False)

    def run(self, args):
        sched = Sched(self.cpus, self.tids)
        syscall = Syscalls(self.cpus, self.tids, self.syscalls)

        size = getFolderSize(args.path)
        widgets = ['Processing the trace: ', Percentage(), ' ',
                Bar(marker='#',left='[',right=']'), ' ', ETA(), ' ']

        if not args.no_progress:
            pbar = ProgressBar(widgets=widgets, maxval=size/BYTES_PER_EVENT)
            pbar.start()

        event_count = 0

        for event in self.traces.events:
            if not args.no_progress:
                try:
                    pbar.update(event_count)
                except ValueError:
                    pass

            self.process_event(event, sched, syscall)
            event_count += 1


        if not args.no_progress:
            pbar.finish()
            print

        self.output()

    def output(self):
        transferred = {}

        for tid in self.tids.keys():
            transferred[tid] = {'ipv4': {}, 'ipv6': {}}

            transferred[tid]['ipv4'] = {'up': 0, 'down': 0};
            transferred[tid]['ipv6'] = {'up': 0, 'down': 0};

            for fd in self.tids[tid].fds.values():
                if fd.fdtype is FDType.net:
                    if fd.family == AddressFamily.AF_INET:
                        transferred[tid]['ipv4']['up'] += fd.net_write
                        transferred[tid]['ipv4']['down'] += fd.net_read
                    elif fd.family == AddressFamily.AF_INET6:
                        transferred[tid]['ipv6']['up'] += fd.net_write
                        transferred[tid]['ipv6']['down'] += fd.net_read

        print('Processes by Network I/O')
        print('#' * 80)

        for tid in sorted(transferred, key = lambda tid:
                          self.get_total_transfer(transferred[tid]),
                          reverse = True)[:self.number]:

            total = self.get_total_transfer(transferred[tid])

            if total != 0:
                print(NetTop.TOTAL_FORMAT.format(self.tids[tid].comm,
                                                 '(' + str(tid) + ')',
                                                 convert_size(total)))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Network usage \
    analysis by process')
    parser.add_argument('path', metavar='<path/to/trace>', help='Trace path')
    parser.add_argument('-t', '--type', type=str, default='all',
                        help='Types of network IO to measure. Possible values:\
                        all, up, down')
    parser.add_argument('-c', '--connection', type=str, default='all',
                        help='Types of connections to measure. Possible values:\
                        all, ipv4, ipv6')
    parser.add_argument('-n', '--number', type=int, default=10,
                        help='Number of processes to display')
    parser.add_argument('--no-progress', action="store_true",
                        help='Don\'t display the progress bar')

    args = parser.parse_args()

    io_types = args.type.split(',')
    possible_io_types = ['up', 'down']

    if 'all' in io_types:
        is_io_measured = { x: True for x in possible_io_types }
    else:
        is_io_measured = { x: False for x in possible_io_types }
        for type in io_types:
            if type in possible_io_types:
                is_io_measured[type] = True
            else:
                print('Invalid type:', type)
                parser.print_help()
                sys.exit(1)

    connection_types = args.connection.split(',')
    possible_connection_types = ['ipv4', 'ipv6']

    if 'all' in connection_types:
        is_connection_measured = { x: True for x in possible_connection_types }
    else:
        is_connection_measured = { x: False for x in possible_connection_types }
        for type in connection_types:
            if type in possible_connection_types:
                is_connection_measured[type] = True
            else:
                print('Invalid type:', type)
                parser.print_help()
                sys.exit(1)

    if args.number < 0:
        print('Number of processes must be non-negative')
        parser.print_help()
        sys.exit(1)

    traces = TraceCollection()
    handle = traces.add_trace(args.path, 'ctf')

    c = NetTop(traces, is_io_measured, is_connection_measured, args.number)
    c.run(args)

    traces.remove_trace(handle)
