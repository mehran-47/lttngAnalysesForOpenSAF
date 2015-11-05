#!/usr/bin/env python3
from lttngAnalyses.networking.connection import connection
from lttngAnalyses.serverAnalyses.analysis import dictParser
from multiprocessing import Process, Pipe
from copy import deepcopy
import sys, shelve as sh, pickle

class connection_extended(connection):
    def __init__(self, host, port, iplog_loc,**kwargs):
        super(connection_extended, self).__init__(host, port, **kwargs)
        self.ip_log_location = iplog_loc

    def __decoder(self, conn, child_pipe, addr):
        fdict = {}
        IPs = []
        while True:
            data = conn.recv(1024)
            if not data:
                if self.debug:
                    print('Connected thread ending from %r' %(self.thread_from))
                    child_pipe.send({'msg':'END_OF_Q'})
                db = sh.open(self.ip_log_location, writeback=True)
                IPs = deepcopy(db['IPs'])
                if addr in IPs: del IPs[IPs.index(addr)]
                db['IPs'] = IPs
                db.close()
                conn.close()
                break
                return
            fdict = pickle.loads(data)
            self.thread_from = fdict.get('from')
            child_pipe.send(fdict)
            #string_received = data.decode(encoding='UTF-8')


    def listen(self, child_pipe):
        msg = ''
        toret = None
        writer_proc = None
        try:
            self.socket.bind((self.host, self.port))
        except (socket.error, msg):
            print('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
            sys.exit()

        if self.debug:
            print('Socket bind complete')

        self.socket.listen(10)
        if self.debug:
            print('Socket now listening')

        while True:
            try:
                conn, addr = self.socket.accept()
                print('Connected with ' + addr[0] + ':' + str(addr[1]))
                writer_proc = Process(target=self.__decoder, args=((conn),(child_pipe),addr[0]))
                writer_proc.start()
                db = sh.open(self.ip_log_location, writeback=True)
                db['IPs'] = [] if 'IPs' not in db else db['IPs']+[addr[0]]
                print(db['IPs'])
                db.close()
            except KeyboardInterrupt:
                print("\n'KeyboardInterrupt' received. Stopping server.")
                if writer_proc.is_alive():
                    writer_proc.join()
                break
            except:
                raise
        writer_proc.join()
        self.socket.close()


if __name__ == '__main__':
    if len(sys.argv) == 2:
        port = sys.argv[1]
    elif len(sys.argv) == 3:
        addr = sys.argv[1]
        port = int(sys.argv[2])
    #server = connection_extended(addr,port, 'tempFiles/connectedIPs.db', debug=True)
    server = connection(addr,port, debug=True)
    parent_conn, child_conn = Pipe()
    dp = dictParser()
    print("server listening to %r:%r" %(addr,port))
    getterproc = Process(target=dp.run , args=((child_conn), addr+':'+str(8000) ) )
    print("GUI running at %r:%r/monitoringGui/index.html" %(addr, 8000))
    getterproc.start()
    server.listen(parent_conn)
    if getterproc.is_alive():
        getterproc.join()