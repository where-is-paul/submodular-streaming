// This file converts the problem file file to a specialized compressed binary format
// which is significantly smaller and faster to read. This is required for some large
// test streams.

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdio.h>
#include <stdlib.h>

// Roll our own to be platform indep.
#include "getopt.h"

using namespace std;

int main(int argc, char* argv[]) {
	extern char *optarg;
	static char usage[] = "usage: %s -i input_file -o output_name (produces output_name.prob, output_name.bin)\n";

	int c, flags = 0;
	string input_file, output_name;
	while ((c = getopt(argc, argv, "i:o:")) != -1) {
		switch (c) {
			case 'i':
				flags += 1;
				input_file = string(optarg);
				break;
			case 'o':
				flags += 1;
				output_name = string(optarg);
				break;
			case '?':
				break;
		}
	}

	if (flags < 2) {	/* invalid option */
		fprintf(stderr, "%s: missing arguments\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	// Read the file into a list of vectors and store it in memory
	// Skip the first line because we use it to describe the problem instance
	ifstream config_file(input_file);
	string header, type;
	getline(config_file, header);

	size_t N, K, W, DIM;
	stringstream ss(header);
	ss >> type >> N >> K >> W >> DIM;


	ofstream binary_info(output_name + ".prob");
	binary_info << "binary " << N << " " << K << " " << W << " " << DIM << "\n";
	binary_info << output_name + ".bin";
	binary_info.close();

	ofstream binary_output(output_name + ".bin", ios::out | ios::binary);
	for (size_t i = 0; i < N; i++) {
		double inp;
		for (size_t j = 0; j < DIM; j++) {
			config_file >> inp;
			binary_output.write(reinterpret_cast<char*>(&inp), sizeof(double));
		}
	}
	binary_output.close();

  	return 0;
}
