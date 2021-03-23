#ifndef _RANDOM_GREEDY_STREAMER_H
#define _RANDOM_GREEDY_STREAMER_H

#include "base_streamer.h"

#include <algorithm>

class RandomGreedyStreamer : public BaseStreamer {
	MAPObjective m_obj;
public:
	RandomGreedyStreamer(size_t wsize, size_t k, MetricEvaluator* metric = nullptr) 
	: BaseStreamer(wsize, k, metric), m_obj(m_cache) {
	}

	// Returns true if element was accepted into the cache,
	// false otherwise.
	bool add(const vector<double>& elt) {
		m_cache->add(m_time, elt);
		m_time++;
		if (m_time > m_wsize) {
			m_cache->remove(m_time - m_wsize - 1);
		}
		return true;
	}

	double query() {
		m_obj.clear();
		while (m_obj.length() < m_k) {
			vector<pair<double, int>> sorted_marginals;
			for (size_t i = 1; i <= m_wsize; i++) {
				if (m_time < i) break;
				size_t id = m_time - i;
				double val = m_obj.marginal(id);
				sorted_marginals.push_back({val, id});
			}
			sort(sorted_marginals.rbegin(), sorted_marginals.rend());

			int choice = rand() % min(m_k, sorted_marginals.size());
			m_obj.push(sorted_marginals[choice].second);
		}
		return m_obj.value();
	}

	vector<size_t> solution() {
		// Call query first to populate the solution
		query();
		return m_obj.ids();
	}

	size_t size() {
		return min(m_cache->size(), m_k);
	}
};
#endif