main:
	gcc -O3 -o main main.c Sort.c -Wall -Wextra -DTRACE -march=native
	gcc -O3 -o benchmark benchmark.c Sort.c -Wall -Wextra -DOPTIMIZE -DSMALL_N_OPTIMIZE -lpthread -march=native
	gcc -O3 -o measure measure.c Sort.c -Wall -Wextra -DOPTIMIZE -DSMALL_N_OPTIMIZE -lpthread -march=native
debug:
	gcc -o main main.c Sort.c -Wall -Wextra -fsanitize=undefined -fsanitize=address -DTRACE
	gcc -o test-no-malloc test.c Sort.c -Wall -Wextra -fsanitize=undefined -fsanitize=address -DOPTIMIZE -DNO_MALLOC
	gcc -o test test.c Sort.c -Wall -Wextra -fsanitize=undefined -DOPTIMIZE -lpthread
clean:
	rm -f main test test-no-malloc benchmark measure
