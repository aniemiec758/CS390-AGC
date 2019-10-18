// Anthony Niemiec, Purdue University Spring '19 | CS390-AGC Advanced Garbage Collection Techniques

#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/memfd.h>
#define PAGESIZE 4096 // 4KB in x86 systems

int main(int argc, char** argv) {
    int writeSize = 50; // number of characters to write in mmap's

    // specifications of file to open/create
	int fSize = 50*1048576; // 50MiB file
	char fName[] = {"./large_file"};

	// Part 1: opening a file with mmap() and writing to it
	//	good resource by Jacob Sorber: https://www.youtube.com/watch?v=m7E9piHcfr4
//	int inFile = open(fName, O_RDWR | O_CREAT, 0660); // fetching a file descriptor so we can make mmap's (WILL create a file on disk)
    int inFile = memfd_create(fName, 0); // create an anonymous file descriptor that won't remain in our system
	if (inFile < 0) {
		perror("open");
		exit(1);
	}

    // jump to start of file descriptor
	lseek(inFile, fSize, SEEK_SET);
	write(inFile, "", 1); // avoid random bugs upon file creation

    // Part 2: creating a PUBLIC mmap, where changes to the mmap will make changes to the file
	char* shared_mem = static_cast<char*>(mmap(NULL, fSize, PROT_READ | PROT_WRITE, MAP_SHARED, inFile, 0));
	if (shared_mem == (char*)MAP_FAILED) {
		perror("mmap");
		exit(1);
	}

    // writing some data to the file on disk; because of MAP_SHARED, writing to the mmap will also edit the file
	for (int i = 0; i < writeSize; i++) {
        shared_mem[i] = '1';
	}

    // Part 3: creating a PRIVATE memory mapping of the same file (for copy-on-write)
    char* private_mem = static_cast<char*>(mmap(NULL, fSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, inFile, 0));
    if (private_mem == (char*)MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // writing some data again; because of MAP_PRIVATE, a copy-on-write will occur (writing to mmap will not modify file on disk)
    for (int i = 0; i < writeSize/2; i++) {
        private_mem[i] = '2';
    }

    // Part 4: comparing the contents of the two mappings to the file
    lseek(inFile, 0, SEEK_SET); // move back to beginning of file
    char fBuff[writeSize];
    read(inFile, fBuff, writeSize);

    printf("File contents:\t%s\n", fBuff);
    printf("Public mmap:\t%s\n", shared_mem);
    printf("Private mmap:\t%s\n", private_mem);

	// don't need the file descriptor or mmap's anymore
	munmap(shared_mem, fSize);
	munmap(private_mem, fSize);
	close(inFile);
}
