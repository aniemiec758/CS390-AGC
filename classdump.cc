#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "classdump.hh"

#define err(s)					\
	printf("-> Error: %s\n", s);	\
	exit(1);
#define ld(n)					\
	inFile.read(n, sizeof(n));
#define print(u)				\
	print_uBuff(u, sizeof(u));
/* TODO TODO TODO
#define decimal(u)				\
	uBuffToDecimal(u, sizeof(u));

// TODO idea: what if this simply uses mmap() to place the char*
//	array into a uint_32 or whatever, no need for a for-loop
//	(doesn't work on both endian-nesses, however)

*/

// reads through a .class file and dumps sections as defined in the JVM 8 specification
//   see Chapter 4: https://docs.oracle.com/javase/specs/jvms/se8/jvms8.pdf
int main(int argc, char** argv) {
	// usage
	if (argc != 2) {
		printf("-> Usage: %s <filename>\n", argv[0]);
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

/*	
	// checking Endian-ness of the machine: Java bytecode is strictly Big-Endian itself
	if (isLittleEndian()) {
		#undef ld
		#define ld(n) \
			for (char i=sizeof(n)-1; i>=0; i--) { \
				inFile.read(n+i, 1); \
			}
	}
*/	

/*					commencing the classdump					*/
	printf("\nDumping classfile %s:\n", filename);
	printf("  Size: %zu B\n---\n", filesize);

	// useful variables to keep around
	short cpoolSize; // size of the constant pool

	// useful .class buffers; the JVM 8 spec utilizes 1-, 2-, 4-, and 8-byte tags
	// macros at the top of the file allow for syntax ld(u_x) -> read(u_x, x)
	char u1[1]; char u2[2];
	char u4[4]; char u8[8];

	// checking for magic number, 0xCAFEBABE (also checks Endian-ness
	ld(u4);
	if (checkMagic(u4) == 0) { // flag for Little-Endian machine
		#undef ld
		#define ld(n)							\
			for (char i=sizeof(n)-1; i>=0; i--) {	\
				inFile.read(n+i, 1);			\
			}
	}
/*
	if (!checkMagic(u4)) { // TODO construct buffers in an Endian-independent way at some point
		err("potentially corrupted .class file: magic number 0xCAFEBABE not detected!")
	}
*/
	printf("Magic number found: 0x"); print(u4); printf("\n");
	// misc header info
	ld(u2); printf("Minor version [currently in hex]: "); print(u2); printf("\n");
	ld(u2); printf("Major version [currently in hex]: "); print(u2); printf("\n");
	ld(u2); printf("Number of items in the constant pool [currently in hex]: "); print(u2); printf("\n");


/* TODO TODO TODO
	ld(u2); printf("Minor version: %d\n", decimal(u2));
	ld(u2); printf("Major version: %d\n", decimal(u2));
	ld(u2); printf("Number of items in constant pool: %d\n", decimal(u2));
*/
	// the constant pool
	cpoolSize = *(short*)(u2); // cast buffer to a 2-byte pointer, then dereference
	for (short i = 0; i < cpoolSize; i++) {
	
printf("hello #%d of %d\n", i, cpoolSize);
	
	}


// scratch work

printf("\nTO BE CONTINUED... [scratch work below]\n");

// TODO need a #define macro that converts a buffer into decimal

// TODO update uBuffToDecimal to return a template <T>, so
//	that it returns uchar for u1, short for u2, uint for u4,
//	and long for u8 (currently returns long, wastes space)


/*
	int32_t s;
	inFile.read(reinterpret_cast<char*>(&s), 4);
	printf("%x\n", s);
*/

// TODO endianness thought:
//	change ld() to point to a load_little() or load_big()
//	function based on if the machine is big or little endian:
//	this way, the code will be endian-independent due to
//	the u_x buffers separating the code from the hardware
// TODO are the buffers actually already endian-independent?

	ld(u4); // TODO endian-ness?
	print(u4);
	

// end of scratch work

/*					end of classdump						*/

	// cleanup
	inFile.close();

	return 0;
}
