from keymap import keys, cap_keys, shift_keys


caps = False
shift = False
evlst = []

log = open('evlog.txt')
f = log.read()
log.close()

evlog = f.split('-')

# Splits the received keylog into a list.
for ev in evlog:
	if len(ev) > 5:
		evlst.append(ev)
		continue
	ev = ev.split(',')
	try:
		ev[0] = int(ev[0])
		ev[1] = int(ev[1])
		evlst.append(ev)
	except:
		continue


# Accepts the loop iterator (num) and the key symbol 
# to be counted (key_sym). Counts the number of 
# occurences of the specified key_sym.
def counter(num, key_sym):
	if evlst[num] != key_sym:
		return 0
	return 2 + counter(num-1, key_sym)


# Accepts the count (c) and loop iterator (num), 
# then deletes that number of items beginning with
# the list index determined by the iterator.
def del_count(num, c):
	if c == 0:
		return num
	del evlst[num]
	del_count(num-1, c-1)


def which_keymap(num, km):
	# For cases when a letter is held down.
	if evlst[num][1] == 2:
		evlst.insert(num+1, km[evlst[num][0]])
		evlst[num] = km[evlst[num][0]]
	else:
		evlst[num] = km[evlst[num][0]]


# Translates evlst. BACKSPACE (Buggy)
for n in reversed(xrange(1, len(evlst))):
	# Prints the date and system information that is not user output.
	if len(evlst[n]) > 5:
		continue

	if keys[evlst[n][0]] == '[SHIFT]' and evlst[n][1] == 0:
		shift = True
		del evlst[n]
		continue
	elif keys[evlst[n][0]] == '[SHIFT]' and evlst[n][1] == 1:
		shift = False
		del evlst[n]
		continue
	
	if evlst[n][1] == 0:
		del evlst[n]
		continue
	
	if keys[evlst[n][0]] == '[CAPSLOCK]' and caps == False:
		caps = True
		del evlst[n]
		continue
	elif keys[evlst[n][0]] == '[CAPSLOCK]' and caps == True:
		caps = False
		del evlst[n]
		continue

	# Determining which keymap to use to translate the log.
	if shift:
		which_keymap(n, shift_keys)
	elif shift and caps:
		which_keymap(n, shift_keys)
	elif caps:
		which_keymap(n, cap_keys)
	else:
		which_keymap(n, keys)

	if evlst[n] == '[ENTER]':
		evlst[n] = ' [ENTER]\n'

i = len(evlst)-1
while i >= 0:
	if evlst[i] == '[BACKSPACE]':
		c = counter(i, '[BACKSPACE]')
		del_count(i, c)
		i -= c
	i -= 1

print ''.join(evlst)
