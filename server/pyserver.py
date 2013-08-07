import socket
import datetime
import threading 
import os

class Server(threading.Thread):
	def __init__(self, port):
		threading.Thread.__init__(self)
		self.port = port

	def now(self):
		d = datetime.datetime.now()
		return d.strftime("%d/%m/%y %H:%M:%S")

	def run(self):
		host = ''
		s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		s.bind((host, self.port))
		print 'server running...'
		s.listen(1)
		conn, addr = s.accept()
		print 'contact', addr, 'on', self.now()

		while 1:
			try:
				data = conn.recv(1024)
			except socket.error:
				print 'lost', addr, '...'
				s.listen(1)
				conn, addr = s.accept()
				print 'contact', addr, 'on', self.now()
				continue

			if not data:
				print 'lost', addr, '...'
				s.listen(1)
				conn, addr = s.accept()
				print 'contact', addr, 'on', self.now()
			else:
				print 'received msg:', data
				conn.send('roger')

t = Server(31944)
t.start()
