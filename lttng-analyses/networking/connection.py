#!/usr/bin/env python3
import threading
import queue
import time
import socket
import sys
import os
import pickle

class connection():
	def __init__(self, host, port, **kwargs):
		self.debug = kwargs["debug"] if "debug" in kwargs else False
		self.stopafter = kwargs["stopafter"] if "stopafter" in kwargs else None
		self.host = host
		self.port = port
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.queue = queue.Queue(maxsize=0)
		if self.debug:
			print('Server socket created')


	def __decoder(self, conn):
		fdict = {}
		while True:
			data = conn.recv(1024)
			if not data:
				if self.debug:
					print('Connected thread ending from %r' %os.uname()[1])
				conn.close()
				break
				return
			fdict = pickle.loads(data)
			#string_received = data.decode(encoding='UTF-8')
			#string_received = ast.literal_eval(string_received)
			#fdict = json.dumps(string_received)
			print(fdict)


	def listen(self):
		msg = ''
		listeners = []
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
				threading.Thread(target=self.__decoder , args=[conn]).start()
			except KeyboardInterrupt:
				print("\n'KeyboardInterrupt' received. Stopping server.")
				break
			except:
				raise
		self.socket.close()


	def connect(self, serverip, serverport):
		ipname = {'msg': 'Connection made from ' + socket.gethostbyname(socket.gethostname()) + str(self.port)}
		self.socket.connect((serverip , serverport))
		self.socket.send(pickle.dumps(ipname, -1)) #send only takes dictionary/JSON objects
		return self.socket
		

	def send(self,dataToSend):
		self.socket.sendall(pickle.dumps(dataToSend, -1))

	def close(self):
		self.socket.close()