/*
 * files.h
 *
 *  Created on: 1 mar. 2022
 *      Author: p.flopez@alumnos.upm.es
 */

#ifndef FILES_H_
#define FILES_H_



int read_file(const char *filename) {
	FILE *filePointer;

	char fileContents;

	filePointer = fopen(filename, "r");
	fscanf(filePointer, "%s", &fileContents);
	printf("%s", fileContents);
	fclose(filePointer);

	return 0;
}

#endif /* FILES_H_ */
