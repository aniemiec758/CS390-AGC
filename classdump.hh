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
bool checkMagic(char* u4) {
	return (
		u4[0] == static_cast<char>(0xca) &&
		u4[1] == static_cast<char>(0xfe) &&
		u4[2] == static_cast<char>(0xba) &&
		u4[3] == static_cast<char>(0xbe)
	);
}

// takes a u_x buffer and converts it to decimal (buffer initially
//	contains hex after ld(u_x))
/* TODO TODO TODO
long uBuffToDecimal(char* u, int len) {
	long dec = ;
	for (int i = 0; i < len; i++) { // LSB stored at end; buffers are Big-Endian
		
*/
