#ifndef _DATA_CACHE_H
#define _DATA_CACHE_H

#include <map>

#include <cmath>
#include <cassert>

// Backsolve a lower triangular system.
inline vector<double> backsolve(const vector<vector<double>>& L, 
						 const vector<double>& y) {
	assert(L.size() == y.size() && 
		"Backsolve dimension not matching.");
	// Solves an equation of the form Lx = y
	vector<double> x(y.size());
	for (size_t i = 0; i < y.size(); i++) {
		x[i] = y[i];
		for (size_t j = 0; j < i; j++) {
			x[i] -= L[i][j] * x[j];
		}
		x[i] /= L[i][i];
	}
	return x;
}

// Some helpers for computing dot products and back-solving
// lower-triangular linear systems.
inline double dot(const vector<double>& u, const vector<double>& v) {
	assert(u.size() == v.size() && 
		"Dot product not between vectors of same length.");
	double res = 0;
	for (size_t i = 0; i < u.size(); i++) {
		res += u[i] * v[i];
	}
	return res;
}

/* A class for evaluating the "similarity" between vectors in the cache. */
class MetricEvaluator {
public:
	virtual ~MetricEvaluator() = default;
	virtual double evaluate(const vector<double>& u, const vector<double>& v) = 0;
};


class SimilarityMetric : public MetricEvaluator {
public:
	double evaluate(const vector<double>& u, const vector<double>& v) {
		return dot(u, v);
	}
};

class GeodistanceMetric : public MetricEvaluator {
public:
	inline double geodistance(double lat1, double lon1, double lat2, double lon2) {
		double R = 6371e3; // metres
		double phi1 = lat1 * M_PI/180; // φ, λ in radians
		double phi2 = lat2 * M_PI/180;
		double dphi = (lat2-lat1) * M_PI/180;
		double dlambda = (lon2-lon1) * M_PI/180;

		double a = sin(dphi/2) * sin(dphi/2) 
				   + cos(phi1) * cos(phi2) * sin(dlambda/2) * sin(dlambda/2);
		double c = 2 * atan2(sqrt(a), sqrt(1-a));

		double d = R * c; // in metres
		return d;
	}

	double evaluate(const vector<double>& u, const vector<double>& v) {
		const double kernel_radius = 1500;
		double d = geodistance(u[0], u[1], v[0], v[1]);
		return exp(-d * d / (kernel_radius * kernel_radius));
	}
};

/* A cache for stored input vectors. */
class VectorCache {
	// Can also use a hash map here (map is a BST by default)
	map<size_t, vector<double>> cache;
	MetricEvaluator* metric;
public:
	VectorCache(MetricEvaluator* input_metric = nullptr) {
		if (input_metric) {
			metric = input_metric;
		}
	}

	vector<double> get(size_t id) {
		if (!cache.count(id)) cerr << id << endl;
		assert(cache.count(id) != 0 && "Requested index not found.");
		return cache[id];
	}

	bool contains(size_t id) {
		return cache.count(id) != 0;
	}

	void add(size_t id, const vector<double>& vec) {
		if (cache.count(id)) return;
		cache[id] = vec;
	}

	void remove(size_t id) {
		assert(cache.count(id) != 0 && "Element removed not actually in the cache.");
		cache.erase(id);
	}

	size_t size() {
		return cache.size();
	}

	double evaluate(size_t id0, size_t id1) {
		assert(cache.count(id0) && cache.count(id1) && "Items not found in cache on evaluate!");
		return metric->evaluate(cache[id0], cache[id1]);
	}
};

#endif