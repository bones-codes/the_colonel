#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(void) {
	FILE *fp = NULL;
	pid_t process_id = 0;
	pid_t sid = 0;

	process_id = fork();			/* fork a child process */

	if (process_id < 0) {
		printf("ERROR: fork() failed.\n");
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

	chdir("log/");					/* change daemon working 
									 * directory to root */
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	fp = fopen("log.txt", "a+"); 	/* open daemon log */
	while(1) {
		sleep(1);
		fprintf(fp, "Logging info.......\n");
		fflush(fp);
		// Implement and call some function that does core work for this daemon
	}

	fclose(fp);
	return 0;
}