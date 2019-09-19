#include<stdio.h>
#include<stdlib.h>
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
  
  *array=(int *)malloc((arrayLength+1)*sizeof(int))+1;

  if(*array==NULL)
    {
      return OVERFLOAT;
    }
  
  //save array length 
  *(*array-1)=arrayLength;

  return OK;
}


int arrayLength(int *array)
{
  return *(array-1);
}

status insertX(int *array,int x)
{
  int i=0;
  for(i=arrayLength(array)-2;array[i]>=x&&i!=-1;i--)
    {
      array[i+1]=array[i];
    }
  printf("i=%i\n",i);
  array[i+1]=x;
  return OK;
}

status foreachArray(int *array,status (* method)(int elem))
{
  for(int i=0;i<arrayLength(array);i++)
    {
      method(array[i]);
    }
  return OK;
}
status printInt(int i)
{
  printf("int=%i\n",i);
  return OK;
}
int main(int argc,char **argv)
{
  int *array=NULL;
  initArray(10,&array);
  printf("arrayLength=%i\n",arrayLength(array));
  for(int i=0;i<10;i++)
    {
      array[i]=2*i;
    }
  foreachArray(array,printInt);
  insertX(array,-2);
  foreachArray(array,printInt);
}
