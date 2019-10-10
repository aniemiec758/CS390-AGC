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
	) { return 1; }
	// check for Little-Endian
	if (
		u4[3] == static_cast<char>(0xca) &&
		u4[2] == static_cast<char>(0xfe) &&
		u4[1] == static_cast<char>(0xba) &&
		u4[0] == static_cast<char>(0xbe)
	) { return 0; }

	// corrupted header
	printf("-> potentially corrupted .class file: magic number 0xCAFEBABE not detected!\n");
	exit(1);
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

// takes a u_x buffer and converts it to decimal (buffer initially
//	contains hex after ld(u_x))
/* TODO TODO TODO
long uBuffToDecimal(char* u, int len) {
	long dec = ;
	for (int i = 0; i < len; i++) { // LSB stored at end; buffers are Big-Endian
		
*/
