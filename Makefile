all: clean classdump

classdump:
	gcc -g -o classdump classdump.cc

clean:
	rm classdump
