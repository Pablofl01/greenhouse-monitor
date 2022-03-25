#include "files.h"

//Count lines in a file in order to allocate enough memory for storing its content.
int count_lines (char *filename) {
	FILE *filePointer;
	int file_lines = 1;

	filePointer = fopen(filename, "r");
	if (filePointer == NULL) {
		printf("Error abriendo el fichero: %s\n", strerror(errno));
		return -1;
	}

	char ch;

	while ((ch = fgetc(filePointer)) != '\377') {
      if (ch == '\n') file_lines++;
    }

	fclose(filePointer);
	
	return file_lines;
}

// Function that reads a file line by line and dumps it into an array of strins passed as parameter, one element per line.
int read_lines (char *filename, char **read) {
	FILE *filePointer;

	filePointer = fopen(filename, "r");
	if (filePointer == NULL) {
		printf("Error abriendo el fichero.\n");
		return -1;
	}

	int n = 0;
	ssize_t lines;
    size_t len = 0;

    while ((lines = getline(&read[n], &len, filePointer)) != -1) {
		read[n][strcspn(read[n], "\n\r")] = 0;
		n++;
    }

	fclose(filePointer);

	return 0;
}

// Function that writes content to a file.
int write_lines (char *filename, char *write) {
	FILE *filePointer;

	filePointer = fopen(filename, "a");
	if (filePointer == NULL) {
		printf("Error abriendo el fichero.\n");
		return -1;
	}

	time_t t = time(NULL);
  	struct tm tm = *localtime(&t);

	if ((fprintf(filePointer,"[%d-%02d-%02d %02d:%02d:%02d] %s\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, write)) < 0) {
		fclose(filePointer);
		return -2;
	} 

	fclose(filePointer);
	return 0;
}