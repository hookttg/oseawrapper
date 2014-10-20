/*
 * oseawrapper: Simple tab-delimited input/output for the Open Source ECG Analyzer (OSEA)
 * Copyright (C) 2014 James Mardell <james.mardell@imperial.ac.uk>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

/* External function prototypes from OSEA */
void ResetBDAC(void);
int BeatDetectAndClassify(int ecg_sample, int *beat_type, int *beat_match);

/* External interpretation structures */
const char *beat_type_string[] = {"No beat detected", "Normal", "?", "?", "?", "Premature ventricular contraction", "?", "?", "?", "?", "?", "?", "?", "Unknown"};

int main(int argc, char **argv) 
{
	/* File variables */
	FILE *input_data = NULL;
	FILE *output_data = NULL;
	int use_stdout = 0;

	/* Check arguments */
	if ((argc < 2) || (argc > 3)) {
		fprintf(stderr, "Usage: %s [input datafile] (output datafile)\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	if (access(argv[1], R_OK ) == -1) {
		fprintf(stderr, "Unable to open file '%s' for reading. Aborting.\n", argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Open files */
	input_data = fopen(argv[1], "r");
	if (argc == 2) {
		use_stdout = 1;
	} else {
		output_data = fopen(argv[2], "w");
	}

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
	
	double *float_samples = NULL;
	float_samples = (double *) malloc(number_of_samples * sizeof(double) );
	if (float_samples == NULL) {
		fprintf(stderr, "Unable to allocate enough memory to store %d samples from '%s'. Aborting.\n", number_of_samples, argv[1]);
		exit(EXIT_FAILURE);
	}

	/* Dump contents of input_data into memory */
	double sample;
	int index = 0;

	while (!feof(input_data) ) {
		
		if (fscanf(input_data, "%lf\n", &sample) != 1) {
			break;
		} else {
			float_samples[index] = sample;
			index++;
		}
	} /* while feof */

	/* Sanity check for input data loading */
	if (index != number_of_samples) {
		fprintf(stderr, "Parsed %d samples, but expected to find %d samples. Did '%s' change? Continuing.\n", index, number_of_samples, argv[1]);
	}
	
	/* Convert samples to integers for OSEA */
	/* Find largest number */
	double maximum_sample = 0;
	for (index = 0; index < number_of_samples; index++) {
		if (float_samples[index] > maximum_sample) maximum_sample = float_samples[index];
	}

	for (index = 0; index < number_of_samples; index++) {
		samples[index] = (int)floor((float_samples[index] / maximum_sample) * 1000);
	}

	/* Run OSEA */
	int beat_type = -1;
	int beat_match = -1;
	int delay = -1;
	ResetBDAC();

	if (use_stdout) {
		fprintf(stdout, "#index\tdelay\ttype\tmatch(dbg)\n");
	} else {
		fprintf(output_data, "#index\tdelay\ttype\tmatch(dbg)\n");
	}

	for (index = 0; index < number_of_samples; index++) {
		delay = BeatDetectAndClassify(samples[index], &beat_type, &beat_match);

		if (delay != 0) {
			if (use_stdout) {
				fprintf(stdout, "%d\t%d\t%s\t%d\n", index, delay, beat_type_string[beat_type], beat_match);
			} else {
				fprintf(output_data, "%d\t%d\t%s\t%d\n", index, delay, beat_type_string[beat_type], beat_match);
			}
		}
	} /* for index..number_of_samples */

	/* Clean up */
	free(samples);
	samples = NULL;

	fclose(input_data);
	if (!use_stdout) fclose(output_data);

	exit(EXIT_SUCCESS);
}
