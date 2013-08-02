from keymap import keys, cap_keys, shift_keys
# CHANGE ALL KEYMAP LIST REFERENCES TO DICTIONARY
shift = False
evlst = []

log = open('evlog.txt')
f = log.read()
log.close()

evlog = f.split('-')

for ev in evlog:
	if len(ev) > 5:
		continue
	ev = ev.split(',')
	try:
		ev[0] = int(ev[0])
		ev[1] = int(ev[1])
		evlst.append(ev)
	except:
		continue

# num = for loop iterator, kl = keylog, key_sym = key symbol
def counter(num, kl, key_sym):
	if kl[num][0] != key_sym:
		return 0
	return 2 + counter(num-1, kl, key_sym)


def del_count(num, c, kl):
	if c == 0:
		return
	del kl[num]
	del_count(num-1, c-1, kl)


# YOU ARE TRAVERSING IN REVERSE REVERSE REVERSE!!!!
for n in reversed(xrange(len(evlst))):
	if keys[evlst[n][0]] == '[SHIFT]' and evlst[n][1] == 0:
		shift = True
		del evlst[n]
		continue
	elif keys[evlst[n][0]] == '[SHIFT]' and evlst[n][1] == 1:
		shift = False
		del evlst[n]
		continue

	if shift:
		if evlst[n][1] == 0:
			del evlst[n]
			continue
		evlst[n] = shift_keys[evlst[n][0]]
	else:
		if evlst[n][1] == 0:
			del evlst[n]
			continue
		evlst[n] = keys[evlst[n][0]]

	if evlst[n] == '[ENTER]':
			evlst[n] = ' [ENTER]\n'

	# delete depressed state
	# function for backspace
	# count the number of backspaces and then delete that number of 
	# indices above
	# if evlst[n][0] == '[BACKSPACE]':
	# 	c = counter(n, evlst, '[BACKSPACE]')
	# 	print c
	# 	del_count(n, c, evlst)

# print evlst
print ''.join(evlst)
