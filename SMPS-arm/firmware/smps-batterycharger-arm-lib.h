/* STM32F1 SMPS Battery Charger Library Functions

Initial 1 February 2014
*/

/*
 * This file is part of the smps-batterycharger project.
 *
 * Copyright 2014 K. Sarkies <ksarkies@internode.on.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SMPS_BATTERYCHARGER_LIB_H_
#define SMPS_BATTERYCHARGER_LIB_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void intToAscii(int32_t value, char* buffer);
int32_t asciiToInt(char* buffer);
void stringAppend(char* string, char* appendage);
void stringCopy(char* string, char* original);
uint16_t stringLength(char* string);
uint16_t stringEqual(char* string1,char* string2);

#endif

