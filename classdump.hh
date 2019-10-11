#define err(s)                     \
	printf("-> Error: %s\n", s);  \
	exit(1);

enum Endianness { LITTLE, BIG };
Endianness endianness;

// %x will print a 4-byte hex value (sizeof an int),
//   %hhx prints a 1-byte hex value
void print_uBuff(char* u, int len) {
	for (int i = 0; i < len; i++) {
		printf("%hhx", u[i]);
	}
}

// only must be done once, specifically with a four-byte buffer;
//   because the buffer is comprised of char's so that the
//   ifstream may be read into it, we must convert hex -> char
//
// this is also a good time to check for Endian-ness (or corrupt
//	.class files): because JVM bytecode is strictly Big-Endian,
//	we need to account for this if we are on a Little-Endian
//	machine
bool checkMagic(char* u4) {
	// check for Big-Endian
	if (
		u4[0] == static_cast<char>(0xca) &&
		u4[1] == static_cast<char>(0xfe) &&
		u4[2] == static_cast<char>(0xba) &&
		u4[3] == static_cast<char>(0xbe)
	) { return 1; endianness = BIG; }
	// check for Little-Endian
	if (
		u4[3] == static_cast<char>(0xca) &&
		u4[2] == static_cast<char>(0xfe) &&
		u4[1] == static_cast<char>(0xba) &&
		u4[0] == static_cast<char>(0xbe)
	) { return 0; endianness = LITTLE; }

	// corrupted header
	err("potentially corrupted .class file: magic number 0xCAFEBABE not detected!\n");
}

// also must only be done once; this is because Java bytecode
//	is Big-Endian, and the machine this parser is running on
//	may actually be Little-Endian
bool isLittleEndian() {
	int a = 0x05;
	char* p = (char*) &a;
	if (*p == 0x05) {
		return 1;
	}
	return 0;
}

// small helper function for comprehending and printing access
//	flags for Fields, i.e. in the field pool of the classfile
//	(note that some flags are mutually exclusive, hence the
//	seemingly haphazard groupings of if-else blocks)
void parseAccessFlags(short f) {
	printf("\tAccess flags:\n");
	if (f & 0x0001) {			// ACC_PUBLIC
		printf("\t\tACC_PUBLIC\n");
	} else if (f & 0x0002) {		// ACC_PRIVATE
		printf("\t\tACC_PRIVATE\n");
	} else if (f & 0x0004) {		// ACC_PROTECTED
		printf("\t\tACC_PROTECTED\n");
	}

	if (f & 0x0008) {			// ACC_STATIC
		printf("\t\tACC_STATIC\n");
	}

	if (f & 0x0010) {			// ACC_FINAL
		printf("\t\tACC_FINAL\n");
	} else if (f & 0x0040) {		// ACC_VOLATILE
		printf("\t\tACC_VOLATILE\n");
	}

	if (f & 0x0080) {			// ACC_TRANSIENT
		printf("\t\tACC_TRANSIENT\n");
	}
	if (f & 0x1000) {			// ACC_SYNTHETIC
		printf("\t\tACC_SYNTHETIC\n");
	}
	if (f & 0x4000) {			// ACC_ENUM
		printf("\t\tACC_ENUM\n");
	}
}
