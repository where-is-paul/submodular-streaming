#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>

#include <stdio.h>
#include <stdlib.h>

// Roll our own to be platform indep.
#include "getopt.h"

#include "data_source.h"
#include "greedy_streamer.h"
#include "random_greedy_streamer.h"
#include "multilevel_streamer.h"

using namespace std;

int main(int argc, char* argv[]) {
	extern char *optarg;
	static char usage[] = "usage: %s -f filename\n";

	int c, flags = 0;
	string filename;
	while ((c = getopt(argc, argv, "f:")) != -1) {
		switch (c) {
			case 'f':
				flags = 1;
				filename = string(optarg);
				break;
			case '?':
				break;
		}
	}

	if (flags < 1) {	/* invalid option */
		fprintf(stderr, "%s: missing input file\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	ifstream config_file(filename);
	cerr << filename << endl;
	size_t N, K, W, DIM;
	string type, header;
	
	// Header format is: 
	// (type of input, stream size, cardinality constraint, window size, dim)
	getline(config_file, header);
	stringstream ss(header);
	ss >> type >> N >> K >> W >> DIM;
	cerr << "Problem type: " << type << endl;
	cerr << "(n, k, w, dim) = (" << N << ", " << K << ", " << W << ", " << DIM << ")" << endl;

	MetricEvaluator* metric = nullptr;
	BaseSource* source = nullptr;
	if (type == "random") {
		source = new RandomSource(N, DIM);
		metric = new SimilarityMetric;
	} else if (type == "embedding") {
		source = new FileSource(filename);
		metric = new SimilarityMetric;
	} else if (type == "geolocation") {
		source = new FileSource(filename);
		metric = new GeodistanceMetric;
	} else if (type == "binary") {
		source = new BinarySource(filename);
		cerr << "Done setting up source" << endl;
		metric = new SimilarityMetric;
	} else {
		assert(false && "Unknown problem type!");
	}

	//////////////////////////////////////////////

	N = 1000;
	K = 10;
	GreedyStreamer gstream(K, metric);
	RandomGreedyStreamer rstream(K, metric);
	MultiLevelStreamer mstream(K, metric);

	cout << "***Sequential model tests***" << endl;
	auto tgi = chrono::steady_clock::now();
	for (size_t i = 0; i < N; i++) {
		gstream.add(source->data_at(i));
		rstream.add(source->data_at(i));
		mstream.add(source->data_at(i));
	}

	cout << "Items added successfully." << endl;
	cout << "The greedy objective is: " << gstream.query() << endl;
	cout << "The random greedy objective is: " << rstream.query() << endl;
	cout << "The multi-level objective is: " << mstream.query() << endl;

	auto tgf = chrono::steady_clock::now();
	cout << "Total time: " << chrono::duration_cast<chrono::microseconds>(tgf - tgi).count() << " µs" << endl;
	// cout << "Solution: ";
	// for (int x : gstream.solution()) {
	// 	cout << x << " ";
	// }
	// cout << endl;

	//////////////////////////////////////////////

	// string alg[] = {"greedy", "max-trace", "windowed"};
	// for (int i = 0; i < 3; i++) {
	// 	ofstream value_file("value_" + alg[i] + ".txt");
	// 	ofstream solution_file("solution_" + alg[i] + ".txt");
	// 	for (auto kv : outputValue[i]) {
	// 		value_file << kv.first << " " << kv.second << "\n";
	// 	}
	// 	for (auto kv : outputSolution[i]) {
	// 		solution_file << kv.first << " ";
	// 		for (auto id : kv.second) {
	// 			solution_file << id << " ";
	// 		}
	// 		solution_file << "\n";
	// 	}
	// 	value_file.close();
	// 	solution_file.close();
	// }

	delete source;
	delete metric;
	return 0;
}