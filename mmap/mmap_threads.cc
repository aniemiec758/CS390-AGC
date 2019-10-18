// Anthony Niemiec, Purdue University Spring 2019

/* IMPORTANT you must compile this with the -lpthread flag IMPORTANT */

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <sys/mman.h>
#include <stdlib.h>
#define MEMSIZE 1000

void snapshotThread(int inFile, int fSize);
void mutatorThread(char* shared_mem);

int main(int argc, char** argv) {
// Part 0: setting up environment
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

// Part 1: writing some initial data
	// initializing a SHARED mmap of the anonymous file descriptor
	char* shared_mem = static_cast<char*>(mmap(NULL, fSize, PROT_READ | PROT_WRITE, MAP_SHARED, inFile, 0));
	if (shared_mem == (char*)MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

	// writing values to the shared mmap (reflected in the anonymous file)
	for (int i = 0; i < MEMSIZE; i++) {
		shared_mem[i] = i;
	}

// Part 2: creating a snapshot and mutator thread
	std::thread t_snap(snapshotThread, inFile, fSize); // must pass in arguments to create private_mem
	std::thread t_mutate(mutatorThread, shared_mem); // must pass in shared_mem to mutate the file

	t_snap.join();
	t_mutate.join();
}

// Part 3: the snapshotting thread
void snapshotThread(int inFile, int fSize) {
	// initializing a PRIVATE mmap of the anonymous file descriptor
	char* private_mem = static_cast<char*>(mmap(NULL, fSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, inFile, 0));

	// continuously checking for changes ...
	int i = 0;
	while (i < MEMSIZE) {
		if (private_mem[i] != i) { // detected a change
			fprintf(stdout, "Error: change detected at index %d!\n", i);
			break;
		}
		i %= (MEMSIZE-1); // always keeps i below MEMSIZE

		// to show that we're still checking
		sleep(1);
		fprintf(stdout, "Working...\n");
	}
}

// Part 4: the mutating thread
void mutatorThread(char* shared_mem) {
	sleep(5); // let the snapshot thread run a few times to make sure that it works

	// make some random changes
	for (int i = 0; i < MEMSIZE; i++) {
		shared_mem[i] = i+7;
	}
}

/* IMPORTANT you must compile this with the -lpthread flag IMPORTANT */
