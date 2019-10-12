#define NAME_SIZE 64
#define FUNC_NAME_SIZE NAME_SIZE
#define STR_SIZE NAME_SIZE		
char *readLen(char *ptr,int num)
{
	IF_NULL_RET_NULL(ptr);
	char *line=MALLOC_NUM(char,FUNC_NAME_SIZE);
	IF_NULL_RET_NULL(line);
	for(int i=0;i<num;i++)
		{
			line[i]=*ptr;
			ptr++;
		}
	line[num]='\0';
	return line;
}
char *copyStr(char *buffer)
{
	IF_NULL_RET_NULL(buffer);
	char *line=MALLOC_NUM(char,FUNC_NAME_SIZE);
	IF_NULL_RET_NULL(line);
	for(int i=0;i<FUNC_NAME_SIZE;i++)
		{
			line[i]=*buffer;
			buffer++;
		}
	return line;
}

