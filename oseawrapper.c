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

	/* Open files */
	input_data = fopen(argv[1], "r");
	output_data = fopen(argv[2], "w");

	if (input_data == NULL) {
		fprintf(stderr, "Error reading input file '%s'. Aborting.\n", argv[1]);
		exit(EXIT_FAILURE);
	}
	
	/* Find out the number of samples in the input file */
	/* http://stackoverflow.com/a/4278875 */
	int number_of_samples = 0;
	while (EOF != (fscanf(input_data, "%*[^\n]"), fscanf(input_data, "%*c")))
		++number_of_samples;
	rewind(input_data); /* Rewind the file pointer for reading */

	/* Allocate memory for input data */
	int *samples = NULL;
	samples = (int *) malloc(number_of_samples * sizeof(int) );
	if (samples == NULL) {
		fprintf(stderr, "Unable to allocate enough memory to store %d samples from '%s'. Aborting.\n", number_of_samples, argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Dump contents of input_data into memory */
	int sample = -1;
	int index = 0;

	while (!feof(input_data) ) {
		
		if (fscanf(input_data, "%d", &sample) != 1) {
			break;
		} else {
			samples[index++] = sample;
		}
	} /* while feof */

	/* Sanity check for input data loading */
	if (index != number_of_samples) {
		fprintf(stderr, "Parsed %d samples, but expected to find %d samples. Did '%s' change? Continuing.\n", index, number_of_samples, argv[1]);
	}

	/* Run OSEA */
	int beat_type = -1;
	int beat_match = -1;
	int delay = -1;
	ResetBDAC();

	fprintf(stdout, "#index\tdelay\ttype\tmatch\n");

	for (index = 0; index < number_of_samples; index++) {
		delay = BeatDetectAndClassify(samples[index], &beat_type, &beat_match);

		fprintf(stdout, "%d\t%d\t%d\t%d\n", index, delay, beat_type, beat_match);
	}

	/* Clean up */
	free(samples);
	samples = NULL;

	fclose(input_data);
	fclose(output_data);

	exit(EXIT_SUCCESS);
}
