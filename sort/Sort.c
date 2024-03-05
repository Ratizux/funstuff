#include "Sort.h"
#include <pthread.h>
#include <stdlib.h>

#include <string.h>

#ifdef TRACE
#include <stdio.h>
#endif

void swap(let *a,let *b)
{
	int tmp=*a;
	*a=*b;
	*b=tmp;
}

void move_forward_drop_first(let *target,int size)
{
	for(int i=1;i<size;i++)
	{
		target[i-1]=target[i];
	}
}

void move_backward_drop_last(let *target,int size)
{
	for(int i=size-1;i>0;i--)
	{
		target[i]=target[i-1];
	}
}

#ifdef TRACE
void highlight_print(const let *target,int size,int left,int right)
{
	for(int i=0;i<size;i++)
	{
		if(i==left) printf("[");
		printf("%d",target[i]);
		if(i==right) printf("]");
		if(i!=size-1) printf(" ");
	}
}

int digits(int target)
{
	int ans=1;
	if(target<0) ans++;
	while(target/=10) ans++;
	return ans;
}

void highlight_print_width(const let *target,int size,int left,int right,int gap,int width)
{
	{
		int n=gap;
		n*=width;
		if(left==0) n--;
		while(n>0)
		{
			n--;
			printf(" ");
		}
	}
	for(int i=0;i<size;i++)
	{

		int left_fill=0,right_fill=0;
		left_fill=(width-digits(target[i]))/2;
		right_fill=width-digits(target[i])-left_fill;
		for(int j=0;j<left_fill;j++) printf(" ");
		if(i==left) printf("[");
		printf("%d",target[i]);
		if(i==right) printf("]");
		for(int j=0;j<right_fill;j++) printf(" ");
		if(i!=size-1)
		{
			int n=gap*width*2+width;
			if(i+1==left) n--;
			if(i==right) n--;
			while(n--) printf(" ");
		}
	}
}
#endif

int array_max_pos(const let *target,int size)
{
	if(size<1) return -1;
	let max=target[0];
	int pos=0;
	for(int i=1;i<size;i++)
	{
		if(max<target[i])
		{
			max=target[i];
			pos=i;
		}
	}
	return pos;
}

void bubble_sort(let *target,int size)
{
	for(int i=size-1;i>0;i--)
	{
		#ifdef TRACE
		printf("Sort subarray [0]..[%d]: ",i);
		highlight_print(target,size,0,i);
		printf("\n");
		#endif
		for(int j=0;j<i;j++)
		{
			#ifdef TRACE
			highlight_print(target,size,j,j+1);
			#endif
			if(target[j]>target[j+1])
			{
				swap(&target[j],&target[j+1]);
			}
			#ifdef TRACE
			printf("   ->   ");
			highlight_print(target,size,j,j+1);
			printf("\n");
			#endif
		}
		#ifdef TRACE
		printf("\n");
		#endif
	}
}

void selection_sort(let *target,int size)
{
	for(int i=size-1;i>0;i--)
	{
		int max_pos=array_max_pos(target,i+1);
		let max_value=target[max_pos];
		#ifdef TRACE
		printf("Find the largest number in [0]..[%d] (",i);
		highlight_print(target,size,0,i);
		printf(")\n... Found [%d](%d)\n",max_pos,max_value);
		highlight_print(target,size,max_pos,max_pos);
		printf("   ->   ");
		#endif
		move_forward_drop_first(target+max_pos,i+1-max_pos);
		target[i]=max_value;
		#ifdef TRACE
		highlight_print(target,size,i,i);
		printf("\n\n");
		#endif
	}
}

void insertion_sort(let *target,int size)
{
	for(int i=1;i<size;i++)
	{
		#ifdef TRACE
		printf("Sort [0]..[%d] (",i);
		highlight_print(target,size,0,i);
		printf(")\n");
		#endif
		let tail=target[i];
		int dest=-1;
		for(int j=i-1;j>=0;j--)
		{
			if(target[j]<=tail)
			{
				dest=j;
				break;
			}
		}
		#ifdef TRACE
		highlight_print(target,dest+1,-1,-1);
		if(dest+1 != 0) printf(" ");
		printf("@ ");
		highlight_print(target+dest+1,i-dest-1,-1,-1);
		printf("   <-   %d\n",tail);
		highlight_print(target,size,i,i);
		printf("   ->   ");
		#endif
		move_backward_drop_last(target+dest+1,i-dest);
		target[dest+1]=tail;
		#ifdef TRACE
		highlight_print(target,size,dest+1,dest+1);
		printf("\n\n");
		#endif
	}
}

void custom_step_insertion_sort(let *target,int size,int increment)
{
	for(int i=increment;i<size;i+=increment)
	{
		let tail=target[i];
		int dest=-increment;
		for(int j=i-increment;j>=0;j-=increment)
		{
			if(target[j]<=tail)
			{
				dest=j;
				break;
			}
		}
		for(int j=i;j>dest+increment;j-=increment)
		{
			target[j]=target[j-increment];
		}
		target[dest+increment]=tail;
	}
}

void wrapper_shell_sort(let *target,int size,int increment)
{
	#ifdef TRACE
	printf("Increment: %d\n",increment);
	#endif
	for(int i=0;i<increment;i++)
	{
		#ifdef TRACE
		for(int j=0;j<size;j+=increment)
		{
			int len=increment;
			if(j+increment>=size) len=size-j;
			highlight_print(target+j,len,i,i);
			printf(" ");
		}
		printf("  ->   ");
		#endif
		custom_step_insertion_sort(target+i,size-i,increment);
		#ifdef TRACE
		for(int j=0;j<size;j+=increment)
		{
			int len=increment;
			if(j+increment>=size) len=size-j;
			highlight_print(target+j,len,i,i);
			printf(" ");
		}
		printf("\n");
		#endif
	}
	#ifdef TRACE
	printf("\n");
	#endif
}

int lowbit(int target)
{
	return target&-target;
}

int highbit(int target)
{
	while(target!=lowbit(target)) target^=lowbit(target);
	return target;
}

int log2int(int target)
{
	//log(target)/log(2)
	int ans=0;
	while(target>>=1) ans++;
	return ans;
}

int int2n(int target)
{
	//2^target
	if(target<0) return 0;
	int ans=1;
	while(target--)ans<<=1;
	return ans;
}

void shell_sort(let *target,int size)
{
	//increment sequence
	for(int inc=highbit(size);inc>1;inc>>=1) wrapper_shell_sort(target,size,inc-1);
}

struct heap_t
{
	int size,capacity;
	let *buffer;
};

typedef struct heap_t max_heap;

int binary_tree_node_width(int pos,int size)
{
	return int2n(log2int(size)-log2int(pos))-1;
}

#ifdef TRACE
int max_digits=1;
#endif

void heap_percolate_down(max_heap *src,int pos)
{
	if(src->size <= 1) return;
	int cur=pos;
	let tmp=src->buffer[cur];
	#ifdef TRACE
	int upper_bound=highbit(cur);
	for(int i=1;i<upper_bound;i*=2)
	{
		highlight_print_width(src->buffer+i,i,-1,-1,binary_tree_node_width(i,src->size-1),max_digits);
		printf("\n");
	}
	{
		int print_left=highbit(cur),print_right=print_left*2-1;
		if(print_right > src->size-1) print_right=src->size-1;
		highlight_print_width(src->buffer+print_left,print_right-print_left+1,cur-print_left,cur-print_left,binary_tree_node_width(print_left,src->size-1),max_digits);
		printf("\n");
	}
	#endif
	int endpoint=pos;
	while(cur*2 < src->size)
	{
		let target=cur*2;
		if(target+1 < src->size)
		{
			 if(src->buffer[target+1] > src->buffer[target]) target++;
		}
		#ifdef TRACE
		{
			int print_left=highbit(cur*2),print_right=print_left*2-1;
			if(print_right > src->size-1) print_right=src->size-1;
			if(tmp < src->buffer[target]) highlight_print_width(src->buffer+print_left,print_right-print_left+1,target-print_left,target-print_left,binary_tree_node_width(print_left,src->size-1),max_digits);
			else highlight_print_width(src->buffer+print_left,print_right-print_left+1,-1,-1,binary_tree_node_width(print_left,src->size-1),max_digits);
			printf("\n");
		}
		#endif
		if(tmp >= src->buffer[target])
		{
			endpoint=cur;
			cur=target;
			break;
		}
		src->buffer[cur]=src->buffer[target];
		cur=target;
		endpoint=cur;
	}
	#ifdef TRACE
	while(cur < src->size)
	{
		int print_left=highbit(cur*2),print_right=print_left*2-1;
		if(print_right > src->size-1) print_right=src->size-1;
		highlight_print_width(src->buffer+print_left,print_right-print_left+1,-1,-1,binary_tree_node_width(print_left,src->size-1),max_digits);
		printf("\n");
		cur*=2;
	}
	printf("\n");
	#endif
	src->buffer[endpoint]=tmp;
}

max_heap* heap_init(let *target,int size)
{
	max_heap *dest=(max_heap*)malloc(sizeof(max_heap));
	dest->capacity=size+1;
	dest->size=size+1;
	dest->buffer=(let*)calloc(dest->capacity,sizeof(let));
	//
	dest->buffer[0]=0;
	memcpy(dest->buffer+1,target,size*sizeof(let));
	#ifdef TRACE
	printf("Constructing priority queue using O(N) method...\n");
	#endif
	for(int i=(dest->size-1)/2;i>0;i--)
	{
		#ifdef TRACE
		printf("Percolate down...(%d/%d)\n",(dest->size-1)/2-i+1,(dest->size-1)/2);
		#endif
		heap_percolate_down(dest,i);
	}
	#ifdef TRACE
	printf("Done.\n");
	{
		int cur=1;
		while(cur < dest->size)
		{
			int print_left=highbit(cur),print_right=print_left*2-1;
			if(print_right > dest->size-1) print_right=dest->size-1;
			highlight_print_width(dest->buffer+print_left,print_right-print_left+1,-1,-1,binary_tree_node_width(print_left,dest->size-1),max_digits);
			printf("\n");
			cur*=2;
		}
	}
	printf("\n");
	#endif
	while(dest->size > 1)
	{
		int tail=dest->size-1;
		let tmp=dest->buffer[1];
		#ifdef TRACE
		printf("Pick max number, replace with the last number:\n");
		{
			int cur=1;
			while(cur < dest->size)
			{
				int print_left=highbit(cur),print_right=print_left*2-1;
				if(print_right > dest->size-1) print_right=dest->size-1;
				if(cur==1) highlight_print_width(dest->buffer+print_left,print_right-print_left+1,0,0,binary_tree_node_width(print_left,dest->size-1),max_digits);
				else if(cur*2 >= dest->size) highlight_print_width(dest->buffer+print_left,print_right-print_left+1,dest->size-1-highbit(dest->size-1),dest->size-1-highbit(dest->size-1),binary_tree_node_width(print_left,dest->size-1),max_digits);
				else highlight_print_width(dest->buffer+print_left,print_right-print_left+1,-1,-1,binary_tree_node_width(print_left,dest->size-1),max_digits);
				printf("\n");
				cur*=2;
			}
		}
		printf("\n");
		#endif
		dest->buffer[1]=dest->buffer[tail];
		dest->size--;
		dest->buffer[tail]=tmp;
		#ifdef TRACE
		printf("Sorted array: ");
		highlight_print(dest->buffer+dest->size,dest->capacity-dest->size,0,0);
		printf("\n\n");
		printf("Maintain the priority queue. Percolate down...\n");
		#endif
		heap_percolate_down(dest,1);
		#ifdef TRACE
		printf("\n");
		#endif
	}
	#ifdef TRACE
	printf("Done. Result: ");
	highlight_print(dest->buffer+1,size,-1,-1);
	printf("\n");
	#endif
	return dest;
}

max_heap* heap_destroy(max_heap *target)
{
	free(target->buffer);
	free(target);
	return NULL;
}

void heap_sort(let *target,int size)
{
	#ifdef TRACE
	for(int i=0;i<size;i++) if(max_digits<digits(target[i])) max_digits=digits(target[i]);
	#endif
	max_heap *heap=heap_init(target,size);
	memcpy(target,heap->buffer+1,size*sizeof(let));
	heap=heap_destroy(heap);
}

#ifdef TRACE
void transaction_merge(let *src1,int size1,let *src2,int size2,let *dest,int indent)
{
	int i=0,j=0,k=0;
	for(int i=0;i<indent;i++) printf("|   ");
	printf("Merge:\n");
	for(int i=0;i<indent;i++) printf("|   ");
	highlight_print(src1,size1,0,size1-1);
	printf(" + ");
	highlight_print(src2,size2,0,size2-1);
	printf(" = ");
	while(i<size1 && j<size2)
	{
		if(src1[i] < src2[j])
		{
			dest[k]=src1[i];
			i++;
		}
		else
		{
			dest[k]=src2[j];
			j++;
		}
		k++;
	}
	while(i<size1)
	{
		dest[k]=src1[i];
		i++;
		k++;
	}
	while(j<size2)
	{
		dest[k]=src2[j];
		j++;
		k++;
	}
	highlight_print(dest,size1+size2,0,size1+size2-1);
	printf("\n");
}
#else
void transaction_merge(let *src1,int size1,let *src2,int size2,let *dest)
{
	int i=0,j=0,k=0;
	while(i<size1 && j<size2)
	{
		if(src1[i] < src2[j])
		{
			dest[k]=src1[i];
			i++;
		}
		else
		{
			dest[k]=src2[j];
			j++;
		}
		k++;
	}
	while(i<size1)
	{
		dest[k]=src1[i];
		i++;
		k++;
	}
	while(j<size2)
	{
		dest[k]=src2[j];
		j++;
		k++;
	}
}
#endif

#ifdef TRACE
void transaction_sort(let *target,int size,let *orig_target,int orig_size,int indent)
{
	if(size<2) return;
	int mid=size/2;
	let *left=target;
	int left_size=mid;
	let *right=target+mid;
	int right_size=size-left_size;
	let *dest=(let*)calloc(size,sizeof(let));
	for(int i=0;i<indent;i++) printf("|   ");
	printf("Sort left:\n");
	for(int i=0;i<indent;i++) printf("|   ");
	highlight_print(orig_target,orig_size,target-orig_target,target-orig_target+left_size-1);
	printf("\n");
	transaction_sort(left,left_size,orig_target,orig_size,indent+1);
	for(int i=0;i<indent;i++) printf("|   ");
	printf("Sort right:\n");
	for(int i=0;i<indent;i++) printf("|   ");
	highlight_print(orig_target,orig_size,target-orig_target+left_size,target-orig_target+size-1);
	printf("\n");
	transaction_sort(right,right_size,orig_target,orig_size,indent+1);
	transaction_merge(left,left_size,right,right_size,dest,indent);
	memcpy(target,dest,size*sizeof(let));
	free(dest);
}
#else
void transaction_sort(let *target,int size)
{
	if(size<2) return;
	int mid=size/2;
	let *left=target;
	int left_size=mid;
	let *right=target+mid;
	int right_size=size-left_size;
	let *dest=(let*)calloc(size,sizeof(let));
	transaction_sort(left,left_size);
	transaction_sort(right,right_size);
	transaction_merge(left,left_size,right,right_size,dest);
	memcpy(target,dest,size*sizeof(let));
	free(dest);
}
#endif

void merge_sort(let *target,int size)
{
	#ifdef TRACE
	transaction_sort(target,size,target,size,0);
	#else
	transaction_sort(target,size);
	#endif
}

#ifdef TRACE
void transaction_quick_sort(let *target,int size,let *orig_target,int orig_size,int indent)
{
	if(size<2) return;
	int mid=(size-1)/2;
	let pivot;
	if(target[0]>target[mid]) swap(target,target+mid);
	if(target[mid]>target[size-1]) swap(target+mid,target+size-1);
	if(target[0]>target[mid]) swap(target,target+mid);
	pivot=target[mid];
	for(int i=0;i<indent;i++) printf("|   ");
	printf("Select pivot...\n");
	for(int i=0;i<indent;i++) printf("|   ");
	printf("%d [%d] %d\n",target[0],target[mid],target[size-1]);
	swap(target+mid,target+size-1);
	{
		int i,j;
		for(i=0,j=size-2;i<j;)
		{
			while(i<j && target[i]<pivot) i++;
			while(i<j && target[j]>=pivot) j--;
			swap(target+i,target+j);
		}
		swap(target+i,target+size-1);
		for(int i=0;i<indent;i++) printf("|   ");
		printf("Partition...\n");
		for(int i=0;i<indent;i++) printf("|   ");
		for(let *i=orig_target;i<target;i++)
		{
			printf("%d ",*i);
		}
		printf("[");
		highlight_print(target,size,i,j);
		printf("]");
		for(let *i=target+size;i<orig_target+orig_size;i++)
		{
			printf(" %d",*i);
		}
		printf("\n");
		if(i>1)
		{
			for(int i=0;i<indent;i++) printf("|   ");
			printf("Sort left:\n");
			for(int i=0;i<indent;i++) printf("|   ");
			highlight_print(orig_target,orig_size,target-orig_target,target-orig_target+i-1);
			printf("\n");
			transaction_quick_sort(target,i,orig_target,orig_size,indent+1);
		}
		if(size-i-1 > 1)
		{
			for(int i=0;i<indent;i++) printf("|   ");
			printf("Sort right:\n");
			for(int i=0;i<indent;i++) printf("|   ");
			highlight_print(orig_target,orig_size,target-orig_target+i+1,target-orig_target+size-1);
			printf("\n");
			transaction_quick_sort(target+i+1,size-i-1,orig_target,orig_size,indent+1);
		}
	}
}

void quick_sort(let *target,int size)
{
	transaction_quick_sort(target,size,target,size,0);
}
#else
void quick_sort(let *target,int size)
{
	if(size<2) return;
	int mid=(size-1)/2;
	let pivot;
	if(target[0]>target[mid]) swap(target,target+mid);
	if(target[mid]>target[size-1]) swap(target+mid,target+size-1);
	if(target[0]>target[mid]) swap(target,target+mid);
	pivot=target[mid];
	swap(target+mid,target+size-1);
	{
		int i,j;
		for(i=0,j=size-2;i<j;)
		{
			while(i<j && target[i]<pivot) i++;
			while(i<j && target[j]>=pivot) j--;
			swap(target+i,target+j);
		}
		swap(target+i,target+size-1);
		quick_sort(target,i);
		quick_sort(target+i+1,size-i-1);
	}
}
#endif

//#define OPTIMIZE

#ifdef OPTIMIZE
void optimized_insertion_sort(let *target,int size)
{
	#ifdef SMALL_N_OPTIMIZE
	if(size<100)
	{
		insertion_sort(target,size);
		return;
	}
	#endif
	for(int i=1;i<size;i++)
	{
		let tail=target[i];
		int dest=-1;
		if(target[i-1]<=target[i]) dest=i-1;
		else if(target[0]>target[i]) dest=-1;
		else
		{
			for(int l=0,r=i-1;;)
			{
				int mid=(l+r)/2;
				if(target[mid]<=tail)
				{
					if(target[mid+1]>tail)
					{
						dest=mid;
						break;
					}
					l=mid+1;
				}
				else r=mid-1;
				if(r==l)
				{
					dest=l;
					break;
				}
			}
		}
		move_backward_drop_last(target+dest+1,i-dest);
		target[dest+1]=tail;
	}
}

void *mtqsort(void *args)
{
	let *target=((mtqsort_args*)args)->target;
	int size=((mtqsort_args*)args)->size;
	transaction_optimized_quick_sort(target,size);
	return NULL;
}

void transaction_optimized_quick_sort(let *target,int size)
{
	if(size<2) return;
	#ifdef SMALL_N_OPTIMIZE
	if(size<75)
	{
		insertion_sort(target,size);
		return;
	}
	#endif
	int mid=(size-1)/2;
	let pivot;
	if(target[0]>target[mid]) swap(target,target+mid);
	if(target[mid]>target[size-1]) swap(target+mid,target+size-1);
	if(target[0]>target[mid]) swap(target,target+mid);
	pivot=target[mid];
	swap(target+mid,target+size-1);
	{
		int i,j;
		for(i=0,j=size-2;i<j;)
		{
			while(i<j && target[i]<pivot) i++;
			while(i<j && target[j]>=pivot) j--;
			swap(target+i,target+j);
		}
		swap(target+i,target+size-1);
		transaction_optimized_quick_sort(target,i);
		transaction_optimized_quick_sort(target+i+1,size-i-1);
	}
}

void optimized_quick_sort(let *target,int size)
{
	if(size<2) return;
	int mid=size/2;
	let *left=target;
	int left_size=mid;
	let *right=target+mid;
	int right_size=size-left_size;
	//
	pthread_t left_thread,right_thread;
	mtqsort_args left_args={left,left_size};
	mtqsort_args right_args={right,right_size};
	//
	int left_stat=pthread_create(&left_thread,NULL,mtqsort,&left_args);
	int right_stat=pthread_create(&right_thread,NULL,mtqsort,&right_args);
	//
	if(left_stat)
	{
		transaction_optimized_quick_sort(left,left_size);
	}
	else
	{
		pthread_join(left_thread,NULL);
	}
	if(right_stat)
	{
		transaction_optimized_quick_sort(right,right_size);
	}
	else
	{
		pthread_join(right_thread,NULL);
	}
	let *dest=(let*)calloc(size,sizeof(let));
	transaction_merge(left,left_size,right,right_size,dest);
	memcpy(target,dest,size*sizeof(let));
	free(dest);
}

void *mtmsort(void *args)
{
	let *target=((mtmsort_args*)args)->target;
	int size=((mtmsort_args*)args)->size;
	int threads_now=((mtmsort_args*)args)->threads_now;
	int threads_max=((mtmsort_args*)args)->threads_max;
	if(size<2) return NULL;
	#ifdef SMALL_N_OPTIMIZE
	if(size<75)
	{
		insertion_sort(target,size);
		return NULL;
	}
	#endif
	int mid=size/2;
	let *left=target;
	int left_size=mid;
	let *right=target+mid;
	int right_size=size-left_size;
	if(size>1e5 && threads_now<threads_max)
	{
		pthread_t left_thread,right_thread;
		mtmsort_args left_args={left,left_size,threads_now+2,threads_max};
		mtmsort_args right_args={right,right_size,threads_now+2,threads_max};
		int left_stat=pthread_create(&left_thread,NULL,mtmsort,&left_args);
		int right_stat=pthread_create(&right_thread,NULL,mtmsort,&right_args);
		if(left_stat)
		{
			transaction_optimized_merge_sort(left,left_size);
		}
		else
		{
			pthread_join(left_thread,NULL);
		}
		if(right_stat)
		{
			transaction_optimized_merge_sort(right,right_size);
		}
		else
		{
			pthread_join(right_thread,NULL);
		}
	}
	else
	{
		transaction_optimized_merge_sort(left,left_size);
		transaction_optimized_merge_sort(right,right_size);
	}
	let *dest=(let*)calloc(size,sizeof(let));
	transaction_merge(left,left_size,right,right_size,dest);
	memcpy(target,dest,size*sizeof(let));
	free(dest);
	return NULL;
}

void transaction_optimized_merge_sort(let *target,int size)
{
	if(size<2) return;
	#ifdef SMALL_N_OPTIMIZE
	if(size<75)
	{
		insertion_sort(target,size);
		return;
	}
	#endif
	int mid=size/2;
	let *left=target;
	int left_size=mid;
	let *right=target+mid;
	int right_size=size-left_size;
	let *dest=(let*)calloc(size,sizeof(let));
	transaction_optimized_merge_sort(left,left_size);
	transaction_optimized_merge_sort(right,right_size);
	transaction_merge(left,left_size,right,right_size,dest);
	memcpy(target,dest,size*sizeof(let));
	free(dest);
	return;
}

void optimized_merge_sort(let *target,int size)
{
	if(size<2) return;
	pthread_t main_thread;
	mtmsort_args main_args={target,size,1,6};
	int main_stat=pthread_create(&main_thread,NULL,mtmsort,&main_args);
	if(main_stat)
	{
		transaction_optimized_merge_sort(target,size);
	}
	else
	{
		pthread_join(main_thread,NULL);
	}
}
#endif
