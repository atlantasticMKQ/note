#include<stdio.h>

//ERROR_CODES

#define OVERFLOAT	-1
#define OK		0
#define BAD_ARGS	-2
typedef int status;

//ARRAY_TYPE_DEFINATION

status initArray(int arrayLength,int **array)
{
  if(arrayLength<=0)
    {
      return BAD_ARGS;
    }
  
  *array=(int *)malloc((arrayLength+2)*sizeof(int))+2;

  if(*array==NULL)
    {
      return OVERFLOAT;
    }
  
  //save array length 
  *(*array-2)=arrayLength;

  //number of elements in the array
  *(*array-1)=0;

  return OK;
}

status saveNumber(int *array)
  

