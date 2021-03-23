#ifndef _GREEDY_STREAMER_H
#define _GREEDY_STREAMER_H

#include "base_streamer.h"

class GreedyStreamer : public BaseStreamer {
	MAPObjective m_obj;
public:
	GreedyStreamer(size_t wsize, size_t k, MetricEvaluator* metric = nullptr) 
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
			double best_marginal = -1;
			size_t best_id = 0;
			for (size_t i = 1; i <= m_wsize; i++) {
				if (m_time < i) break;
				size_t id = m_time - i;
				double val = m_obj.marginal(id);
				if (val > best_marginal) {
					best_marginal = val;
					best_id = id;
				}
			}
			m_obj.push(best_id);
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