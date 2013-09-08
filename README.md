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
The rootkit is an experimental linux kernel module written in C. 

Upon [installation](#installation), the rootkit, along with any properly prefixed files, is hidden. 
A custom /proc entry is also created and subsequently hidden. Communication with the rootkit is accomplished by passing commands to the custom /proc entry. The custom /proc entry also displays accepted methods of passing commands, rootkit commands, and current rootkit status. You will also find accepted methods of passing commands outlined in [Usage](#usage).  

The [rootkit hides](../master/lkm/rootkit.c#L52-L65) itself by deleting its placement within the kobject, and modules listing. Prior to deletion, the rootkit stores its placement. This enables the rootkit to 'show' itself on command by reinserting its entry into the listings. The hiding of the custom /proc entry, processes, and files is accomplished by the [modification of page memory attributes](../master/lkm/rootkit.c#L82-L96) and passing in customized functions that target the [/proc](../master/lkm/rootkit.c#L100-L119) and [file system](/master/lkm/rootkit.c#L121-L132) directory listings. The process ids (PIDs) are stored within an array that is referenced by new_proc_readdir whenever a process related command is sent. If the PID is found within the array, it is not returned. This method of hiding leaves process related commands intact, i.e. `ls`, `ps`, `lsof`, `netstat`, `kill`. Both the custom /proc entry and files are hidden by name and prefix.  

During [rootkit removal](/master/lkm/rootkit.c#L292-L295), all modified functions are restored, the custom /proc entry is deleted, and any hidden PIDs and files are revealed.

_In researching the rootkit build, I focused my efforts on [module programming](http://www.tldp.org/LDP/lkmpg/2.6/html/), and other linux rootkits – specifically Ormi's tutorial on [Writing a Simple Linux Rootkit](http://w3.cs.jmu.edu/kirkpams/550-f12/papers/linux_rootkit.pdf). Since my modifications are fairly lightweight, and the implementation fairly straightforward, most of my personal involvement was in commenting to ensure that I understood what was occurring._

<a name="keylogger"/>
**Keylogger:**  
The keylogger is a C daemon. 

The Colonel's keylogger, once activated, records keyboard input to a log in a hidden directory in /opt. The input is captured from the keyboard's /dev/input/event file. The keylogger also records its activity, and any errors, to a log within the same hidden directory. 

Once installed, the keylogger hides it's PID, creates the required directory and logs, and begins listening to the custom /proc entry that was created by the rootkit. When the appropriate command is passed to the custom /proc entry, keylogging is activated. Keycodes and their values are captured from the /dev/input/event file and written to /opt/__col_log/evlog.txt (keylog). The keylogger also logs its activity, as well as any errors, to /opt/__col_log/log.txt.

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
* Linux 'vanilla' Kernel >= 2.6.29 _– tested up to 3.2_

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
