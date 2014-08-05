#!/usr/bin/env python3
import threading
import time
import socket
import sys
 
HOST = ''   # Symbolic name meaning all available interfaces
PORT = 5000 # Arbitrary non-privileged port
 
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print('Socket created')
 
try:
    s.bind((HOST, PORT))
except (socket.error, msg):
    print('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
    sys.exit()
     
print('Socket bind complete')
 
s.listen(10)
print('Socket now listening')
 
#Function for handling connections. This will be used to create threads
def clientthread(conn):
    #Sending message to connected client
    conn.send(('Welcome to the server. Type something and hit enter\n').encode('utf-8')) #send only takes string
    #infinite loop so that function do not terminate and thread do not end.
    while True:
        #Receiving from client
        data = conn.recv(1024)
        print(data)
        print(data.decode(encoding='UTF-8'))
        reply = 'OK...' + data.decode('utf-8')
        if data.decode(encoding='utf-8') == str('exit').encode(encoding='utf-8'):
        	print("it did")
        	break
     
        conn.sendall(reply.encode('utf-8'))
    #came out of loop
    conn.close()

#now keep talking with the client
while True:
    #wait to accept a connection - blocking call
    conn, addr = s.accept()
    print('Connected with ' + addr[0] + ':' + str(addr[1]))
    #start new thread takes 1st argument as a function name to be run, second is the tuple of arguments to the function.
    #start_new_thread(clientthread ,(conn,))
    threading.Thread(target=clientthread , args=[conn]).start()
 
s.close()