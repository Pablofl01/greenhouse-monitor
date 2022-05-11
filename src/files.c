#include "files.h"

extern char **checked_macs;
extern int checked_devices;

char **macs;

int initializeFiles(char *file) {
	int device_number = countLines(file);

	if (device_number == -1) {
		printf("Error recuperando el número de dispositivos.\n");
		return -1;
	}

	printf("Número de dispositivos listados: %d\n", device_number);

	macs = malloc(device_number);
	for (int i=0; i<device_number; i++) {
		macs[i] = malloc(MAX_MAC_LENGTH);
	}

	if (readLines(file, macs) == -1) {
		printf("Error recuperando las direcciones de los dispositivos.\n");
		return -2;
	}

	checked_macs = (char**) malloc(device_number);
	for (int i=0; i<device_number; i++) {
		checked_macs[i] = (char*) malloc(MAX_MAC_LENGTH);
	}

	checked_devices = check_macs(device_number, macs);

	//free(macs);

	if (checked_devices == -1) return -1;
	
	return 0;
}

//Count lines in a file in order to allocate enough memory for storing its content.
int countLines(char *filename)
{
	FILE *filePointer;
	int file_lines = 1;

	filePointer = fopen(filename, "r");
	if (filePointer == NULL)
	{
		printf("Error abriendo el fichero: %s\n", strerror(errno));
		return -1;
	}

	char ch;

	while ((ch = fgetc(filePointer)) != '\377')
	{
		if (ch == '\n')
			file_lines++;
	}

	fclose(filePointer);

	return file_lines;
}

// Function that reads a file line by line and dumps it into an array of strins passed as parameter, one element per line.
int readLines(char *filename, char **read)
{
	FILE *filePointer;

	filePointer = fopen(filename, "r");
	if (filePointer == NULL)
	{
		printf("Error abriendo el fichero.\n");
		return -1;
	}

	int n = 0;
	ssize_t lines;
	size_t len = 0;

	while ((lines = getline(&read[n], &len, filePointer)) != -1)
	{
		read[n][strcspn(read[n], "\n\r")] = 0;
		n++;
	}

	fclose(filePointer);

	return 0;
}

// Function that writes content to a file.
int writeLines(char *filename, char *write)
{
	FILE *filePointer;

	filePointer = fopen(filename, "a");
	if (filePointer == NULL)
	{
		printf("Error abriendo el fichero.\n");
		return -1;
	}

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	if ((fprintf(filePointer, "[%d-%02d-%02d %02d:%02d:%02d] %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, write)) < 0)
	{
		fclose(filePointer);
		return -2;
	}

	fclose(filePointer);
	return 0;
}

int writeValues(char *filename, int write, int device_id)
{
	FILE *filePointer;

	filePointer = fopen(filename, "a");
	if (filePointer == NULL)
	{
		printf("Error abriendo el fichero.\n");
		return -1;
	}

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

	if ((fprintf(filePointer, "[%d-%02d-%02d %02d:%02d:%02d] Valor sensor %d: %d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, device_id, write)) < 0)
	{
		fclose(filePointer);
		return -2;
	}

	fclose(filePointer);
	return 0;
}