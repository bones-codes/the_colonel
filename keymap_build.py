# MAKE C LIST DECLARATION MORE EFFICIENT
import linecache
import fnmatch
import re

keymap = []

for i in range(183, 442):
	linput = linecache.getline("linux-input.h", i).split()
	key = fnmatch.filter(linput, 'KEY*')
	try:
		keymap.append(key[0])
	except:
		continue

keylen = len(keymap) + 1;

f = open("keymap.h", "w")
f.write("#include <stdio.h>\n\nchar KeyMap[%d][30] = {\n" % keylen)

for key in keymap:
	k = re.sub(r'KEY_', "", key)
	entry = '"' + k + '",' + "\n"
	f.write(entry)

f.write("};")

f.close()
