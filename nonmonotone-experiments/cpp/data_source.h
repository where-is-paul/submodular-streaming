#ifndef _DATA_SOURCE_H
#define _DATA_SOURCE_H

#include <random>
#include <memory>

using namespace std;

class BaseSource {
public:
	virtual vector<double> data_at(size_t id) {
		return vector<double>();
	}
	// Return the length of the stream
	virtual size_t length() {
		return 0;
	}

	virtual ~BaseSource() {}
};

/* A stream that simply returns randomly generated vectors of dimension d. 
 * For testing purposes. 
 */
class RandomSource : public BaseSource {
	size_t m_length, m_dim;
	mt19937	m_mt;
	// Default distribution is [0, 1)
	uniform_real_distribution<> m_dist;
public:
	RandomSource(size_t length, size_t dim) : m_length(length), m_dim(dim) {
	}

	vector<double> data_at(size_t id) {
		// Set a fixed seed of the default c++ generator, then feed the output
		// to the mersenne twister to save memory.
		srand(id);
		m_mt = mt19937((long unsigned int) rand());
		vector<double> ret(m_dim);
		for (size_t i = 0; i < m_dim; i++) {
			ret[i] = m_dist(m_mt);
		}
		return ret;
	}

	// Return the length of the stream
	size_t length() {
		return m_length;
	}
};

/* A stream that returns the vectors in a file.
 */
class FileSource : public BaseSource {
	size_t m_length, m_dim;
	vector<vector<double>> m_vecs;
public:
	FileSource(string filename) {
		// Read the file into a list of vectors and store it in memory
		// Skip the first line because we use it to describe the problem instance
		ifstream config_file(filename);
		string header, type;
		getline(config_file, header);

		size_t N, K, W, DIM;
		stringstream ss(header);
		ss >> type >> N >> K >> W >> DIM;

		m_vecs.resize(N);
		for (size_t i = 0; i < N; i++) {
			double inp;
			m_vecs[i].resize(DIM);
			for (size_t j = 0; j < DIM; j++) {
				config_file >> inp;
				m_vecs[i][j] = inp;
			}
		}
		m_length = N;
		m_dim = DIM;
	}

	vector<double> data_at(size_t id) {
		return m_vecs[id];
	}

	// Return the length of the stream
	size_t length() {
		return m_length;
	}
};

/* A stream that returns the vectors in a file.
 */
class BinarySource : public BaseSource {
	const int DOUBLE_BYTES = 8;

	size_t m_length, m_dim, m_vector_bytes, m_lastpos;
	ifstream m_binary_file;
	unique_ptr<char> m_data_buffer;
public:
	BinarySource(string filename) {
		// Read the file into a list of vectors and store it in memory
		// Skip the first line because we use it to describe the problem instance
		ifstream config_file(filename);
		string header, type;
		getline(config_file, header);

		size_t N, K, W, DIM;
		stringstream ss(header);
		ss >> type >> N >> K >> W >> DIM;

		string binary_path;
		config_file >> binary_path;
		m_binary_file.open(binary_path, ios::in | ios::binary);

		m_length = N;
		m_dim = DIM;
		m_vector_bytes = DOUBLE_BYTES * m_dim;
		m_data_buffer.reset(new char[m_vector_bytes]);

		m_lastpos = size_t(-1);
	}

	vector<double> data_at(size_t id) {
		// Seek to right location
		// Optimize for seeking to current location
		// (Seeks seem to be expensive)
		if (m_lastpos + 1 != id)
			m_binary_file.seekg(id * m_vector_bytes);
		
		m_binary_file.read(m_data_buffer.get(), m_vector_bytes);

		double* vector_data = reinterpret_cast<double*>(m_data_buffer.get());

		m_lastpos = id;
		return vector<double>(vector_data, vector_data + m_dim);
	}

	// Return the length of the stream
	size_t length() {
		return m_length;
	}
};

#endif