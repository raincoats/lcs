

all:
	gcc -Wall -Wextra -Wno-unused -ggdb3 -g3 -fno-stack-protector -fsanitize=address -fbuiltin -O0 -o subnet subnet.c
