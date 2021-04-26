#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <ctype.h>
#include "node.h"

#ifndef _DATAH_
#define _DATAH_

typedef struct Attributes
{
	char* name;
	char* values[1000];
	int num_values;
	bool numerical;
} Attributes;

typedef struct DataFrame
{
	unsigned long long maxsize;
	int col;
	int row;
	Attributes** attributes;
	char** dataset;
} DataFrame;

/*
 * Purpose: read dataset from a file
 * Parameters: file - a pointer referencing the input file,
 *			dataframe - a pointer referencing a DataFrame struct
 * Return value: none
 */
void readData(FILE *file, DataFrame *dataframe);

void getUniqueAttributeValues(DataFrame *dataframe);

void handleMissingValue(DataFrame* dataframe);

void printDataframe(DataFrame* dataframe);

/*
 * Purpose: free up the memory used by the dataframe
 * Parameters: dataframe - a pointer referencing a DataFrame struct
 * Return value: none
 */
void freeDataFrame(DataFrame* dataframe);

unsigned seedCurTime();

#endif