#!/usr/bin/env python2
from __future__ import print_function
import time, lttng, babeltrace, sys

class lttngSesstion():
	def __init__(self, sess_type, **kwargs):
		self.name = 'untitled_'+sess_type if kwargs.get('name')==None else kwargs.get('name')
		self.path = '/lttng-traces/'+self.name if kwargs.get('path')==None else kwargs.get('path')+'/'+self.name
		# Making sure session does not already exist
		lttng.destroy(self.name)
		# Creating a new session and handle
		ret = lttng.create(self.name,self.path)
		if ret < 0:
			raise LTTngError(lttng.strerror(ret))
		#Creating LTTng session domain
		self.domain = lttng.Domain()
		if sess_type=='kernel':
			self.domain.type = lttng.DOMAIN_KERNEL
		elif sess_type=='ust':
			self.domain.type = lttng.DOMAIN_UST
		else:
			print("invalid tracing session type '%r', session not created" %(sess_type))
		#Enabling channel
		self.channel = lttng.Channel()
		self.channel.name="channel_1"
		'''
		channel.attr.overwrite = 0
		channel.attr.subbuf_size = 4096
		channel.attr.num_subbuf = 8
		channel.attr.switch_timer_interval = 0
		channel.attr.read_timer_interval = 200
		channel.attr.output = EVENT_MMAP
		'''
		#creating handle
		self.handle = None
		self.handle = lttng.Handle(self.name, self.domain)
		if self.handle is None:
			raise LTTngError("Handle not created")
		#enabling channel on handle
		lttng.enable_channel(self.handle, self.channel)
		# Enabling all events
		self.event = lttng.Event()
		self.event.type = lttng.EVENT_ALL
		self.event.loglevel_type = lttng.EVENT_LOGLEVEL_ALL
		print
		ret = lttng.enable_event(self.handle, self.event, self.channel.name)
		if ret < 0:
			raise LTTngError(lttng.strerror(ret))

	def start(self):
		ret = lttng.start(self.name)
		if ret < 0:
			raise LTTngError(lttng.strerror(ret))

	def stop(self):
		ret = lttng.stop(self.name)
		if ret < 0:
			raise LTTngError(lttng.strerror(ret))

	def destroy(self):
		ret = lttng.destroy(self.name)
		if ret < 0:
			raise LTTngError(lttng.strerror(ret))

	class preExistingSession():
		def __init__(self, name, path, kwargs):
			pass



if __name__=='__main__':
	lts = lttngSesstion('ust') if len(sys.argv)<2 else lttngSesstion('ust', name=str(sys.argv[1]))
	print('LTTng session %r created at path \n%r' %(lts.name,lts.path))
	lts.start()
	print('LTTng session %r started' %(lts.name))
	time.sleep(10)
	lts.stop()
	print('LTTng session %r stopped' %(lts.name))
	lts.destroy()
	print('LTTng session %r destroy' %(lts.name))