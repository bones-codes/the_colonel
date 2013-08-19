Colonel is an experimental linux kernel module (rootkit). Remote communication is handled through IRC. The rootkit is able to hide processes, files, grant root privileges and activate key logging.

Requirements:

- Linux 'vanilla' Kernel >= 2.6.29 _– tested up to 3.6_


## Installation
Note: channel, server and nickname should be set in irc/bot.py prior to installation.

...


## Usage
From infected computer:

`cat /proc/colonel` to see available commands. 

To pass commands use the included program: `./rtcmd <command>` 
or echo: `echo -n <command> >> /proc/colonel`


From IRC:

Use the help command to see available bot commands: `[nickname]: help`


## Resources
[http://linux.die.net/lkmpg](http://linux.die.net/lkmpg)
[http://www.tldp.org/LDP/lkmpg/2.6/html/](http://www.tldp.org/LDP/lkmpg/2.6/html/)