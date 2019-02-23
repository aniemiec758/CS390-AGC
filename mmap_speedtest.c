#include <iostream>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <chrono>
#define PAGESIZE 4096 // 4KB in x86 systems
#define MEMSIZE 1000

void calcTime(auto start, auto finish) { // calculates time between two points to the nearest nanosecond
	auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
	std::cout << timeTaken << "ns, " << timeTaken/MEMSIZE << "ns per operation\n";
}

int main(int argc, char** argv) {
    // Part 0: setting up environment
	// initializing malloc'd memory
	char* mal = (char*)malloc(MEMSIZE * sizeof(char));

	// initializing an anonymous file descriptor
	int fSize = MEMSIZE;
	char fName[] = {"./anon_file"};
	int inFile = memfd_create(fName, 0); // won't remain on disk
	if (inFile < 0) {
		perror("memfd_create");
		exit(1);
	}

	// navigating through file to create a public mmap
	lseek(inFile, fSize, SEEK_SET);
	write(inFile, "", 1); // avoid random bugs upon file creation

	// initializing a mmap based on the anonymous file descriptor
	char* shared_mem = static_cast<char*>(mmap(NULL, fSize, PROT_READ | PROT_WRITE, MAP_SHARED, inFile, 0));
	if (shared_mem == (char*)MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

    // Part 1: measuring write time of mmap vs malloc
	// measuring malloc write time
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MEMSIZE; i++) {
		memset(&mal[i], 'a', 1);
	}
	auto finish = std::chrono::high_resolution_clock::now();
	printf("Malloc: ");
	calcTime(start, finish);

	// measuring mmap write time
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MEMSIZE; i++) {
		memset(&shared_mem[i], 'a', 1);
	}
	finish = std::chrono::high_resolution_clock::now();
	printf("Mmap: ");
	calcTime(start, finish);

    // Part 2: measuring read time of mmap vs malloc
	// measuring malloc read time
	






}
