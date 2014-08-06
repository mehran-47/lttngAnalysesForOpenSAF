#!/usr/bin/env python3
import threading
import time
import socket
import sys

class connection():
	def __init__(self, host, port, debug=True):
		self.debug = debug
		self.host = host
		self.port = port
		self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		if debug:
			print('Server socket created')


	def __decoder(self, conn):
		while True:
			data = conn.recv(1024)
			if not data:
				if self.debug:
					print('Connected thread ending')
				conn.close()
				return
			print(data.decode(encoding='UTF-8'))


	def listen(self):
		msg = ''
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
			conn, addr = self.socket.accept()
			print('Connected with ' + addr[0] + ':' + str(addr[1]))
			threading.Thread(target=self.__decoder , args=[conn]).start()
		self.socket.close()


	def send(self, serverip, serverport, dataToSend):
		ipname = 'Connection made from ' + socket.gethostbyname(socket.gethostname()) + str(self.port)
		self.socket.connect((serverip , serverport))
		self.socket.send((ipname).encode('utf-8')) #send only takes string
		self.socket.sendall(dataToSend.encode('utf-8'))
		self.socket.close()