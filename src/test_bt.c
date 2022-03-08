/*
 * test_bt.c
 *
 *  Created on: 22 feb. 2022
 *      Author: p.flopez@alumnos.upm.es
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int scan_chars();
int read_char();
int write_char();

int main() {
	scan_chars();
	/*read_char();
	write_char();
	read_char();*/

	return 0;
}

int scan_chars() {
	int size = 100;
	char output[size];
	FILE *fp = popen("gatttool -b 30:C6:F7:00:22:EE --characteristics", "r");
	//FILE *fp = popen("ls", "r");
	//FILE *fp = popen("gatttool -b 78:21:84:7E:56:BA --characteristics", "r");

	while (fgets(output, size, fp) != NULL)
	    printf("Características disponibles:\n%s", output);

	pclose(fp);
	return 0;
}

int read_char() {
	int size = 100;
		char output[size];
		char mac[] = "30:C6:F7:00:22:EE";
		char uuid[] = "beb5483e-36e1-4688-b7f5-ea07361b26a9";
		char input[1024];

		snprintf(input, 1024, "gatttool -b %s --char-read --uuid=%s", mac, uuid);

		FILE *fp = popen(input, "r");

		while (fgets(output, size, fp) != NULL)
		    printf("%s", output);

		pclose(fp);
		return 0;
}

int write_char() {
	int size = 100;
		char output[size];
		char mac[] = "30:C6:F7:00:22:EE";
		char handler[]="0x002a";
		char value[]="0x001";
		char input[1024];

		snprintf(input, 1024, "gatttool -b %s --char-write-req --handle=%s --value=%s", mac, handler, value);

		FILE *fp = popen(input, "r");

		while (fgets(output, size, fp) != NULL)
		    printf("%s", output);

		pclose(fp);
		return 0;
}

