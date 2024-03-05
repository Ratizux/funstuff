#include "Sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int clocks(void(*function)(let *target,int size),int n)
{
	if(n<2) return -1;
	let *buffer=(let*)calloc(n,sizeof(let));
	for(int i=0;i<n;i++) buffer[i]=rand();
	clock_t before=clock();
	function(buffer,n);
	if(buffer[0]>buffer[1])
	{
		printf("Error occured in algorithm.\n");
		return -1;
	}
	clock_t after=clock();
	free(buffer);
	return after-before;
}

double measure(void(*function)(let *target,int size),int n)
{
	return clocks(function,n)/((double)CLOCKS_PER_SEC/1000);
}

void benchmark(int n)
{
	if(n<=800)
	{
		const int repeat=10000;
		int i,ans;
		for(i=0,ans=0;i<repeat;i++) ans+=clocks(bubble_sort,n);
		printf("Bubble sort %d clocks\n",ans);
		for(i=0,ans=0;i<repeat;i++) ans+=clocks(selection_sort,n);
		printf("Selection sort %d clocks\n",ans);
		for(i=0,ans=0;i<repeat;i++) ans+=clocks(insertion_sort,n);
		printf("Insertion sort %d clocks\n",ans);
		#ifdef OPTIMIZE
		for(i=0,ans=0;i<repeat;i++) ans+=clocks(optimized_insertion_sort,n);
		printf("Optimized insertion sort %d clocks\n",ans);
		#endif
		for(i=0,ans=0;i<repeat;i++) ans+=clocks(shell_sort,n);
		printf("Shell sort %d clocks\n",ans);
		for(i=0,ans=0;i<repeat;i++) ans+=clocks(heap_sort,n);
		printf("Heap sort %d clocks\n",ans);
		for(i=0,ans=0;i<repeat;i++) ans+=clocks(merge_sort,n);
		printf("Merge sort %d clocks\n",ans);
		for(i=0,ans=0;i<repeat;i++) ans+=clocks(quick_sort,n);
		printf("Quick sort %d clocks\n",ans);
	}
	else if(n<=20000)
	{
		printf("Bubble sort %.3fms\n",measure(bubble_sort,n));
		printf("Selection sort %.3fms\n",measure(selection_sort,n));
		printf("Insertion sort %.3fms\n",measure(insertion_sort,n));
		#ifdef OPTIMIZE
		printf("Optimized insertion sort %.3fms\n",measure(optimized_insertion_sort,n));
		#endif
		printf("Shell sort %.3fms\n",measure(shell_sort,n));
		printf("Heap sort %.3fms\n",measure(heap_sort,n));
		printf("Merge sort %.3fms\n",measure(merge_sort,n));
		printf("Quick sort %.3fms\n",measure(quick_sort,n));
	}
	else if(n<=1e6)
	{
		printf("Shell sort %.3fms\n",measure(shell_sort,n));
		printf("Heap sort %.3fms\n",measure(heap_sort,n));
		printf("Merge sort %.3fms\n",measure(merge_sort,n));
		#ifdef OPTIMIZE
		printf("Optimized merge sort %.3fms\n",measure(optimized_merge_sort,n));
		#endif
		printf("Quick sort %.3fms\n",measure(quick_sort,n));
		#ifdef OPTIMIZE
		printf("Optimized quick sort %.3fms\n",measure(optimized_quick_sort,n));
		#endif
	}
	else
	{
		printf("Merge sort %.3fms\n",measure(merge_sort,n));
		#ifdef OPTIMIZE
		printf("Optimized merge sort %.3fms\n",measure(optimized_merge_sort,n));
		#endif
		printf("Quick sort %.3fms\n",measure(quick_sort,n));
		#ifdef OPTIMIZE
		printf("Optimized quick sort %.3fms\n",measure(optimized_quick_sort,n));
		#endif
	}
}

int main()
{
	srand((unsigned)time(NULL));
	printf("n=100\n");
	benchmark(100);
	printf("\n");
	printf("n=1000\n");
	benchmark(1000);
	printf("\n");
	printf("n=10000\n");
	benchmark(10000);
	printf("\n");
	printf("n=1e5\n");
	benchmark(1e5);
	printf("\n");
	printf("n=1e6\n");
	benchmark(1e6);
	printf("\n");
	printf("n=1e7\n");
	benchmark(1e7);
	printf("\n");
}
