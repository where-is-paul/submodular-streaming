#include <bits/stdc++.h>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

using namespace std;

double f1(int i) {
	return i + 1;
}

double f2(int i) {
	return pow(i + 1, 0.5);
}

double f3(int i) {
	return 1000;
}

double f0(int i) {
	return abs(sin(i));
}


int main(int argc, char* argv[]) {
	extern char *optarg;
	static char usage[] = "usage: %s [-k num_opt]\n";

	int c, flags = 0, k = 100;
	while ((c = getopt(argc, argv, "k:")) != -1) {
		switch (c) {
			case 'k':
				flags = 1;
				k = atoi(optarg);
				break;
			default:
				cout << "No such option found!" << endl;
		}
	}

	if (flags < 0) {	/* missing mandatory options */
		fprintf(stderr, "%s: missing options\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	int n = 100 * k;
	vector<double> input;
	double opt = 0;
	for (int i = 0; i < n; i++) {
		if (i < k) {
			input.push_back(f0(i));
			opt += f0(i);
		} else {
			input.push_back(0);
		}
	}


	int nw = k;
	vector<vector<double>> window;
	int ntrials = 100;
	double avg = 0;
	for (int t = 0; t < ntrials; t++) {
		window.clear();
		window.resize(nw);
		random_shuffle(input.begin(), input.end());
		for (int i = 0; i < n; i++) {
			window[rand() % nw].push_back(input[i]);
		}

		double res = 0;
		for (int i = 0; i < k; i++) {
			double M = 0;
			for (double v : window[i]) {
				M = max(M, v);
			}
			res += M;
		}
		avg += double(res) / opt;
	}
	avg /= ntrials;
	cout << "The optimal value is " << opt << " and the average value is " << avg * opt << endl;
	cout << "Fraction of optimal obtained: " << avg << endl;
	cout << "Expected: " << 1 - exp(-1) << endl;
	
	return 0;
}