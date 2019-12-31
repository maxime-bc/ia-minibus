but: ia-minibus

ia-minibus: src/ia-minibus.c
	gcc -Wall -Wextra -std=c99 -o src/ia-minibus src/ia-minibus.c -lm

play:
	java -jar minibus.jar -b1 -lo C:src/ia-minibus;\
	cd html;\
	php -S localhost:8888 &\

.PHONY: but play
