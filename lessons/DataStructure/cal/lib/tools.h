#define MALLOC(type) (type *)malloc(sizeof(type))
#define MALLOC_NUM(type,num) (type *)malloc(sizeof(type)*(num))
#define TOTAIL(head,tail) do{for(tail=head;tail->next!=NULL;tail=tail->next);}while(0)
#define IF_NULL_RET_NULL(ptr) do{if(ptr==NULL)return NULL;}while(0)
#define IF_NULL_DO_RET_NULL(ptr,state) do{if(ptr==NULL){state;return NULL;}}while(0)
#define IF_NULL_RET_OF(ptr) do{if(ptr==NULL)return OF;}while(0)
#define IF_NULL_RET(ptr,err) do{if(ptr==NULL)return err;}while(0)
#define IF_NULL_DO_RET_OF(ptr,state) do{if(ptr==NULL){state;return OF;}}while(0)
#define IF_NOT_OK_RET(err) do{if(err!=OK)return err;}while(0)
#define IF_NOT_OK_DO_RET(err,state) do{if(err!=OK){state;return err;}}while(0)
#define IF_STACK_OF(stack) do{if(err==OF){printf("stack OF\n");csFree(stack);return OF;}}while(0)
#define IFFUNC(root,func) if(strcmp(root->left->elem,#func)==0)

#define ERROR(error) do{printf("----------------------------------------\n");printf("Error:%s\n",error);printf("----------------------------------------\n");}while(0)

#define ARG_0(root) root->left
#define ARG_1(root) root->right->left
#define ARG_2(root) root->right->right->left
#define ARG_3(root) root->right->right->right->left
#define ARG_4(root) root->right->right->right->right->left
#define ARG_5(root) root->right->right->right->right->right->left

#define IF_EQ(str,name) if(strcmp(str,#name)==0)
#define IF_NEQ(str,name) if(strcmp(str,#name)!=0)
