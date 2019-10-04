#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#define err(s)					\
	printf("-> Error: %s\n", s);	\
	exit(1);

// reads through a .class file and dumps sections as defined in the JVM 8 specification
//   see Chapter 4: https://docs.oracle.com/javase/specs/jvms/se8/jvms8.pdf
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
	std::ifstream inFile;// = (std::ifstream*)malloc(sizeof(std::ifstream));
	inFile.open(filename, std::ios::in);
	if (inFile.fail()) {
		err("could not open/locate file");
	}
	// getting size
	inFile.seekg(0, std::ios::end);
	size_t filesize = inFile.tellg();
	inFile.seekg(0, std::ios::beg);

/*					commencing the classdump					*/
	printf("Dumping classfile %s:\n", filename);
	printf("  Size: %zu\n---\n", filesize);

	// useful .class buffers
	char u1[1]; char u2[2];
	char u4[4]; char u8[8];


/*					end of classdump						*/

	// cleanup
	inFile.close();

	return 0;
}
