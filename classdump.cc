#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

#include "classdump.hh"

#define err(s)								\
	printf("-> Error: %s\n", s);				\
	exit(1);
// TODO in some places, it would be really conventient if ld(n) was able to return the value of the read-in bytes alongside actually doing it
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
	#define ld(n)							\
		inFile.read(n, sizeof(n));			\
		byteCount += sizeof(n);
#else
	#define ld(n)							\
		for (char i=sizeof(n)-1; i>=0; i--) {	\
			inFile.read(n+i, 1);			\
		}								\
		byteCount += sizeof(n);
#endif
#define print(u)							\
	print_uBuff(u, sizeof(u));

// reads through a .class file and dumps sections as defined in the JVM 8 specification
//   see Chapter 4: https://docs.oracle.com/javase/specs/jvms/se8/jvms8.pdf
int main(int argc, char** argv) {
	long byteCount = 0; // very useful debug variable to determine where we are at in the .class file

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
	if (filesize < 4) { // TODO actual smallest .class size?
		err("file too small to be a .class file (insufficient space for magic number)");
	}
	// storing endianness
	if (!calculateEndianness()) {
		err("could not calculate Endianness of machine!");
	}

/*					commencing the classdump					*/
	printf("\nDumping classfile %s:\n", filename);
	printf("  Size: %zu B\n", filesize);
	calculateEndianness();
	printf("---\n");

	// useful .class buffers; the JVM 8 spec utilizes 1-, 2-, 4-, and 8-byte tags
	// macros at the top of the file allow for syntax ld(u_x) -> read(u_x, x)
	char u1[1]; char u2[2];
	char u4[4]; char u8[8];

	// checking for magic number, 0xCAFEBABE
	// NOTE: in the macros above, ld() will ensure that the byte buffers
	//	are all Big-Endian, like the JVM spec
	ld(u4);
	if (!checkMagic(u4)) { // neither Big nor Little Endian gives us the magic number
		printf("Received magic number: 0x %hhx %hhx %hhx %hhx\n", u4[0], u4[1], u4[2], u4[3]);
		err("potentially corrupted .class file: magic number 0xCAFEBABE not detected!\n");
	}

	printf("Magic number found: 0x"); print(u4); printf("\n");
	// misc header info
	ld(u2); printf("Minor version: %d\n", *(short*)u2);
	ld(u2); printf("Major version: %d\n", *(short*)u2);

	// the constant pool
	ld(u2); printf("\nNumber of items in the constant pool: %d\n", *(short*)u2);
	printf("  Entry 0:\n  -Reserved for `this` keyword\n"); // TODO aren't there cases where this isn't true i.e. static classes?
	short cpoolSize = *(short*)(u2); // cast buffer to a 2-byte pointer, then dereference
	for (short i = 1; i < cpoolSize; i++) { // size of constant pool ranges [1,n], because 0 is a reserved index
		printf("  Entry %d:\n", i);

		ld(u1); // get the tag
		switch (*u1) {
			case 1:		// CONSTANT_Utf8
{				printf("  -CONSTANT_Utf8 ");
				ld(u2); // length of constant
				short len = *(short*)u2;
				char* utf8_constant = (char*)malloc(len);
				// we must rely back on endianness, because this isn't going into the register (where the ld macro deals with endianness for us)
				if (endianness == BIG) {
					printf("(in Big-Endian format):\n\t");
					for (short i = 0; i < len; i++) {
						inFile.read(utf8_constant+i, 1);
					}
				} else {
					printf("(in Little-Endian format):\n\t");
					for (short i = len - 1; i >= 0; i--) {
						inFile.read(utf8_constant+i, 1);
					}
				}
				for (short i = 0; i < len; i++) {
					printf("%hhx", utf8_constant[i]);
					byteCount++; // debug variable to show where we are in the hexdump
				}
				printf("\n");
				free(utf8_constant);
}				break;
			case 3:		// CONSTANT_Integer
				printf("  -CONSTANT_Integer:\n");
				ld(u4); printf("\t%d\n", *(int*)u4);
				break;
			case 4:		// CONSTANT_Float
				printf("  -CONSTANT_Float:\n");
				ld(u4); printf("\t%f\n", *(float*)u4);
				break;
			case 5:		// CONSTANT_Long
// TODO are the ld(u8) lines done correctly, or should it actually be two separate ld(u4) lines [u4 high_bytes comes before u4 low_bytes in the JVM 8 spec]
				printf("  -CONSTANT_Long:\n");
				ld(u8); printf("\t%ld\n", *(long*)u8);
				break;
			case 6:		// CONSTANT_Double
// TODO same as above concerning ld(u8) vs using two ld(u4) lines
				printf("  -CONSTANT_Double:\n");
				ld(u8); printf("\t%lf\n", *(double*)u8);
				break;
			case 7:		// CONSTANT_Class
				printf("  -CONSTANT_Class:\n");
				ld(u2); printf("\tname_index located at constant pool index %d\n", *(short*)u2);
				// TODO make the check for if this is actually CONSTANT_utf8, as the JVM 8 spec asserts
				break;
			case 8:		// CONSTANT_String
				printf("  -CONSTANT_String:\n");
				ld(u2); printf("\tstring_index located at constant pool index %d\n", *(short*)u2);
				// TODO make the check for if this is actually CONSTANT_utf8, as the JVM 8 spec asserts
				break;
			case 9:		// CONSTANT_Fieldref
				printf("  -CONSTANT_Fieldref:\n");
				ld(u2); printf("\tclass_index located at constant pool index %d\n", *(short*)u2);
				ld(u2); printf("\tname_and_type_index located at constant pool index %d\n", *(short*)u2);
				// TODO like above, make JVM 8 spec checks
				break;
			case 10:		// CONSTANT_Methodref
				printf("  -CONSTANT_Methodref:\n");
				ld(u2); printf("\tclass_index located at constant pool index %d\n", *(short*)u2);
				ld(u2); printf("\tname_and_type_index located at constant pool index %d\n", *(short*)u2);
				// TODO like above, make JVM 8 spec checks
				break;
			case 11:		// CONSTANT_InterfaceMethodref
				printf("  -CONSTANT_InterfaceMethodref:\n");
				ld(u2); printf("\tclass_index located at constant pool index %d\n", *(short*)u2);
				ld(u2); printf("\tname_and_type_index located at constant pool index %d\n", *(short*)u2);
				// TODO like above, make JVM 8 spec checks
				break;
			case 12:		// CONSTANT_NameAndtype
				printf("  -CONSTANT_NameAndType:\n");
				ld(u2); printf("\tname_index located at constant pool index %d\n", *(short*)u2);
				ld(u2); printf("\tdescriptor_index located at constant pool index %d\n", *(short*)u2);
				// TODO like above, make JVM 8 spec checks
				break;
			case 15:		// CONSTANT_MethodHandle
				printf("  -CONSTANT_MethodHandle:\n");
				ld(u1); printf("\tReference kind: %d\n", *(char*)u1);
				ld(u2); printf("\treference_index located at constant pool index %d\n", *(short*)u2);
				// TODO like above, make JVM 8 spec checks
				break;
			case 16:		// CONSTANT_MethodType
				printf("  -CONSTANT_MethodType:\n");
				ld(u2); printf("\tdescriptor_index located at constant pool index %d\n", *(short*)u2);
				// TODO like above, make JVM 8 spec checks
				break;
			case 18:		// CONSTANT_InvokeDynamic
				printf("  -CONSTANT_InvokeDynamic:\n");
				ld(u2); printf("\tbootstrap_method_attr_index located at constant pool index %d\n", *(short*)u2);
				ld(u2); printf("\tname_and_type_index located at constant pool index %d\n", *(short*)u2);
				break;
				// TODO like above, make JVM 8 spec checks
			default:
				printf("  -Unexpected value for constant! %d\n", *u1);
		}
	}

	// misc middle info
	ld(u2); printf("\nAccess flag bits: %d\n", *(short*)u2); // TODO flesh out, eventually
	ld(u2); printf("this_class located at constant pool index %d\n", *(short*)u2);
	ld(u2); printf("super_class located at constant pool index %d\n", *(short*)u2);

	// the interfaces pool
	ld(u2); printf("\nNumber of interfaces: %d", *(short*)u2);
	short ipoolSize = *(short*)u2;
	for (short i = 0; i < ipoolSize; i++) {
		printf("\n  Entry [%d]:\n", i);
		ld(u2); printf("  -%d\n", *(short*)u2);
	}

	// the fields pool
	ld(u2); printf("\n\nNumber of items in the fields pool: %d", *(short*)u2);
	short fpoolSize = *(short*)u2;
	for (short i = 0; i < fpoolSize; i++) {
		printf("\n  Entry %d:\n", i);

		ld(u2); // get the access flags
		parseAccessFlags(*(short*)u2);

		ld(u2); printf("\tname_index located at constant pool index %d\n", *(short*)u2);
		ld(u2); printf("\tdescriptor_index located at constant pool index %d\n", *(short*)u2);

		ld(u2); printf("\tNumber of attributes for this field: %d\n", *(short*)u2);
		short apoolSize = *(short*)u2;
		for (short j = 0; j < apoolSize; j++) {
			ld(u2); printf("\t\tattribute_name_index located at %d\n", *(short*)u2);
			ld(u4); printf("\t\tLength of the attribute: %d\n", *(int*)u4);
			printf("\t\tThe attribute itself, in Big-Endian:\n\t\t\t");
			for(int k = 0; k < *(int*)u4; k++) {
				ld(u1); print(u1);
			}
		}
	}


	// the methods pool
	ld(u2); printf("\n\nNumber of items in the methods pool: %d", *(short*)u2);
	short mpoolSize = *(short*)u2;
	for (short i = 0; i < mpoolSize; i++) {
		printf("\n  Entry %d:\n", i);

		ld(u2); // get the access flags
		// TODO parseMethodsActionFlags() call here

		ld(u2); printf("\tname_index located at constant pool index %d\n", *(short*)u2);
		ld(u2); printf("\tdescriptor_index located at constant pool index %d\n", *(short*)u2);

		ld(u2); printf("\tNumber of attributes for this method: %d\n", *(short*)u2);
		short apoolSize = *(short*)u2;
		for (short j = 0; j < apoolSize; j++) {
			ld(u2); printf("\t\tattribute_name_index located at %d\n", *(short*)u2);
			ld(u4); printf("\t\tLength of the attribute: %d\n", *(int*)u4);
			printf("\t\tThe attribute itself, in Big-Endian:\n\t\t\t");
			for(int k = 0; k < *(int*)u4; k++) {
				ld(u1); print(u1);
			}
		}
	}

	// the generalized attributes pool
	ld(u2); printf("\n\nNumber of items in the attributes pool: %d", *(short*)u2);
	short apoolSize = *(short*)u2;
	for (short i = 0; i < apoolSize; i++) {
		printf("\n  Entry %d:\n", i);

		short poolSize = *(short*)u2;
		for (short j = 0; j < poolSize; j++) {
			ld(u2); printf("\tattribute_name_index located at %d\n", *(short*)u2);
			ld(u4); printf("\tLength of the attribute: %d\n", *(int*)u4);
			printf("\tThe attribute itself, in Big-Endian:\n\t\t");
			for(int k = 0; k < *(int*)u4; k++) {
				ld(u1); print(u1);
			}
		}
	}

	// checking for no extra bytes at the end
	if (!byteCount == filesize) {
		err("extra bytes found at end of .class file!");
	}
	printf("\n---\nClassdump complete!\n");

// scratch work

//printf("\nTO BE CONTINUED... [scratch work below]\n");
//ld(u8); print(u8);

// end of scratch work

/*					end of classdump						*/

	// cleanup
	inFile.close();

	return 0;
}
