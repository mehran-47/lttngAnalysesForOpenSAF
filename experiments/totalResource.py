#!/usr/bin/env python3
from subprocess import call, Popen, PIPE
import re, time, psutil as ps, sys


if __name__ == '__main__':
    memexec = Popen(['cat','/proc/meminfo'], stdout=PIPE)
    cpuexec = Popen(['lscpu'], stdout=PIPE)
    memTotalPattern = re.compile(r'(?<=MemTotal:)(.*?)(?=kB)', re.DOTALL)
    cpuTotalPattern = re.compile(r'(?<=CPU MHz)(.*?)(?=\n)', re.DOTALL)
    memtot = float(memTotalPattern.search(memexec.communicate()[0].decode('utf-8')).group(0).strip())
    #print(float(cpuTotalPattern.search(cpuexec.communicate()[0].decode('utf-8')).group(0).split(':')[1].strip()))

    tot, tries = 0.0, 56
    for numrun in range(tries):
        cpuexec = Popen(['lscpu'], stdout=PIPE)
        tot += float(cpuTotalPattern.search(cpuexec.communicate()[0].decode('utf-8')).group(0).split(':')[1].strip())
    tot = tot/tries
    #print("total cpu cycles:%r, single core cpu cycles: %r" %((tot/tries)*ps.NUM_CPUS, tot))

    while True:
        if sys.argv[1:]:
            pp = ps.Process(int(sys.argv[1]))
            cpup = pp.get_cpu_percent(interval=1)
            print("cpu percent:%.2f, cpu abs:%.2f, cpu tot:%.2f" %(cpup ,cpup*tot/100, tot))
            print("mem percent:%.2f, mem abs:%.2f, mem tot:%.2f" %(pp.get_memory_percent(), pp.get_memory_percent()*memtot/100, memtot/1024))
            print('\n\n\n\n')
        else:
            print('provide PID')
            sys.exit()