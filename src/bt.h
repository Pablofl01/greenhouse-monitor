/*
 * bt.h
 *
 *  Created on: 14 mar. 2022
 *      Author: p.flopez@alumnos.upm.es
 */

#ifndef BT_H_
#define BT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>

#define READ_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define WRITE_HANDLER "0x002a"

//int scan_chars();
int read_char (char *mac, int output);
int write_char (char *mac, char *value, char *output);
int check_macs (int device_number, char **to_check, char **checked);

#endif // BT_H_
