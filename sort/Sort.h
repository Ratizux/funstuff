#ifndef SORT_H
#define SORT_H

#include <stdlib.h>
typedef int let;

void bubble_sort(let *target,int size);

void selection_sort(let *target,int size);

void insertion_sort(let *target,int size);

void shell_sort(let *target,int size);

void heap_sort(let *target,int size);

void merge_sort(let *target,int size);

void quick_sort(let *target,int size);

//#define OPTIMIZE

#ifdef OPTIMIZE
#include<pthread.h>
struct mtqsort_args_t
{
	let *target;
	int size;
};
typedef struct mtqsort_args_t mtqsort_args;
struct mtmsort_args_t
{
	let *target;
	int size;
	int threads_now;
	int threads_max;
};
typedef struct mtmsort_args_t mtmsort_args;
void *mtqsort(void *args);
void transaction_optimized_quick_sort(let *target,int size);
void optimized_quick_sort(let *target,int size);
void transaction_optimized_merge_sort(let *target,int size);
void optimized_merge_sort(let *target,int size);
void optimized_insertion_sort(let *target,int size);
#endif

#endif
