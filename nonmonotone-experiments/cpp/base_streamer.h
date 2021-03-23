#ifndef _BASE_STREAMER_H
#define _BASE_STREAMER_H

#include "data_cache.h"
#include "dpp_objectives.h"

#include <cassert>
#include <set>
#include <map>
#include <vector>
#include <memory>

/* The base class for all our streaming algorithms.
 */
class BaseStreamer {
protected:
	// Window size and cardinality constraint
	// The "time" parameter is how many elements have been added so far.
	// Used for generating unique ids.
	size_t m_wsize, m_k, m_time;
	shared_ptr<VectorCache> m_cache;
public:
	BaseStreamer() {
		m_cache = make_shared<VectorCache>();
	}

	BaseStreamer(size_t wsize, size_t k, MetricEvaluator* metric = nullptr) 
	: m_wsize(wsize), m_k(k), m_time(0) {
		m_cache = make_shared<VectorCache>(metric);
	}

	// Add one element to the sliding window, removing the least recent if
	// the window capacity is exceeded.
	// Returns true if element was accepted into the cache,
	// false otherwise. Being accepted into the cache means the element was
	// deemed valuable for future use (does not necessarily mean it is part
	// of the returned solution).
	virtual bool add(const vector<double>& elt) = 0;

	// Query for the optimal MIC objective on this window
	virtual double query() = 0;

	// The ids of vectors in the solution
	virtual vector<size_t> solution() = 0;

	size_t cache_size() {
		return m_cache->size();
	}
};

#endif 