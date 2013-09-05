## The Colonel Rootkit
[Demo](showterm.io/13720f013d95a0ceeb05f#fast) | [Documentation](#documentation) | [Installation](#installation) | [Usage](#usage) | [Resources](#resources)  

Colonel is an experimental linux kernel module (rootkit) and keylogger. Remote communication is handled through the included IRC bot. The Colonel is able to:  
* log keyboard input
* grant root privileges
* hide files
* hide processes


<a name="documentation"/>
## Documentation (draft - editing needed)
[Rootkit](#rootkit) | [Keylogger](#keylogger) | [IRC Bot](#irc)  

**TODO:** Add diagram.

The following is an overview of the three main components of the Colonel.

<a name="rootkit"/>
**Rootkit:**  
The rootkit is a linux kernel module (lkm) written in C. Upon installation, the lkm, along with any properly prefixed files, is hidden. 
A custom /proc entry is also created and subsequently hidden. Communication with the lkm is accomplished by passing commands to the custom /proc entry.  

The [lkm hides](../master/lkm/rootkit.c#L52-L65) itself by noting its placement within the kobject, and modules listing. The hiding of the custom /proc entry, processes, and files is accomplished by the [modification of page memory attributes](../master/lkm/rootkit.c#L82-L96) and passing in customized functions that target the [/proc](../master/lkm/rootkit.c#L100-L119) and [file system](/master/lkm/rootkit.c#L121-L132) directory listings. This method of hiding leaves process related commands intact, i.e. `ls`, `ps`, `lsof`, `netstat`, `kill`.  

The custom /proc entry displays accepted rootkit commands, methods of passing commands, and the current rootkit status. This is accomplished by modifying the /proc entry's [proc_read function](../master/lkm/rootkit.c#L134-L179). All passed commands are processed through the /proc entry's modified [proc_write function](../master/lkm/rootkit.c#L181-L213).  

**TODO:** Add references/resources

<a name="keylogger"/>
**Keylogger:**  
The keylogger is a daemonized C program. Activation/deactivation is accomplished by passing the appropriate command to the rootkit's custom /proc entry. Keyboard entries (keycodes of released keys) are captured from the /dev/input/event file and written to /opt/__col_log/evlog.txt (keylog). The keylogger also logs its activity, as well as any errors, to /opt/__col_log/log.txt.

_Both [directory](../master/lkm/col_kl.c#L50-L52) and keylogger [PID](../master/lkm/col_kl.c#L71-L88) are automatically hidden upon installation._

<a name="irc"/>
**IRC Bot:**  
The IRC bot is a daemon based on the python IRC framework. Commands can be passed through channel traffic, private messages, and DCC sessions. Bot PID is automatically hidden upon installation.  

Commands that are not bot-specific are written to the custom rootkit /proc entry. [Keylog translation](,,/master/irc/col_bot#L109-L116) is handled through the translate function in [irc/key.py](../master/irc/key.py#L57-L109).

_IRC bot [PID](../master/irc/col_bot#L256-L262) is hidden on installation._

<a name="installation"/>
## Installation
Installation and removal are accomplished via shell scripts. The Colonel should only be run in a virtual machine. Keylogging is not available on Vagrant.   
_Note: server, channel and nickname should be set in [irc/col_bot](../master/irc/col_bot#L36-L39) prior to installation._

1. `git clone https://github.com/cara-bones/colonel.git`
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
