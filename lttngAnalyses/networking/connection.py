#!/usr/bin/env python3
import threading
import time
import socket
import sys
import os
import pickle
from multiprocessing import Process, Pipe

class connection():
	def __init__(self, host, port, **kwargs):
		self.debug = kwargs["debug"] if "debug" in kwargs else False
		self.stopafter = kwargs["stopafter"] if "stopafter" in kwargs else None
		self.host = host
		self.port = port
		self.thread_from = None
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		if self.debug:
			print('Server socket created')


	def __decoder(self, conn, child_pipe):
		fdict = {}
		while True:
			data = conn.recv(2048)
			if not data:
				if self.debug:
					print('Connected thread ending from %r' %(self.thread_from))
					child_pipe.send({'msg':'END_OF_Q'})
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
				writer_proc = Process(target=self.__decoder, args=((conn),(child_pipe),))
				writer_proc.start()
			except KeyboardInterrupt:
				print("\n'KeyboardInterrupt' received. Stopping server.")
				if writer_proc.is_alive():
					writer_proc.join()
				break
			except:
				raise
		writer_proc.join()
		self.socket.close()


	def connect(self, serverip, serverport):
		ipname = {'msg': 'Connection made from ' + socket.gethostbyname(socket.gethostname()) + ':' + str(self.port)}
		self.socket.connect((serverip , serverport))
		self.socket.send(pickle.dumps(ipname, -1)) #send only takes dictionary/JSON objects
		return self.socket
		

	def send(self,dataToSend):
		self.socket.sendall(pickle.dumps(dataToSend, -1))

	def close(self):
		self.socket.close()