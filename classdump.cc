#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

#define err(s)					\
	printf("Error: %s\n", s);	\
	exit(1);

size_t getFilesize(const char* filename) {
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}

// reads through a .class file and dumps sections as defined in the JVM 8 specification
//   see Chapter 4: https://docs.oracle.com/javase/specs/jvms/se8/jvms8.pdf
int classdump(int fd) {
	




	return 0; // success
}

int main(int argc, char** argv) {
	// usage
	if (argc != 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return -1;
	}
	const char* filename = argv[1];

	// checking for .class suffix
	if (strlen(filename) < 6) {
		err("expected a file ending in .class");
	}
	if (strcmp( strdup(filename + strlen(filename) - 6), ".class" )) {
		err("expected a file ending in .class");
	}

	// opening file descriptor
	int inFile = open(filename, O_RDONLY);
	if (inFile < 0) {
		printf("Could not find file, %s\n", filename);
		perror("memfd_create");
		return -1;
	}
	int filesize = getFilesize(argv[1]);

	// creating mmap of file
	char* map = static_cast<char*>(mmap(0, filesize, PROT_READ, MAP_SHARED, inFile, 0));
	if (map == MAP_FAILED) {
		printf("Error in creating mmap\n");
		perror("mmap");
		exit(-1);
	}

	// reading mmap
	char* magic[4];
	memcpy(magic, map, 4);
	if (map[0] == 0xCAFEBABE) {
		printf("success!\n");
	}

	for (int i = 0; i < filesize; i++) {
		printf("%c", map[i]);
	}

	return 0;
}
