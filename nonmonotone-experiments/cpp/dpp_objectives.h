#ifndef _DPP_OBJECTIVES_H
#define _DPP_OBJECTIVES_H

/* A class for efficiently evaluating the log-MAP objective.
 * Supports inserting vectors one at a time.
 * Requires memory access to the 'cache' of vectors stored by the streaming alg.
 * (We want to avoid storing the full vectors within the objective function to
 * ensure low memory usage.)
 * Note: Very similar to MIC objective. The two classes can be unified but 
 * opting to keep it separate for now in case MAP-specific optimizations
 * are needed.
 */
class MAPObjective {
	vector<vector<double>> m_L; // Current triangular factors of I+K
								// (I = identity, K = DPP kernel)
	vector<size_t> m_stored_ids; // Current stored vectors (by id)
	shared_ptr<VectorCache> m_cache; // Cache for transforming id->vector
	size_t m_n; // Current dimension of kernel
	double m_value; // Current solution value

	// Epsilon value for floating point comparisons
	static constexpr double EPS = 1e-6;

public:
	MAPObjective(shared_ptr<VectorCache> cache) : m_cache(cache), m_n(0), m_value(0) {
	}
	MAPObjective() : m_n(0), m_value(0) {}

	const size_t length() const {
		return m_n;
	}

	vector<size_t> ids() const {
		return m_stored_ids;
	}

	const double value() const {
		return m_value;
	}

	void set_cache(shared_ptr<VectorCache> cache) {
		m_cache = cache;
	}

	// Clears the solution but does not clear the cache.
	void clear() {
		m_L.clear();
		m_stored_ids.clear();
		m_n = 0;
		m_value = 0;
	}

	// Adds the vector to the MAP objective, updates L and Linv accordingly
	double push(size_t id) {
		// Form a column of the DPP kernel.
		vector<double> Xv(m_n);
		for (size_t i = 0; i < m_n; i++) {
			Xv[i] = m_cache->evaluate(m_stored_ids[i], id);
		}
		double v2 = m_cache->evaluate(id, id);

		// Compute the last diagonal entry of the inverse
		vector<double> w = backsolve(m_L, Xv);
		double w2 = dot(w, w);

		assert(v2 >= w2 - EPS && "Input kernel is not positive definite!");
		double D = sqrt(v2 - w2 + EPS);

		// Update L
		w.push_back(D);
		for (size_t i = 0; i < m_n; i++) {
			m_L[i].push_back(w[i]);
		}
		m_L.push_back(w);
		m_n++;

		double res = 2 * log(D);

		m_value += res;
		m_stored_ids.push_back(id);
		return res;
	}

	void pop() {
		assert(m_n > 0 && "There's nothing to remove from the MIC objective!");
		for (size_t i = 0; i < m_n; i++) {
			m_L[i].pop_back();
		}
		m_L.pop_back();
		m_n--;
		m_stored_ids.pop_back();
	}

	// Computes the marginal value of adding a new vector on top of current
	// solution.
	double marginal(size_t id) {
		double res = push(id);
		pop();
		m_value -= res;
		return res;
	}

	// Adds to solution if exceeds threshold
	bool threshold_add(size_t id, double threshold) {
		double res = push(id);
		if (res < threshold) { 
			pop();
			m_value -= res;
			return false;
		}
		return true;
	}
};
#endif