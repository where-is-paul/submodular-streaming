#ifndef _MULTILEVEL_STREAMER_H
#define _MULTILEVEL_STREAMER_H

#include "base_streamer.h"

#include <algorithm>
#include <random>
#include <functional>

class MultiLevelStreamer : public BaseStreamer {
	// Stores the historical solutions for every single window
	// This uses more memory than the algorithm in the paper.
	// We do it this way because we're comparing the approximation
	// ratio only, and this algorithm is much easier to implement and 
	// has the same approximation.
	vector<vector<MAPObjective>> m_obj;
	const double ALPHA = 42;

public:
	MultiLevelStreamer(size_t k, MetricEvaluator* metric = nullptr) 
	: BaseStreamer(k, metric) {
	}

	// Returns true if element was accepted into the cache,
	// false otherwise.
	bool add(const vector<double>& elt) {
		m_cache->add(m_time, elt);
		m_time++;
		return true;
	}

	double query() {
		assert(ALPHA * m_k <= m_time && "alpha is set too large! (More windows than elements)");

		default_random_engine generator;
		uniform_real_distribution<double> distribution(0, 1);
		static auto flip = bind(distribution, generator);

		m_obj.resize(ALPHA * m_k + 1);
		for (int i = 0; i <= ALPHA * m_k; i++) {
			m_obj[i].resize(m_k+1);
			for (size_t j = 0; j <= m_k; j++) {
				m_obj[i][j].set_cache(m_cache);
			}
		}

		set<int> H;
		for (int i = 0; i <= ALPHA * m_k; i++) {
			int delta = m_time / (ALPHA * m_k);
			int idL = i * delta, idR = min((i+1) * delta, (int) m_time);
			vector<int> C;
			for (int id = idL; id < idR; id++) {
				// Do the subwindow algorithm
				vector<double> q(i+1);
				// Reconstruct L_l^{j-1}
				for (int j = 0; j <= i; j++) {
					// If e_{id} is in window j, would it have been taken?
					int A = 0;
					for (int k = 0; k < j; k++) {
						// Would it have been subsampled away?
						if (flip() > q[k]) {
							A++;
							continue;
						}
						// Would it have been taken on any level?
						bool taken = false;
						for (size_t l = 0; l < m_k; l++) {
							double val = m_obj[k][l].marginal(id);
							if (val > m_obj[k][l+1].value() - m_obj[k][l].value()) {
								taken = true;
							}
						}

						if (!taken) {
							A++;
						}
					}
					q[j] = (ALPHA * m_k - j + A) / double(ALPHA * m_k);
				}
				if (flip() <= q[i]) {
					C.push_back(i);
				}
			}

			// Get the best gain on each level. This is a little bit different from
			// the algorithm, which compares a single average gain over \sqrt(k)
			// levels, but the analysis is the same.
			for (int id : H) {
				if (flip() <= 1. / (ALPHA * m_k)) {
					C.push_back(id);
				}
			}

			for (size_t l = 1; l <= m_k; l++) {
				if (i > 0) {
					// Copy over from previous levels
					m_obj[i][l] = m_obj[i-1][l];
				}

				// Check for the best on each level
				int best_id = -1;
				double best_val = 0;
				for (int id : C) {
					double delta = m_obj[i][l-1].marginal(id);
					if (delta >= m_obj[i][l].value() - m_obj[i][l-1].value()) {
						if (delta > best_val) {
							best_val = m_obj[i][l-1].marginal(id);
							best_id = id;
						}
					}
				}
				auto copy = m_obj[i][l-1];
				if (best_id != -1) {
					copy.push(best_id);
					m_obj[i][l] = copy;
					H.insert(best_id);
				}
			}
		}

		double retval = 0;
		for (size_t l = 0; l <= m_k; l++) {
			retval = max(retval, m_obj.back()[l].value());
		}
		return retval;
	}

	vector<size_t> solution() {
		// Call query first to populate the solution
		query();
		return m_obj.back()[m_k].ids();
	}
};
#endif