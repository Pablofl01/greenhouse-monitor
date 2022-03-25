#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <regex.h>
#include <wiringPi.h>
#include "files.h"
#include "bt.h"

// Some needed constants.
#define MAX_LINE_LENGTH 256
#define MAX_MAC_LENGTH 17
#define WRITE_VALUE "0x001"
const char *file = "devices.txt";

char **macs;
char **checked_macs;
char *data, *result;

int main() {

	int device_number = count_lines(file);

	if (device_number == -1) {
		printf("Error recuperando el número de dispositivos.\n");
		return -1;
	}

	printf("Número de dispositivos listados: %d\n", device_number);

	macs = malloc(device_number);
	for (int i=0; i<device_number; i++) {
		macs[i] = malloc(MAX_MAC_LENGTH);
	}

	if (read_lines(file, macs) == -1) {
		printf("Error recuperando las direcciones de los dispositivos.\n");
		return -2;
	}

	/**for (int i=0; i<device_number; i++) {
		printf("%s\n", macs[i]);
	}**/

	checked_macs = (char**) malloc(device_number);
	for (int i=0; i<device_number; i++) {
		checked_macs[i] = (char*) malloc(MAX_MAC_LENGTH);
	}

	if (check_macs(device_number, macs, checked_macs) == 0) {
		for (int i=0; i<device_number; i++) {
			if (strcmp(checked_macs[i], "") != 0) write_lines("output.txt", checked_macs[i]);
		}
	}

	data = malloc(MAX_LINE_LENGTH);
	result = malloc(MAX_LINE_LENGTH);

	for (int i=0; i<device_number; i++) {
		if (strcmp(checked_macs[i], "") != 0) {
			write_char(checked_macs[i], WRITE_VALUE, result);
			read_char(checked_macs[i], data);
			write_lines("output.txt", (char*) data);
		}
	}

	free(data);
	free(macs);
	free(checked_macs);

	return 0;
}