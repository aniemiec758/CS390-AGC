all: classdump

classdump:
	g++ -g -o classdump classdump.cc

clean:
	rm classdump
