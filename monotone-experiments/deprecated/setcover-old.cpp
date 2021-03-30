#include <bits/stdc++.h>

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

using namespace std;

int gain(const set<int>& S, const set<int>& T) {
	int res = 0;
	for (int x : T) {
		if (!S.count(x)) {
			res++;
		}
	}
	return res;
}

int fixed_threshold(vector<set<int>> sets, int k, double tau) {
	set<int> sol;
	for (size_t i = 0; i < sets.size(); i++) {
		if (gain(sol, sets[i]) > tau) {
			for (int x : sets[i]) sol.insert(x);
			k--;
		}
		if (k == 0) break;
	}
	return sol.size();
}

int moving_threshold(vector<set<int>> sets, int k, double maxtau) {
	set<int> sol;
	for (size_t i = 0; i < sets.size(); i++) {
		double beta = 1 - double(i) / sets.size();
		if (gain(sol, sets[i]) > beta * maxtau) {
			for (int x : sets[i]) sol.insert(x);
			k--;
		}
		if (k == 0) break;
	}
	return sol.size();
}

int lazy_greedy(vector<set<int>> sets, int k) {
	set<int> sol;
	priority_queue<pair<int, int>> q;
	for (size_t i = 0; i < sets.size(); i++) {
		q.push(make_pair(sets[i].size(), i));
	}

	while (k > 0) {
		auto cost = q.top().first, i = q.top().second;
		if (cost == 0) break;
		q.pop();
		if (gain(sol, sets[i]) == cost) {
			for (int x : sets[i]) sol.insert(x);
			k--;
		} else {
			cost = gain(sol, sets[i]);
			q.push(make_pair(cost, i));
		}
	}

	return sol.size();
}

int window_greedy_smooth(vector<set<int>> sets, int k, int alpha = 10) {
	set<int> sol[k+1];
	set<int> hist;
	int del = (sets.size() + alpha*k - 1) / (alpha*k);
	for (int i = 0; i < alpha * k; i++) {
		int best = 0, idx = -1;
		for (int l = 1; l <= k; l++) {
			best += sol[l].size();
		}
		for (int j = 0; j < del; j++) {
			if (i*del + j >= (int) sets.size()) {
				break;
			}
			int id = i*del + j;
			int cost = 0;
			for (int l = 1; l <= k; l++) {
				// can optimize this further by stopping the computation whenever the marginal gets too low
				// relative to what remains
				int tmp = gain(sol[l-1], sets[id]) + sol[l-1].size();
				cost += tmp;
			}
			if (cost > best) {
				best = cost;
				idx = id;
			}
		}
		for (int id : hist) {
			if (rand() % (alpha * k) != 0) continue;
			int cost = 0;
			for (int l = 1; l <= k; l++) {
				int tmp = gain(sol[l-1], sets[id]) + sol[l-1].size();
				cost += tmp;
			}
			if (cost > best) {
				best = cost;
				idx = id;
			}
		}
		if (idx != -1) {
			for (int l = k; l >= 1; l--) {
				sol[l] = sol[l-1];
				for (int x : sets[idx]) {
					sol[l].insert(x);
				}
			}
			hist.insert(idx);
			for (int l = 1; l < k; l++) {
				if (sol[l].size() > sol[l+1].size()) {
					sol[l+1] = sol[l];
				}
			}
		}
	}
	return sol[k].size();
}

int window_greedy(vector<set<int>> sets, int k, int alpha = 10) {
	set<int> sol[k+1];
	set<int> hist;
	int del = (sets.size() + alpha*k - 1) / (alpha*k);
	for (int i = 0; i < alpha * k; i++) {
		for (int l = 1; l <= k; l++) {
			int best = sol[l].size() - sol[l-1].size(), idx = -1;
			for (int j = 0; j < del; j++) {
				if (i*del + j >= (int) sets.size()) {
					break;
				}
				int id = i*del + j;
				int cost = gain(sol[l-1], sets[id]);
				if (cost > best) {
					best = cost;
					idx = id;
				}
			}
			for (int id : hist) {
				if (rand() % (alpha * k) != 0) continue;
				int cost = gain(sol[l-1], sets[id]);
				if (cost > best) {
					best = cost;
					idx = id;
				}
			}
			if (idx != -1) {
				sol[l] = sol[l-1];
				for (int x : sets[idx]) {
					sol[l].insert(x);
				}
				hist.insert(idx);
			}
		}
	}

	return sol[k].size();
}

int main(int argc, char* argv[]) {
	extern char *optarg;
	static char usage[] = "usage: %s [-f file] [-g file] [-r] [-k numsets] [-o OPT]\none of -f (steiner file mode), -g (freq item file mode) or -r (random mode) is required\n";

	int c, flags = 0, K = 100, OPT = 10000;
	bool opt_set = false;
	string filename;
	while ((c = getopt(argc, argv, "f:rk:o:g:")) != -1) {
		switch (c) {
			case 'f':
				flags = 1;
				filename = string(optarg);
				break;
			case 'r':
				flags = 2;
				break;
			case 'g':
				flags = 3;
				filename = string(optarg);
				break;
			case 'k':
				K = atoi(optarg);
				break;
			case 'o':
				OPT = atoi(optarg);
				opt_set = true;
				break;
			case '?':
				break;
		}
	}

	if (flags < 1) {	/* missing mandatory options */
		fprintf(stderr, "%s: missing options\n", argv[0]);
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	vector<set<int>> sets;
	if (flags == 1) {
		int n, m;
		ifstream fin(filename, ifstream::in);
		fin >> n >> m;

		sets.resize(n);
		for (int i = 0; i < m; i++) {
			int u, v, w; fin >> u >> v >> w;
			sets[u-1].insert(i);
			sets[v-1].insert(i);
			sets[w-1].insert(i);
		}
		cerr << "Finished reading datafile: " << n << " sets with " << m << " elements max." << endl;
	} else if (flags == 2) {
		const int ns = 10000;
		const int t = 100;

		cout << "Generating " << ns << " sets with opt equal to " << OPT << " and sets of size " << t << endl;
		for (int i = 0; i < ns; i++) {
			set<int> s;
			for (int j = 0; j < t; j++) {
				s.insert(rand() % OPT);
			}
			sets.push_back(s);
		}

		for (int i = 0; i < OPT; i += OPT/K) {
			set<int> s;
			for (int j = i; j < i + OPT/K; j++) {
				s.insert(j);
			}
			sets.push_back(s);
		}
	} else if (flags == 3) {
		ifstream fin(filename, ifstream::in);
		string line;
		int m = 0;
		while (getline(fin, line)) {
			stringstream ss(line);
			set<int> s;
			int x;
			while (ss >> x) {
				s.insert(x);
				m = max(m, x);
			}
			if (s.size()) {
				sets.push_back(s);
			}
		}
		int n = sets.size();
		cerr << "Finished reading datafile: " << n << " sets with " << m << " elements max." << endl;
	}

	srand(time(NULL));
	int nruns = 1;
	vector<int> lazy(K+1), fixed(K+1), moving(K+1), window(K+1);
	for (int i = 0; i < nruns; i++) {
		//cerr << "On iteration " << i << endl;
		random_shuffle(sets.begin(), sets.end());
		for (int k = 1; k <= K; k++) {
			lazy[k] += lazy_greedy(sets, k);
			
			// Hack to estimate the optimum
			if (!opt_set) {
				OPT = lazy_greedy(sets, k);
			}

			fixed[k] += fixed_threshold(sets, k, 0.5 * OPT / k);
			//moving += moving_threshold(sets, k, OPT / k);
			window[k] += window_greedy_smooth(sets, k);
		}
	}

	for (int k = 1; k <= K; k++) {
		lazy[k] /= nruns;
		fixed[k] /= nruns;
		moving[k] /= nruns;
		window[k] /= nruns;

		cout << "k = " << k << endl;
		cout << "Lazy greedy: " << lazy[k] << endl;
		cout << "Fixed threshold: " << fixed[k] << endl;
		//cout << "Moving threshold: " << moving << endl;
		cout << "Window greedy: " << window[k] << endl;
	}
	return 0;
}