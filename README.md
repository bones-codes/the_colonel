## The Colonel Rootkit
Colonel is an experimental linux kernel module (rootkit). Remote communication is handled through IRC. The rootkit is able to hide processes, files, grant root privileges and activate key logging.

## Installation
The Colonel should only be run in a virtual machine. Keylogging is not available on Vagrant.  
_Note: [server](../blob/master/irc/bot.py#L37), [channel](../blob/master/irc/bot.py#L39) and [nickname](../blob/master/irc/bot.py#L40) should be set in irc/bot.py prior to installation._

Install –  `./install`  
Remove –  `./uninstall`  

**Requirements:**
* Linux 'vanilla' Kernel >= 2.6.29 _– tested up to 3.6_

## Usage
**From infected computer:**

To pass commands use the included program: `sudo ./rtcmd <command>` or echo: `echo -n <command> >> /proc/colonel`  
To see available commands: `sudo ./rtcmd help` or `cat /proc/colonel`  
_Custom /proc file will not be visible on content listing of /proc._


**From IRC:**

If in channel, preface all commands with bot nickname and `:`, i.e. `bot-nickname: <command>`.  
In private messages or DCC sessions, commands should be passed without prefix.  
Use `help` to see the  available bot and root commands: `bot-nickname: help`


## Resources
* [The Linux Kernel Module Programming Guide](http://www.tldp.org/LDP/lkmpg/2.6/html/)
* [Linux Daemon Writing HOWTO](http://www.netzmafia.de/skripten/unix/linux-daemon-howto.html)
* [irc 8.5.1](https://pypi.python.org/pypi/irc)
