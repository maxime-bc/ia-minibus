but: ia-minibus-maxime

ia-minibus-maxime: src/ia-minibus-maxime.c
	gcc -Wall -Wextra -std=c99 -o src/ia-minibus-maxime src/ia-minibus-maxime.c -lm

play:
	java -jar minibus.jar -b1 -lo C:src/ia-minibus-maxime;\
	cd html;\
	php -S localhost:8888 &\

.PHONY: but play
