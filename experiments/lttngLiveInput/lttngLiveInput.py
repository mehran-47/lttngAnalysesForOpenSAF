#!/usr/bin/env python3
from subprocess import Popen, PIPE, call, check_call
import shlex, sys

if __name__ == '__main__':
    #out_string = check_call("ls -la".split(" "), universal_newlines=True)
    #print(out_string)
    babeltrace_string = 'babeltrace -i lttng-live'
    print( Popen(['babeltrace -i lttng-live net://localhost/host/mk-HP-ProBook-4430s/'+str(sys.argv[1])] , shell=True, stdout=PIPE).communicate()[0].decode('utf-8'))
