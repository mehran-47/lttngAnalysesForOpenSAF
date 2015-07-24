from matplotlib import pyplot as plt
import json, numpy as np

if __name__ == '__main__':
    usageDict = {}
    with open('failoverWLDump.json', 'r') as jfr: usageDict = json.loads(jfr.read())
    major_ticks_x = np.arange(0,101,10)
    minor_ticks_x = np.arange(0,101,1)
    ticks_y = np.arange(0,11,1)
    #print(len(usageDict['cpu_usage']['HA_vidStream_2']))
    usage_strings = ['CPU', 'Memory', 'cpu_usage', 'memory_usage']
    fig = plt.figure()
    for i in range(2):
        ax = fig.add_subplot(2,1,i+1)
        plt.scatter(range(len(usageDict[usage_strings[2+i]]['HA_vidStream_2'])), usageDict[usage_strings[2+i]]['HA_vidStream_2'], color=(0,0,0,0.6), marker='o')
        plt.scatter(range(len(usageDict[usage_strings[2+i]]['HA_vidStream_1'])), usageDict[usage_strings[2+i]]['HA_vidStream_1'], color=(0,0,0,0.8), marker='x')
        plt.ylabel(usage_strings[i]+' usage (%)-->')
        plt.title(usage_strings[i]+' usage of "SI_HA_vidStream_1" (marked by "x") and "SI_HA_vidStream_2" (marked by "o")')
        #grid and labels' preparation
        ax.set_xticks(major_ticks_x)
        ax.set_xticks(minor_ticks_x, minor=True)
        ax.set_yticks(ticks_y)
        ax.grid(which='major', alpha=0.5)
        ax.grid(which='minor', alpha=0.2)
        plt.xlabel('time (s) -->')
        plt.axis([0,90,0,4])
    plt.show()