import linecache
import fnmatch
import re


keymap = []
cap_keymap = []
cap_shift_keymap = []
shift_keymap = []

special = ['RESERVED', 'ESC', 'BACKSPACE',  'CAPSLOCK', 'F1', 'F2', 'F3', 
'F4', 'F5', 'F6', 'F7', 'F8', 'F9', 'F10', 'NUMLOCK', 'SCROLLLOCK', 'F11',
'F12', 'SYSRQ', 'LINEFEED', 'UP', 'PAGEUP', 'LEFT', 'RIGHT', 'END', 'DOWN', 
'PAGEDOWN', 'INSERT', 'DELETE', 'MACRO', 'MUTE', 'VOLUMEDOWN', 'VOLUMEUP', 
'POWER', 'PAUSE', 'SCALE', 'HANGEUL', 'COMPOSE', 'STOP', 'AGAIN', 'PROPS', 
'UNDO', 'FRONT', 'COPY', 'OPEN', 'PASTE', 'FIND', 'CUT', 'HELP', 'MENU', 
'CALC', 'SETUP', 'SLEEP', 'WAKEUP', 'FILE', 'SENDFILE', 'DELETEFILE', 'XFER', 
'PROG1', 'PROG2', 'WWW', 'MSDOS', 'COFFEE', 'SCREENLOCK', 'DIRECTION', 
'CYCLEWINDOWS', 'MAIL', 'BOOKMARKS', 'COMPUTER', 'BACK', 'FORWARD', 
'CLOSECD', 'EJECTCD', 'EJECTCLOSECD', 'HOME','NEXTSONG', 'PLAYPAUSE', 
'PREVIOUSSONG', 'STOPCD', 'RECORD', 'REWIND', 'PHONE', 'ISO', 'CONFIG', 
'HOMEPAGE', 'REFRESH', 'EXIT', 'MOVE','EDIT', 'SCROLLUP', 'SCROLLDOWN', 
'NEW', 'REDO', 'F13', 'F14', 'F15', 'F16', 'F17', 'F18', 'F19', 'F20', 'F21', 
'F22', 'F23', 'F24', 'PLAYCD', 'PAUSECD', 'PROG3', 'PROG4', 'DASHBOARD', 
'SUSPEND', 'CLOSE', 'PLAY', 'FASTFORWARD', 'BASSBOOST', 'PRINT', 'HP', 
'CAMERA', 'SOUND', 'QUESTION', 'EMAIL', 'CHAT', 'SEARCH', 'CONNECT', 
'FINANCE', 'SPORT', 'SHOP', 'ALTERASE', 'CANCEL', 'BRIGHTNESSDOWN', 
'BRIGHTNESSUP', 'MEDIA', 'SWITCHVIDEOMODE', 'KBDILLUMTOGGLE', 'KBDILLUMDOWN', 
'KBDILLUMUP', 'SEND', 'REPLY', 'FORWARDMAIL', 'SAVE', 'DOCUMENTS', 'BATTERY', 
'BLUETOOTH', 'WLAN', 'UWB', 'UNKNOWN', 'VIDEO_NEXT', 'VIDEO_PREV', 
'BRIGHTNESS_CYCLE', 'BRIGHTNESS_ZERO', 'DISPLAY_OFF', 'WIMAX', 'RFKILL']

sym = {'KPLEFTPAREN': '(', 'KPRIGHTPAREN': ')','ENTER': '[ENTER]', 
'KPENTER': '[ENTER]', 'TAB': ' [TAB]	', 'RIGHTCTRL': '[CTRL]', 
'LEFTCTRL': '[CTRL]', 'MINUS': '-', 'EQUAL': '=', 'LEFTBRACE': '[', 
'RIGHTBRACE': ']', 'SEMICOLON': ';', 'APOSTROPHE': "'", 'GRAVE': '`', 
'RIGHTALT': '[ALT]', 'BACKSLASH': '[BACKSLASH]', 'COMMA': ',', 'DOT': '.', 
'SLASH': '/', 'KPASTERISK': '*', 'SPACE': ' ', 'KP7': '7', 'KP8': '8', 
'KP9': '9', 'KPMINUS': '-', 'KP4': '4', 'KP5': '5', 'KP6': '6', 
'KPPLUS': '+', 'KP1': '1', 'KP2': '2', 'KP3': '3', 'KP0': '0', 
'KPDOT': '.', 'KPSLASH': '/', 'KPEQUAL': '=', 'KPPLUSMINUS': '+-', 
'KPCOMMA': ',', 'LEFTMETA': 'COMMAND', 'RIGHTMETA': 'COMMAND', 
'LEFTSHIFT': '[SHIFT]', 'RIGHTSHIFT': '[SHIFT]', 'LEFTALT': '[ALT]', 
'Q': 'q', 'W': 'w', 'E': 'e', 'R': 'r', 'T': 't', 'Y': 'y', 'U': 'u', 
'I': 'i', 'O': 'o', 'P': 'p', 'A': 'a', 'S': 's', 'D': 'd', 'F': 'f', 
'G': 'g', 'H': 'h', 'J': 'j', 'K': 'k', 'L': 'l', 'Z': 'z', 'X': 'x', 
'C': 'c', 'V': 'v', 'B': 'b', 'N': 'n', 'M': 'm'}

cap_sym = {'KPLEFTPAREN': '(', 'KPRIGHTPAREN': ')', 'ENTER': '[ENTER]', 
'KPENTER': '[ENTER]', 'TAB': ' [TAB]	', 'RIGHTCTRL': '[CTRL]', 
'LEFTCTRL': '[CTRL]', 'MINUS': '-', 'EQUAL': '=', 'LEFTBRACE': '[', 
'RIGHTBRACE': ']', 'SEMICOLON': ';', 'APOSTROPHE': "'",'GRAVE': '`', 
'RIGHTALT': '[ALT]','BACKSLASH': '[BACKSLASH]', 'COMMA': ',', 'DOT': '.', 
'SLASH': '/', 'KPASTERISK': '*', 'SPACE': ' ', 'KP7': '7', 'KP8': '8', 
'KP9': '9', 'KPMINUS': '-', 'KP4': '4', 'KP5': '5', 'KP6': '6', 'KPPLUS': '+', 
'KP1': '1', 'KP2': '2', 'KP3': '3', 'KP0': '0', 'KPDOT': '.', 'KPSLASH': '/', 
'KPEQUAL': '=', 'KPPLUSMINUS': '+-', 'KPCOMMA': ',', 'LEFTMETA': 'COMMAND', 
'RIGHTMETA': 'COMMAND', 'LEFTSHIFT': '[SHIFT]', 'RIGHTSHIFT': '[SHIFT]', 
'LEFTALT': '[ALT]'}

cap_shift_sym = {'KPLEFTPAREN': '(', 'KPRIGHTPAREN': ')', 'ENTER': '[ENTER]', 
'KPENTER': '[ENTER]', 'TAB': ' [TAB]	', 'RIGHTCTRL': '[CTRL]', 
'LEFTCTRL': '[CTRL]', 'MINUS': '_', 'EQUAL': '+', 'LEFTBRACE': '{', 
'RIGHTBRACE': '}', 'SEMICOLON': ':', 'APOSTROPHE': '\\\"', 'GRAVE': '~', 
'RIGHTALT': '[ALT]', 'BACKSLASH': '|', 'COMMA': '<', 'DOT': '>', 
'SLASH': '?', 'KPASTERISK': '*', 'SPACE': ' ', 'KP7': '7', 'KP8': '8', 
'KP9': '9', 'KPMINUS': '-', 'KP4': '4', 'KP5': '5', 'KP6': '6', 
'KPPLUS': '+', 'KP1': '1', 'KP2': '2', 'KP3': '3', 'KP0': '0', 'KPDOT': '.', 
'KPSLASH': '/', 'KPEQUAL': '=', 'KPPLUSMINUS': '+-', 'KPCOMMA': ',', 
'LEFTMETA': 'COMMAND', 'RIGHTMETA': 'COMMAND', 'LEFTSHIFT': '[SHIFT]', 
'RIGHTSHIFT': '[SHIFT]', 'LEFTALT': '[ALT]', '1': '!', '2': '@', '3': '#', 
'4': '$', '5': '%', '6': '^', '7': '&', '8': '*', '9': '(', '0': ')',
'Q': 'q', 'W': 'w', 'E': 'e', 'R': 'r', 'T': 't', 'Y': 'y', 'U': 'u', 
'I': 'i', 'O': 'o', 'P': 'p', 'A': 'a', 'S': 's', 'D': 'd', 'F': 'f', 
'G': 'g', 'H': 'h', 'J': 'j', 'K': 'k', 'L': 'l', 'Z': 'z', 'X': 'x', 
'C': 'c', 'V': 'v', 'B': 'b', 'N': 'n', 'M': 'm'}

shift_sym = {'KPLEFTPAREN': '(', 'KPRIGHTPAREN': ')', 'ENTER': '[ENTER]', 
'KPENTER': '[ENTER]', 'TAB': ' [TAB]	', 'RIGHTCTRL': '[CTRL]', 
'LEFTCTRL': '[CTRL]', 'MINUS': '_', 'EQUAL': '+', 'LEFTBRACE': '{', 
'RIGHTBRACE': '}', 'SEMICOLON': ':', 'APOSTROPHE': '\\\"', 'GRAVE': '~', 
'RIGHTALT': '[ALT]', 'BACKSLASH': '|', 'COMMA': '<', 'DOT': '>', 
'SLASH': '?', 'KPASTERISK': '*', 'SPACE': ' ', 'KP7': '7', 'KP8': '8', 
'KP9': '9', 'KPMINUS': '-', 'KP4': '4', 'KP5': '5', 'KP6': '6', 
'KPPLUS': '+', 'KP1': '1', 'KP2': '2', 'KP3': '3', 'KP0': '0', 'KPDOT': '.', 
'KPSLASH': '/', 'KPEQUAL': '=', 'KPPLUSMINUS': '+-', 'KPCOMMA': ',', 
'LEFTMETA': 'COMMAND', 'RIGHTMETA': 'COMMAND', 'LEFTSHIFT': '[SHIFT]', 
'RIGHTSHIFT': '[SHIFT]', 'LEFTALT': '[ALT]', '1': '!', '2': '@', '3': '#', 
'4': '$', '5': '%', '6': '^', '7': '&', '8': '*', '9': '(', '0': ')'}


def keyed(pyf, km, sym_list):
	for s in km:
		if s == km[len(km)-1]:
			last = '"[' + s + ']"]'
			pyf.write(last)
			break
		elif s in special:
			btn = '"[' + s + ']",\n'
			pyf.write(btn)
		elif s in sym_list:
			punc = '"' + sym_list[s] + '",\n'
			pyf.write(punc)
		else:
			entry = '"' + s + '", \n'
			pyf.write(entry)
	return km

# DYNAMICALLY FIND linux-input.h
for i in range(183, 442):
	linput = linecache.getline('linux-input.h', i).split()
	key = fnmatch.filter(linput, 'KEY*')
	try:
		key = re.sub(r'KEY_', "", key[0])
		keymap.append(key)
		cap_keymap.append(key)
		cap_shift_keymap.append(key)
		shift_keymap.append(key)
	except:
		continue

f = open("keymap.py", "w")

f.write("keys = [\n")  					# Create keymap (unmodified).
keyed(f, keymap, sym)

f.write("\n\ncap_keys = [\n") 			# Create capslock keymap.
keyed(f, cap_keymap, cap_sym)

f.write("\n\ncap_shift_keys = [\n")		# Create cap/shift keymap.
keyed(f, cap_shift_keymap, cap_shift_sym)

f.write("\n\nshift_keys = [\n")			# Create shift keymap.
keyed(f, shift_keymap, shift_sym)

f.close()
