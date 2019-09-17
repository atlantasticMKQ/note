#include<stdio.h>
int main()
{
  int x=0;
  int n=1;
  for(int i=0;i<n;i++)
    for(int j=i;j<n;j++)
      for(int k=j;k<n;k++)
	x+=1;
  printf("%i\n",x);
}
