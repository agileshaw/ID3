#include "data.h"

void readData(FILE *file, DataFrame *dataframe)
{
	int i = 0, j = 0, count = 0;
	char line[2048];
	char *tok, *tmp;

	dataframe->col = 0;
	dataframe->row = 0;

	while (fgets(line, 2048, file)) {
		if (line[0] != '\n')
			count++;
	}
	fseek(file, 0, SEEK_SET);

	if (fgets(line, 2048, file)) {
		tmp = strdup(line);

		for (tok = strtok(tmp, ","); tok && *tok; tok = strtok(NULL, ",\n"))
			dataframe->col++;
		printf("Columns: %d\n", dataframe->col);
		free(tmp);

		dataframe->attributes = malloc(sizeof(Attributes*) * dataframe->col);
		tmp = strdup(line);
		for (tok = strtok(tmp, ","); tok && *tok; tok = strtok(NULL, ",\n")) {
			dataframe->attributes[i] = malloc(sizeof(Attributes));
			dataframe->attributes[i]->name = malloc(60 * sizeof(char));
			strcpy(dataframe->attributes[i]->name, tok);
			dataframe->attributes[i]->num_values = 0;
			i++;
		}
		free(tmp);
	}

	dataframe->maxsize = (count-1) * dataframe->col;
	dataframe->row = count - 1;
	j = 0;
	dataframe->dataset = malloc(sizeof(char*) * dataframe->maxsize);
	while (fgets(line, 2048, file)) {
		if (line[0] != '\n') {
			tmp = strdup(line);
			for (tok = strtok(tmp, ","); tok && *tok; tok = strtok(NULL, ",\n")) {
				dataframe->dataset[j] = malloc(60 * sizeof(char));
				strcpy(dataframe->dataset[j], tok);
				j++;
			}
			free(tmp);
		}
	}
	printf("Rows: %d\n", dataframe->row);
	getUniqueAttributeValues(dataframe);
}

void getUniqueAttributeValues(DataFrame *dataframe)
{
	int i, j, k, num = 0, found = 0;
	bool check;

	while (num < (dataframe->row * dataframe->col)) {
		i = num % dataframe->col;
		k = dataframe->attributes[i]->num_values;

		if (!strcmp(dataframe->dataset[num], "?") ||
			!strcmp(dataframe->dataset[num], " ") ||
			strlen(dataframe->dataset[num]) == 0) {
			//do nothing
		} else {
			for (j = 0; j < k; j++) {
				if (!strcmp(dataframe->attributes[i]->values[j], dataframe->dataset[num])){
					found = 1;
				}
			}

			if (dataframe->attributes[i]->num_values == 0) {
				dataframe->attributes[i]->values[k] = malloc(60 * sizeof(char));
				strcpy(dataframe->attributes[i]->values[0], dataframe->dataset[num]);
				dataframe->attributes[i]->num_values++;
				found = 1;
			}

			if (found != 1) {
				dataframe->attributes[i]->values[k] = malloc(60 * sizeof(char));
				strcpy(dataframe->attributes[i]->values[k], dataframe->dataset[num]);
				dataframe->attributes[i]->num_values++;
			}
		}
		num++;
		found = 0;
	}

	for (i = 0; i < dataframe->col; i++) {
		printf("%s: ", dataframe->attributes[i]->name);
		check = true;
		for (j = 0; j < dataframe->attributes[i]->num_values; j++) {
			printf("'%s' ", dataframe->attributes[i]->values[j]);
			if (check) {
				for (k = 0; k < strlen(dataframe->attributes[i]->values[j]); k++)
					if (isdigit(dataframe->attributes[i]->values[j][k]) == 0) {
						check = false;
					}
			}
		}
		dataframe->attributes[i]->numerical = check;
		printf("(Numerical? %s)", dataframe->attributes[i]->numerical ? "true" : "false"); //check if the feature contains numerical values
		printf("\n");
	}
}

void handleMissingValue(DataFrame* dataframe)
{
	int i, tmp;

	for (i = 0; i < dataframe->maxsize; i++) {
		if (!strcmp(dataframe->dataset[i], "?") ||
			!strcmp(dataframe->dataset[i], " ") ||
			strlen(dataframe->dataset[i]) == 0){
				tmp = rand()/(RAND_MAX/(dataframe->attributes[i % dataframe->col]->num_values));
				memset(dataframe->dataset[i], 0, strlen(dataframe->dataset[i]));
				strcpy(dataframe->dataset[i], dataframe->attributes[i % dataframe->col]->values[tmp]);
		}
	}
}

void printDataframe(DataFrame* dataframe)
{
	int i;

	for (i = 0; i < dataframe->col; i++) {
		printf("%s ", dataframe->attributes[i]->name);
	}

	for (i = 0; i < dataframe->maxsize; i++) {
		if (i%dataframe->col == 0)
			printf("\n");
		printf("%s ", dataframe->dataset[i]);
	}
	printf("\n");

}

void freeDataFrame(DataFrame* dataframe)
{
	int i, j, k;

	for (i = 0; i < dataframe->col; i++) {
		if (dataframe->attributes[i]->name) {
			free(dataframe->attributes[i]->name);
			for (j = 0; j < dataframe->attributes[i]->num_values; j++) {
				free(dataframe->attributes[i]->values[j]);
			}
		}
		free(dataframe->attributes[i]);
	}
	free(dataframe->attributes);

	for (k = 0; k < (dataframe->row * dataframe->col); k++) {
		free(dataframe->dataset[k]);
	}
	free(dataframe->dataset);
}


unsigned seedCurTime()
{
	unsigned seed = 0;
	time_t cur_time = time(0);
	unsigned char *tmp;
	size_t i;

	tmp = (unsigned char *)&cur_time;
	for (i = 0; i < sizeof(cur_time); i++)
		seed = seed *(UCHAR_MAX) + tmp[i];

	return seed;
}
