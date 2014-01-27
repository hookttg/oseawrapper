#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* External function prototypes from OSEA */
void ResetBDAC(void);
int BeatDetectAndClassify(int ecg_sample, int *beat_type, int *beat_match);

int main(int argc, char **argv) 
{
	/* File variables */
	FILE *input_data;
	FILE *output_data;

	/* Check arguments */
	if (argc != 3) {
		fprintf(stderr, "Usage: %s [input datafile] [output datafile]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (access(argv[1], R_OK ) == -1) {
		fprintf(stderr, "Unable to open file '%s' for reading. Aborting.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/*if (access(argv[2], W_OK) == -1) {
		fprintf(stderr, "Unable to open file '%s' for writing. Aborting.\n", argv[2]);
		exit(EXIT_FAILURE);
	}*/

	/* Open files */
	input_data = fopen(argv[1], "r");
	output_data = fopen(argv[2], "w");

	if (input_data == NULL) {
		fprintf(stderr, "Error reading input file '%s'. Aborting.\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	int sample = -1;
	int index = 0;

	while (!feof(input_data) ) {
		index++;
		
		if (fscanf(input_data, "%d", &sample) != 1) {
			break;
		} else {
			fprintf(stdout, "%d\t%d\n", index, sample);
		}
	} /* while feof */

	fprintf(stderr, "Parsed %d samples.\n", index);

	/* Clean up */
	fclose(input_data);
	fclose(output_data);

	exit(EXIT_SUCCESS);
}
