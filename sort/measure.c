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

int measure1000(void(*function)(let *target,int size),int n)
{
	int ans=0;
	for(int i=0;i<1000;i++) ans+=clocks(function,n);
	return ans;
}

double measure(void(*function)(let *target,int size),int n)
{
	return clocks(function,n)/((double)CLOCKS_PER_SEC/1000);
}

void benchmark(FILE *stream,const char *funcname,void(*function)(let *target,int size),int from,int to)
{
	const int pts=1000;
	int inc=1;
	if(to-from > pts) inc=(to-from)/pts;
	fprintf(stream,"# name: %s\n",funcname);
	fprintf(stream,"# type: matrix\n");
	fprintf(stream,"# rows: %d\n",(to-from)/inc);
	fprintf(stream,"# columns: 2\n");
	if(to<10000) for(int i=from;i<to;i+=inc)
	{
		fprintf(stream," %d %d\n",i,measure1000(function,i));
	}
	else for(int i=from;i<to;i+=inc)
	{
		fprintf(stream," %d %.2f\n",i,measure(function,i));
	}
	fprintf(stream,"\n\n");
}

int main()
{
	srand((unsigned)time(NULL));
	//10-10000
	//1e5-1e7
	//FILE *stream=stdout;
	FILE *stream=NULL;
	stream=fopen("sort.txt","w");
	benchmark(stream,"bubble_small",bubble_sort,2,100);
	benchmark(stream,"selection_small",selection_sort,2,100);
	benchmark(stream,"insertion_small",insertion_sort,2,100);
	#ifdef OPTIMIZE
	benchmark(stream,"insertion_optimized_small",optimized_insertion_sort,2,100);
	#endif
	benchmark(stream,"shell_small",shell_sort,2,100);
	benchmark(stream,"heap_small",heap_sort,2,100);
	benchmark(stream,"merge_small",merge_sort,2,100);
	benchmark(stream,"quick_small",quick_sort,2,100);
	//
	benchmark(stream,"bubble",bubble_sort,100,10000);
	benchmark(stream,"selection",selection_sort,100,10000);
	benchmark(stream,"insertion",insertion_sort,100,10000);
	#ifdef OPTIMIZE
	benchmark(stream,"insertion_optimized",optimized_insertion_sort,100,10000);
	#endif
	benchmark(stream,"shell",shell_sort,100,10000);
	benchmark(stream,"heap",heap_sort,100,10000);
	benchmark(stream,"merge",merge_sort,100,10000);
	benchmark(stream,"quick",quick_sort,100,10000);
	//benchmark(stream,"shell_large",shell_sort,1e5,1e7);
	benchmark(stream,"heap_large",heap_sort,1e5,1e7);
	benchmark(stream,"merge_large",merge_sort,1e5,1e7);
	#ifdef OPTIMIZE
	benchmark(stream,"merge_large_optimized",optimized_merge_sort,1e5,1e7);
	#endif
	benchmark(stream,"quick_large",quick_sort,1e5,1e7);
	#ifdef OPTIMIZE
	benchmark(stream,"quick_large_optimized",optimized_quick_sort,1e5,1e7);
	#endif
}
