#include <bits/stdc++.h>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

using namespace std;

int main(int argc, char* argv[]) {
	extern char *optarg;
	static char usage[] = "usage: %s [-k num_opt] [-n num_elem] [-t ntrials]\n";

	int c, flags = 0, N = 10000, K = 100, ntrials = 1;

	while ((c = getopt(argc, argv, "n:k:t:")) != -1) {
		switch (c) {
			case 'k':
				K = atoi(optarg);
				break;
			case 'n':
				N = atoi(optarg);
				break;
			case 't':
				ntrials = atoi(optarg);
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

	double nrep = 0;
	vector<vector<int>> L(K + 1);
	vector<int> cmax(K + 1);
	vector<int> ccnt(K + 1);

	for (int t = 0; t < ntrials; t++) {
		vector<int> reps(K + 1);
		for (int i = 0; i < K; i++) {
			L[i].clear();

			for (int j = 0; j < N; j++) {
				L[i].push_back(j + 1);
			}
			random_shuffle(L[i].begin(), L[i].end());
			ccnt[i] = 0;
			cmax[i] = 0;
		}

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < K; j++) {
				// cerr << j << endl;
				if (cmax[j] < L[j][ccnt[j]]) {
					// cerr << "Evicting " << j << " on element " << i 
						 // << " for value " << L[j][ccnt[j]] 
						 // << " which was previously " << cmax[j] << endl;
					nrep++;
					reps[j]++;
					cmax[j] = L[j][ccnt[j]];
					// cerr << "Set max of " << j << " to " << cmax[j] << endl;

					ccnt[j+1] = 0;
					cmax[j+1] = 0;
					L[j+1].clear();
					for (int k = 0; k < N-i; k++) {
						L[j+1].push_back(k + 1);
					}
					random_shuffle(L[j+1].begin(), L[j+1].end());
				}
				ccnt[j]++;
			}
		}

		for (int i = 0; i < K; i++) {
			cerr << i << " has " << reps[i] << " reps" << endl;
		}
	}
	nrep /= ntrials;

	cout << nrep << endl;
	return 0;	
}