// Anthony Niemiec, Purdue University Spring 2019

#include <iostream>
#include <linux/memfd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <iomanip>
#include <chrono>
#define PAGESIZE 4096 // 4KB in x86 systems
#define MEMSIZE 1000

auto calcTime(auto start, auto finish) { // calculates time between two points to the nearest nanosecond
	float timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
	std::cout << std::setw(6) << timeTaken << "ns,\t" << std::setw(7) << timeTaken/MEMSIZE << "ns per operation\n";
	return timeTaken;
}

void calcDiff(auto t1, auto t2) { // compares two time measurements
	float delta = abs(t1-t2);
	printf("\033[33m"); // set a different color
	std::cout << "Difference:\t\t" << std::setw(6) << delta << "ns,\t" << std::setw(7) << delta/MEMSIZE << "ns per operation\n";
	printf("\033[0m"); // restore default terminal color
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

	// initializing a SHARED mmap based on the anonymous file descriptor
	char* shared_mem = static_cast<char*>(mmap(NULL, fSize, PROT_READ | PROT_WRITE, MAP_SHARED, inFile, 0));
	if (shared_mem == (char*)MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	// initializing a PRIVATE mmap based on the anonymous file descriptor
	char* private_mem = static_cast<char*>(mmap(NULL, fSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, inFile, 0));
	if (private_mem == (char*)MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

// Part 1: measuring write time of mmap vs malloc
	printf("Write time of shared/private mmap vs malloc\n");

	// measuring malloc write time
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MEMSIZE; i++) {
		memset(&mal[i], 'a', 1);
	}
	auto finish = std::chrono::high_resolution_clock::now();
	printf("Malloc write:\t\t");
	auto mal_writeTime = calcTime(start, finish);

	// measuring SHARED mmap write time
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MEMSIZE; i++) {
		memset(&shared_mem[i], 'b', 1);
	}
	finish = std::chrono::high_resolution_clock::now();
	// comparing to malloc
	printf("\033[31mSHARED\033[0m mmap write:\t");
	calcDiff(mal_writeTime, calcTime(start, finish));

	// measuring PRIVATE mmap write time
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MEMSIZE; i++) {
		memset(&private_mem[i], 'c', 1);
	}
	finish = std::chrono::high_resolution_clock::now();
	printf("\033[31mPRIVATE\033[0m mmap write:\t");
	calcDiff(mal_writeTime, calcTime(start, finish));

// Part 2: measuring read time of mmap vs malloc
	printf("\nRead time of shared/private mmap vs malloc\n");

	// measuring malloc read time
	char mallocBuff[MEMSIZE];
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MEMSIZE; i++) {
		memcpy(&mal[i], &mallocBuff[i], 1);
	}
	finish = std::chrono::high_resolution_clock::now();
	printf("Malloc read:\t\t");
	auto mal_readTime = calcTime(start, finish);

	// measuring SHARED mmap read time
	char sharedBuff[MEMSIZE];
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MEMSIZE; i++) {
		memcpy(&shared_mem[i], &sharedBuff[i], 1);
	}
	finish = std::chrono::high_resolution_clock::now();
	// comparing to malloc
	printf("\033[31mSHARED\033[0m mmap read:\t");
	calcDiff(mal_readTime, calcTime(start, finish));

	// measuring PRIVATE mmap read time
	char privateBuff[MEMSIZE];
	start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < MEMSIZE; i++) {
		memcpy(&private_mem[i], &privateBuff[i], 1);
	}
	finish = std::chrono::high_resolution_clock::now();
	// comparing to malloc
	printf("\033[31mPRIVATE\033[0m mmap read:\t");
	calcDiff(mal_readTime, calcTime(start, finish));
}
