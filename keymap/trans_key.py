from keymap import keys, cap_keys, shift_keys

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

# num = for loop iterator, km = keymap, kl = keylog, key_sym = key symbol
def counter(num, km, kl, key_sym):
	if km[kl[num][0]] != key_sym:
		return 2
	return 2 + counter(num-1, km, kl, key_sym)


def del_count(num, c, kl):
	if c == 0:
		return
	del kl[num]
	del_count(num-1, c-1, kl)


for n in reversed(xrange(len(evlst))):
	# delete depressed state
	if evlst[n][1] == 1 and evlst[n][1] is not '[SHIFT]':
		del evlst[n]
		continue
	# function for backspace
	# count the number of backspaces and then delete that number of 
	# indices above
	if keys[evlst[n][0]] == '[BACKSPACE]':
		c = counter(n, keys, evlst, '[BACKSPACE]')
		del_count(n, c, evlst)
		continue


	evlst[n] = keys[evlst[n][0]]
# print evlst
# print ''.join(evlst)
