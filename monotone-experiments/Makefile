all: setcover sim-worst cache-evict-sim

cache-evict-sim: cache-evict-sim.cpp
	g++ -std=c++17 -Wall -O3 -o cache-evict-sim cache-evict-sim.cpp
	
setcover: setcover.cpp
	g++ -std=c++17 -Wall -O3 -o setcover setcover.cpp

sim-worst: sim-worst.cpp
	g++ -std=c++17 -Wall -O3 -o sim-worst sim-worst.cpp


clean:
	rm setcover sim-worst
