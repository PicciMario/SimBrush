SIMbrush : SIMbrush.o pcsc-wrappers.o dirtree.o utils.o xml-results.o
	gcc -o SIMbrush SIMbrush.o pcsc-wrappers.o dirtree.o utils.o xml-results.o -lpthread -lpcsclite

SIMbrush.o : SIMbrush.c
	gcc -c -ggdb SIMbrush.c

pcsc-wrappers.o : pcsc-wrappers.c pcsc-wrappers.h gsm-constants.h
	gcc -c -ggdb pcsc-wrappers.c

dirtree.o : dirtree.c dirtree.h gsm-constants.h
	gcc -c -ggdb dirtree.c

utils.o : utils.c utils.h
	gcc -c -ggdb utils.c

xml-results.o : xml-results.c
	gcc -c -ggdb xml-results.c

clean :
	rm -rf SIMbrush *.o *~
