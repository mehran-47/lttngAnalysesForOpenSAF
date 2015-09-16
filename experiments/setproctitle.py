#!/usr/bin/env python3
import setproctitle, time
from multiprocessing import Process

def dank(strToPrint):
    setproctitle.setproctitle('dank_process')
    while True:
        time.sleep(2)
        print(strToPrint, end='\t')

if __name__=='__main__':
    pL = [Process(target=dank, name='dank_proc_'+str(i), args=('alert from process -'+str(i), ) ) for i in range(1,6)]
    for p in pL:
        p.start()