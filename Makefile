t9search: src/cluster.c
	#gcc -std=c99 -Wall -Wextra -Werror -DNDEBUG src/cluster.c -o cluster -lm
	cc src/cluster.c -o cluster -lm
