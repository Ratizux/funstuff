#include "Sort.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

const int max=1000;
let array[1000],brray[1000];

void (*func_a)(let *target,int size);
void (*func_b)(let *target,int size);

void tick(int n)
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

int mainloop()
{
	printf("[1] Input data\n");
	printf("[2] Generate random data\n");
	printf("[0] Exit\n");
	int resp;
	if(!scanf("%d",&resp))
	{
		printf("Invalid responce.\n");
		return 0;
	}
	if(resp==0) return 0;
	if(resp==1 || resp==2)
	{
		int n;
		let *target=NULL;
		if(resp==1)
		{
			printf("Size of input:\n> ");
			scanf("%d",&n);
			if(n<1)
			{
				printf("Invalid size.\n");
				return 1;
			}
			printf("Data:\n");
			target=(let*)calloc(n,sizeof(let));
			for(int i=0;i<n;i++)
			{
				printf("> ");
				if(scanf("%d",target+i))
				{
					printf("%d ok\n",target[i]);
				}
				else
				{
					printf("? invalid input\n");
					free(target);
					return 1;
				}
			}
		}
		else
		{
			printf("[1] 20 numbers from 0 to 9\n");
			printf("[2] 100 numbers from INT_MIN to INT_MAX\n");
			printf("[3] 100 numbers from 0 to 9\n");
			if(!scanf("%d",&resp))
			{
				printf("Invalid responce.\n");
				return 1;
			}
			if(resp==1)
			{
				n=20;
				target=(let*)calloc(n,sizeof(let));
				for(int i=0;i<n;i++) target[i]=rand()%10;
			}
			else if(resp==2)
			{
				n=100;
				target=(let*)calloc(n,sizeof(let));
				for(int i=0;i<n;i++) target[i]=rand();
			}
			else if(resp==3)
			{
				n=100;
				target=(let*)calloc(n,sizeof(let));
				for(int i=0;i<n;i++) target[i]=rand()%10;
			}
			else
			{
				printf("Invalid responce.\n");
				return 1;
			}
		}
		while(1)
		{
			printf("[1] Bubble sort\n");
			printf("[2] Selection sort\n");
			printf("[3] Insertion sort\n");
			printf("[4] Shell sort\n");
			printf("[5] Heap sort\n");
			printf("[6] Merge sort\n");
			printf("[7] Quick sort\n");
			printf("[0] Exit\n");
			if(!scanf("%d",&resp))
			{
				printf("Invalid responce.\n");
				free(target);
				return 1;
			}
			void (*sort_func)(let *target,int size);
			switch(resp)
			{
				case 1:sort_func=bubble_sort;break;
				case 2:sort_func=selection_sort;break;
				case 3:sort_func=insertion_sort;break;
				case 4:sort_func=shell_sort;break;
				case 5:sort_func=heap_sort;break;
				case 6:sort_func=merge_sort;break;
				case 7:sort_func=quick_sort;break;
				case 0:free(target);return 1;
				default:printf("Invalid responce.\n");free(target);return 1;
			}
			let *buffer;
			buffer=(let*)calloc(n,sizeof(let));
			memcpy(buffer,target,n*sizeof(let));
			sort_func(buffer,n);
			printf("Result:\n");
			for(int i=0;i<n;i++) printf("%d ",buffer[i]);
			free(buffer);
			buffer=NULL;
			printf("\n\n");
		}
		//free(target);
		//target=NULL;
	}
	return 0;
}

int main()
{
	srand((unsigned)time(NULL));
	while(mainloop());
}
