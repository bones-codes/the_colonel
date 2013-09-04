## The Colonel Rootkit
[Documentation](#documentation) | [Installation](#installation) | [Usage](#usage) | [Resources](#resources)  

Colonel is an experimental linux kernel module (rootkit) with an integrated keylogger. Remote communication is handled through the included IRC bot. The rootkit is able to:  
* activate key logging
* grant root privileges
* hide files
* hide processes


<a name="documentation"/>
## Documentation (draft - editing needed)
[Rootkit](#rootkit) | [Keylogger](#keylogger) | [IRC Bot](#irc)  

Add showterm and diagram.

<a name="rootkit"/>
**Rootkit:**
C, LKM, kernel mode. Installed via social engineering. Modifies the page memory attributes and passes in modified fs and proc functions. Creates custom kernel entry - receives commands from custom entry.

<a name="keylogger"/>
**Keylogger:**
C, daemon, userland. Logs keycodes and values of released keys to log in hidden directory. Includes error log. File path. Receives commands via /proc/colonel. 

<a name="irc"/>
**IRC Bot:**
Python, IRC Library, python-daemon, userland. File path? Reference keymap/build and keylog translation module. Listens for commands in channel traffic, private messages and DCC sessions. Writes commands to /proc/colonel.

<a name="installation"/>
## Installation
Installation and removal are accomplished via shell scripts. The Colonel should only be run in a virtual machine. Keylogging is not available on Vagrant.   
_Note: [server](../master/irc/col_bot#L36), [channel](../master/irc/col_bot#L38) and [nickname](../master/irc/col_bot#L39) should be set in irc/bot.py prior to installation._

1. Download the [zip file](../archive/master.zip) or `git clone https://github.com/cara-bones/colonel.git`
2. From /colonel run the `./install` command.  
3. To remove, run `./uninstall` from /colonel.

**Requirements:**
* Linux 'vanilla' Kernel >= 2.6.29 _– tested up to 3.6_

<a name="usage"/>
## Usage
**Local:**

To pass commands use the included program: `./rtcmd <command>` or echo: `echo -n <command> >> /proc/colonel`  
To see available commands: `./rtcmd help` or `cat /proc/colonel`  
_Custom /proc file will not be visible on content listing of /proc._


**Remote:**

If in channel, preface all commands with bot nickname and `:`, i.e. `bot-nickname: <command>`.  
In private messages or DCC sessions, commands should be passed without prefix.  
Use `help` to see the  available bot and root commands: `bot-nickname: help`

<a name="resources"/>
## Resources
* [The Linux Kernel Module Programming Guide](http://www.tldp.org/LDP/lkmpg/2.6/html/)
* [Linux Daemon Writing HOWTO](http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html)
* [irc 8.5.1](https://pypi.python.org/pypi/irc)
