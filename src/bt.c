/*
 * bt.c
 *
 *  Created on: 14 mar. 2022
 *      Author: p.flopez@alumnos.upm.es
 */

#include "bt.h"
/*#include <stdio.h>
#include <regex.h>*/

// TO-DO
// Scans available characteristics of a BLE device.
/**int scan_chars(char mac[100][18]) {
	int size = 100;
	char output[size];
	for (int i=0; i < 100; i++) {
		if (mac[i] != NULL) {
			char cmd[50] = "gatttool -b ";
			strcat(cmd, mac[i]);
			strcat(cmd, " --characteristics");
			FILE *fp = popen(cmd, "r");
			while (fgets(output, size, fp) != NULL)
			printf("%s - Características disponibles:\n%s", mac[i][0], output);
		}
	}
	FILE *fp = popen("gatttool -b 30:C6:F7:00:22:EE --characteristics", "r");
	//FILE *fp = popen("ls", "r");
	//FILE *fp = popen("gatttool -b 78:21:84:7E:56:BA --characteristics", "r");

	while (fgets(output, size, fp) != NULL)
	    printf("Características disponibles:\n%s", output);

	pclose(fp);
	return 0;
}**/

// Stablishes connection with a BLE device and reads the specified characteristic.
int read_char (char *mac, int output) {
	int size = 100;
		/**char output[size];
		char mac[] = "30:C6:F7:00:22:EE";
		char uuid[] = "beb5483e-36e1-4688-b7f5-ea07361b26a9";**/
		char input[1024];

		snprintf(input, 1024, "gatttool -b %s --char-read --uuid=%s", mac, READ_UUID);

		FILE *cmd_output = popen(input, "r");
		if (cmd_output == NULL) {
			printf("Error abriendo la conexión con la consola.");
			return -1;
		}

		while (fgets((char*)output, size, cmd_output) != NULL)
		    printf("%d", output);

		pclose(cmd_output);
		return 0;
}

// Stablishes connection with a BLE device and writes the given value to the specified characteristic.
int write_char (char *mac, char *value, char *output) {
	int size = 100;
		/**char output[size];
		char mac[] = "30:C6:F7:00:22:EE";
		char handler[]="0x002a";
		char value[]="0x001";**/
		char input[1024];

		snprintf(input, 1024, "gatttool -b %s --char-write-req --handle=%s --value=%s", mac, WRITE_HANDLER, value);

		FILE *cmd_output = popen(input, "r");
		if (cmd_output == NULL) {
			printf("Error abriendo la conexión con la consola.");
			return -1;
		}

		while (fgets(output, size, cmd_output) != NULL)
		    printf("%s", output);

		pclose(cmd_output);
		return 0;
}

// Check an array of strings in order to remove any line that does not match with a valid MAC address.
int check_macs (int device_number, char **to_check, char **checked) {
	regex_t regex;

	int result = regcomp(&regex, "([a-fA-F0-9]{2}:){5}[a-fA-F0-9]{2}", REG_EXTENDED|REG_NOSUB);
	if (result != 0) {
		printf("Error compilando la expresión regular");
		return -1;
	}

	int n = 0;

	for (int i=0; i<device_number; i++) {
		int match = regexec(&regex, to_check[i], (size_t) 0, NULL, 0);
		if (match == 0) {
			checked[n] = to_check[i];
			n++;
		}
	}

	return 0;
}