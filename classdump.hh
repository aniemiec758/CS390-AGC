// %x will print a 4-byte hex value (sizeof an int),
//   %hhx prints a 1-byte hex value
void print_uBuff(char* u, int len) {
	for (int i = 0; i < len; i++) {
		printf("%hhx", u[i]);
	}
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
