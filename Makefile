all: mmap_concept mmap_speedtest mmap_threads

mmap_concept: mmap_concept.cc
	g++ -o mmap_concept mmap_concept.cc

mmap_speedtest: mmap_speedtest.cc
	g++ -o mmap_speedtest mmap_speedtest.cc

mmap_threads: mmap_threads.cc
	g++ -o mmap_threads mmap_threads.cc -lpthread

clean:
	rm -f *.o mmap_concept mmap_speedtest mmap_threads
