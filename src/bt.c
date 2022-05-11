#include "bt.h"

extern char **checked_macs;

// Stablishes connection with a BLE device and reads the specified characteristic.
int read_char (char *mac, char *output) {
	char input[1024];

	snprintf(input, 1024, "gatttool -b %s --char-read --uuid=%s", mac, READ_UUID);

	FILE *cmd_output = popen(input, "r");
	if (cmd_output == NULL) {
		printf("Error abriendo la conexión con la consola.");
		output = "No ha sido posible conectar con el dispositivo.";
		return -1;
	}

	while (fgets(output, 256, cmd_output) != NULL)
	    printf("%s", output);

	output[strcspn(output, "\n\r")] = 0;

	pclose(cmd_output);
	return 0;
}

// Stablishes connection with a BLE device and writes the given value to the specified characteristic.
int write_char (char *mac, char *value, char *output) {
	char input[1024];

	snprintf(input, 1024, "gatttool -b %s --char-write-req --handle=0x002d --value=%s", mac, value);

	FILE *cmd_output = popen(input, "r");
	if (cmd_output == NULL) {
		printf("Error abriendo la conexión con la consola.");
		return -1;
	}

	while (fgets(output, sizeof(output), cmd_output) != NULL)
	    printf("%s", output);

	pclose(cmd_output);
	return 0;
}

// Check an array of strings in order to remove any line that does not match with a valid MAC address.
int check_macs (int device_number, char **to_check) {
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
			checked_macs[n] = to_check[i];
			n++;
		}
	}
	return n;
}