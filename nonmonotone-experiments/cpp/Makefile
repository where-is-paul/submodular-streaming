all: main binarize_data
debug: main-debug binarize_data

main: main.cpp *.h
	g++ -std=c++17 -march=native -Wall -pg -fopenmp -O3 -I./ -o main main.cpp

binarize_data: binarize_data.cpp
	g++ -std=c++17 -Wall -O3 -I./ -o binarize_data binarize_data.cpp

main-debug: main.cpp *.h
	g++ -std=c++17 -Wall -pg -fopenmp -O0 -I./ -o main main.cpp

clean:
	rm main
