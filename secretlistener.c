#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
	int key;

	if(argc < 2) {
		printf("USAGE: %s <device>\n", argv[0]);
		return 1;
	}
	key = open(argv[1], "r");
	struct input_event ev;

	while(1) {
		read(key, &ev, sizeof(struct input_event));

	if(ev.type == 1) {
		printf("key %i state %i\n", ev.code, ev.value);
	}
	}

	return 0;
}