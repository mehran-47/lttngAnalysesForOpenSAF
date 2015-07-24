#!/usr/bin/env python3
import psutil as ps, json
from sys import argv
from multiprocessing import cpu_count

if __name__=='__main__':
    usages = {'cpu_usage':[], 'memory_usage':[]}
    procList = [ps.Process(int(aProcess)) for aProcess in argv[1:]]
    while True:
        try:
            cpu_percent = 0
            memory_usage = 0
            for aProcess in procList:
                cpu_percent += aProcess.get_cpu_percent(interval=0.5)
                memory_usage += aProcess.get_memory_percent()
            usages['cpu_usage'].append(cpu_percent/cpu_count())
            usages['memory_usage'].append(memory_usage)
        except KeyboardInterrupt:
            summary = {}
            for key in usages.keys(): summary[key] = sum(usages[key])/len(usages[key])
            usages['summary'] = summary
            print(summary)
            with open('usageDump.json','w') as udf: json.dump(usages, udf)
            break