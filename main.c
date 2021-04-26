#include "main.h"

int main(int argc, char *argv[])
{
	clock_t begin, end;
	DataFrame dataframe;
	unsigned seed = seedCurTime();
	int opt, read = 0, leaf = 0, size = 0, tf = 0;
	char d[LENGTH], s[SEEDLEN], treefile[LENGTH];

	begin = clock();
	printf("Generated seed value: %u\n", seed);

	while ((opt = getopt(argc, argv, "d:s:t:")) != -1) {
        switch (opt) {
			case 'd':
	            read = 1;
				snprintf(d, LENGTH, "%s", optarg);
				break;
			case 't':
				tf = 1;
				snprintf(treefile, LENGTH, "%s", optarg);
				break;
			case 's':
				snprintf(s, SEEDLEN, "%s", optarg);
				seed = strtoul(s, NULL, 10);
	            printf("Input seed: %u\n", seed);
	            break;
	        case '?':
	            write(STDOUT_FILENO, "Usage: ./classifier [-d datafile] [-s seed] [-t treefile_folder_path]\n", 70);
	            exit(1);
		}
	}

	if (read) {
		FILE* file = fopen(d, "r+");
		if(file == NULL) {
			perror("Unable to open file!");
			exit(1);
		}
		readData(file, &dataframe);
		fclose(file);
	} else {
		FILE *file = fopen("data/adult+stretch.csv", "r+");
		if(file == NULL) {
			perror("Unable to open file!");
			exit(1);
		}
		readData(file, &dataframe); //read data file file and build a dataframe
		fclose(file);
	}

	if(!tf)
		snprintf(treefile, LENGTH, ".");

	srand(seed);

	handleMissingValue(&dataframe); //handle missing values with random assignment

	RuleTree* tree = buildTree(&dataframe, NULL, NULL); //infer a decision tree

	snprintf(treefile+strlen(treefile), LENGTH-strlen(treefile), "/id3-s%u-f%d-n%d.txt", seed, (dataframe.col - 1), dataframe.row);
	FILE *tree_file = fopen(treefile, "w+");
	if(tree_file == NULL) {
		perror("Unable to open tree_file!");
		exit(1);
	}
	printRuleTreeToFile(tree_file, &dataframe, tree, &leaf, &size); //print decision tree to file
	fclose(tree_file);

	//free up allocated memory
	destroyRuleTree(tree);
	freeDataFrame(&dataframe);

	end = clock();
	executionTime(begin, end); //print the total run time

	return 0;
}

void executionTime(clock_t begin, clock_t end)
{
	double time_elapsed;
	int hr, min;
	float sec;

	time_elapsed = ((double) (end - begin)) / CLOCKS_PER_SEC;

	hr = time_elapsed / 3600;
	min = (time_elapsed - hr * 3600) / 60;
	sec = time_elapsed - hr * 3600 - min * 60;

	printf("Total execution time: %d hours, %d minuetes and %f seconds.\n\n", hr, min, sec);
}
