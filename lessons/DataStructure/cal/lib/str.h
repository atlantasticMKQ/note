#define NAME_SIZE 32
#define FUNC_NAME_SIZE NAME_SIZE
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
#define IF_EQ(name) if(strcmp(str,#name)==0)
int strToType(char *str)
{
	IF_EQ(UNSET)
		return UNSET;
	IF_EQ(FUNC)
		return FUNC;
	IF_EQ(INT)
		return INT;
	IF_EQ(BOOL)
		return BOOL;
	IF_EQ(POLY)
		return POLY;
	IF_EQ(STR)
		return STR;
}
