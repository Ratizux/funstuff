#include "Sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int max=1000;
let array[1000],brray[1000];

void tick(int n,void (*func_a)(let *target,int size),void (*func_b)(let *target,int size))
{
	for(int i=0;i<n;i++)
	{
		array[i]=brray[i]=rand()%10+1;
	}
	func_a(array,n);
	func_b(brray,n);
	for(int i=0;i<n;i++)
	{
		if(array[i]!=brray[i])
		{
			printf("Error occured.\n");
			exit(0);
		}
	}
}

void test(void (*func_a)(let *target,int size),void (*func_b)(let *target,int size))
{
	for(int i=1;i<max;i++)
	{
		for(int j=0;j<100;j++) tick(i,func_a,func_b);
		printf("(%d/%d)\n",i,max-1);
	}
	printf("OK\n");
}

int main()
{
	srand((unsigned)time(NULL));
	test(bubble_sort,selection_sort);
	test(bubble_sort,insertion_sort);
	test(selection_sort,insertion_sort);
	test(insertion_sort,shell_sort);
	#ifndef NO_MALLOC
	test(insertion_sort,heap_sort);
	test(insertion_sort,merge_sort);
	#endif
	test(insertion_sort,quick_sort);
	#ifdef OPTIMIZE
	test(insertion_sort,optimized_insertion_sort);
	#ifndef NO_MALLOC
	test(insertion_sort,optimized_quick_sort);
	test(insertion_sort,optimized_merge_sort);
	#endif
	#endif
}
