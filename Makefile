but: ia-minibus

ia-minibus: src/ia-minibus.c
	gcc -o src/ia-minibus src/ia-minibus.c

play:
	java -jar minibus.jar -b2 C:src/ia-minibus;\
	cd html;\
	php -S localhost:8888 &\

.PHONY: but play
