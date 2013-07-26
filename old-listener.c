#include <stdio.h>
#include <time.h>
#include <linux/input.h>

#define MAX_INPUT 30
#define PST (+8)

int main(void) {
	FILE *listening;
	char str[MAX_INPUT];

	listening = fopen("overheard.txt", "a");
	setvbuf(listening, NULL, _IONBF, 0);
	printf("Start typing...");
	gets(str);
	printf("Your input: %s\n", str);
	fwrite(str, 1, sizeof(str), listening);

	fclose(listening);

	int renfile;
	time_t rawtime;
	struct tm *gmt;

	time(&rawtime);
	/* getting GMT time */
	// gmt = gmtime(&rawtime);
	// char currentime[] = ("%2d:%02d:%02d", (gmt->tm_hour+PST)%24, gmt->tm_min, gmt->tm_sec);
	// printf("%s", currentime);
	char oldname[] = "overheard.txt";
	char newname[] = "compname_timestamp.txt";

	renfile = rename(oldname, newname);

	return 0;
}