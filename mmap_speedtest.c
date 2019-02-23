#include <iostream>
#include <chrono>
#define PAGESIZE 4096 // 4KB in x86 systems

int main(int argc, char** argv) {
    // Part 0: setting up environment
	int n = 1000;

	// initializing malloc'd memory
	char* mal = (char*)malloc(10 * sizeof(char));

	// initializing an anonymous file descriptor
	int fSize = 50*PAGESIZE;
	char fName[] = {"./anon_file"};
	int inFile = memfd_create(fName, 0); // won't remain on disk
	if inFile < 0) {
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
	auto start = std::chrono::high_resolution_clock::now();
	
	auto finish = std::chrono::high_resolution_clock::now();
	auto timeTaken = std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count()
	std::cout << "mmap: " << timeTaken << "ns, " << timeTaken/n << "ns per operation\n";
