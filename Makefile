ALL: COMPILE RUN
DEBUG: src/*.c
	gcc src/*.c -o build/litgc --std=c89 -g
	valgrind build/litgc build/test.lig build/test.html
COMPILE: src/*.c
	gcc src/*.c -o build/litgc --std=c89
RUN:
	build/litgc build/test.lig test.html
INSTALL:
	sudo cp build/litgc /usr/bin/litgc