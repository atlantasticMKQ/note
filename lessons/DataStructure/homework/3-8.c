#include<stdio.h>
#include<time.h>
#include<stdlib.h>
int isEven(int num)
{
	if(num%2==0)
		return 1;
	else
		return 0;
}
int division(int *array,int arrayLen)
{
	int *head,*tail;
	head=array;
	tail=array+arrayLen-1;
	int tmp=array[0];
	
	//op
	//0:head go
	//1:tail go
	for(int op=1;head!=tail;)
		{
			switch(op)
				{
				case 0:
					if(isEven(*head))
						{
							*tail=*head;
							op=1;
							tail--;
							//printf("head->tail:[%i],head:%i,tail:%i\n",*head,head-array,tail-array);
						}
					else
						{
							head++;
							op=0;
							//printf("head++,head:%i\n",head-array);
						}
					break;
				case 1:
					if(isEven(*tail))
						{
							tail--;
							op=1;
							//printf("tail++,tail:%i\n",tail-array);
						}
					else
						{
							*head=*tail;
							op=0;
							head++;
							//printf("tail->head:[%i],tail:%i,head:%i\n",*tail,tail-array,head-array);
						}
					break;
				}
		}
	*head=tmp;
}
int printArray(int *array,int arrayLen)
{
	for(int i=0;i<arrayLen;i++)
		{
			printf("array[%i]=%i\n",i,array[i]);
		}
}
int main()
{
	srand(time(NULL));
	int array[20];
	int arraym[1]={5};
	int arrayq[2]={4,4};
	for(int i=0;i<20;i++)
		{
			array[i]=rand()%100;
			
		}
	//printArray(array,20);
	//int arrayk[10]={11,3,2,4,3,5,8,6,7,4};
	division(arrayq,2);
	printArray(arrayq,2);
}
	
						      
							
			
	
