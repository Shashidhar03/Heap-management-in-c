#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define max_heap_size 1024;

struct heap_Node
{
    int node_size;
    char variable_name[100];
    char status[20];
    int heap_start_address;
    int heap_end_address;
    struct heap_Node* next;
    struct heap_Node* prev;
};

struct free_Node
{
    int free_size;
    int free_start_address;
    int free_end_address;
    struct free_Node* next;
    struct free_Node* prev;
};

typedef struct heap_Node heap_node;
typedef struct free_Node free_node;

free_node* initilise_heap(free_node* free_lptr)         //it initilises heap as total free in freelist
{
    free_node* new_node=(free_node*)malloc(sizeof(free_node));
    new_node->free_size=max_heap_size;
    new_node->free_start_address=0;
    new_node->free_end_address=max_heap_size-1;
    new_node->next=NULL;
    new_node->prev=NULL;
    free_lptr=new_node;
    return free_lptr;
}

heap_node* sort_heap_list(heap_node* heap_lptr)       //sorts heap list according to start address
{
    heap_node *ifptr=heap_lptr;
    while(ifptr!=NULL)
    {
        heap_node *jfptr=ifptr;
        while(jfptr!=NULL&&jfptr->next!=NULL)
        {
            if((jfptr->heap_start_address)>(jfptr->next->heap_start_address))
            {
                int temp_end,temp_start,temp_size;
                char temp_name[100];

                temp_end=jfptr->heap_end_address;
                temp_start=jfptr->heap_start_address;
                temp_size=jfptr->node_size;
                strcpy(temp_name,jfptr->variable_name);
                
                jfptr->heap_start_address=jfptr->next->heap_start_address;
                jfptr->heap_end_address=jfptr->next->heap_end_address;
                jfptr->node_size=jfptr->next->node_size;
                strcpy(jfptr->variable_name,jfptr->next->variable_name);

                jfptr->next->heap_start_address=temp_start;
                jfptr->next->heap_end_address=temp_end;
                jfptr->next->node_size=temp_size;
                strcpy(jfptr->next->variable_name,temp_name);

            }
            jfptr=jfptr->next;
        }
        ifptr=ifptr->next;
    }
    return heap_lptr;
}

free_node* sort_free_list(free_node* free_lptr)           //sorts free list according to start address
{
    free_node *ifptr=free_lptr;
    while(ifptr!=NULL)
    {
        free_node *jfptr=ifptr;
        while(jfptr!=NULL&&jfptr->next!=NULL)
        {
            if((jfptr->free_start_address)>(jfptr->next->free_start_address))
            {
                int temp_end,temp_start,temp_size;

                temp_end=jfptr->free_end_address;
                temp_start=jfptr->free_start_address;
                temp_size=jfptr->free_size;
                
                jfptr->free_start_address=jfptr->next->free_start_address;
                jfptr->free_end_address=jfptr->next->free_end_address;
                jfptr->free_size=jfptr->next->free_size;

                jfptr->next->free_start_address=temp_start;
                jfptr->next->free_end_address=temp_end;
                jfptr->next->free_size=temp_size;

            }
            jfptr=jfptr->next;
        }
        ifptr=ifptr->next;
    }
    return free_lptr;
}

heap_node* insert_in_heap_with_first_allocation(free_node** free_ptr, heap_node* heap_lptr, int size, char name[100])
{
    free_node* free_head=*free_ptr;       //this to to update free_lptr in main
    free_node* free_lptr=free_head;       //this free lptr is for traversing
    int insert=0;
    while(insert==0&&free_lptr!=NULL)
    {
        if(free_lptr->free_size==size)  //if there is exact size then we will allocate heap block and delete free block
        {
            //allocating heap block
            heap_node* new_node=(heap_node*)malloc(sizeof(heap_node));
            new_node->node_size=size;
            strcpy(new_node->variable_name,name);
            strcpy(new_node->status,"allocated");

            new_node->heap_start_address=free_lptr->free_start_address;  //start and end address are set from free list
            new_node->heap_end_address=(new_node->heap_start_address)+(size-1);

            new_node->prev=NULL;          //its prev and next pointers are set
            new_node->next=heap_lptr;
            
            if(heap_lptr!=NULL)         //condition check if new node weather first node or not, if not previous node's prev should point to new node
            {
                heap_lptr->prev=new_node;
            }
            heap_lptr=new_node;

            //deleting free block
            free_node *dfptr=free_lptr;         //we need to delete free block, if there variable is requesting exact free size block
            
            if(dfptr!=*free_ptr)              //checking weather dptr is any node other than head of freelist
            {
                free_node *prev=dfptr->prev;      //these are needed because, before deleting its prev and next should be set,so that dptr 
                free_node *next=dfptr->next;       //can be easily deleted
                prev->next=next;
                if(next!=NULL)          //checking weather dptr->next is null or not,because if null, then its prev cannot be defined
                {                       //else its prev should point to dptr->prev
                    next->prev=prev;
                }
            }
            else                       //if dptr is head then, free list is moved forward by one step and dptr is deleted
            {
                free_lptr=free_lptr->next;
                if(free_lptr!=NULL)   //this condition for, if in free list only one node is there and after moving free list by one step
                {                       //if freelist becomes null then its prev cannot be defined  
                    free_lptr->prev=NULL;
                }
            }

            dfptr->next=dfptr->prev=NULL;      //deleting dptr
            free(dfptr);
            dfptr=NULL;
            
            // *free_ptr=free_lptr;
            insert=1;
        }
        else if(free_lptr->free_size>size) //if free block has more space then it splits free block into 2 parts 
        {
            //allocating heap block in free block and changing start and end address
            heap_node* new_node=(heap_node*)malloc(sizeof(heap_node));
            new_node->node_size=size;
            strcpy(new_node->variable_name,name);
            strcpy(new_node->status,"allocated");

            new_node->heap_start_address=free_lptr->free_start_address;
            new_node->heap_end_address=(new_node->heap_start_address)+(size-1);

            //changing start and end address of free block
            free_lptr->free_start_address=(new_node->heap_end_address)+1;
            free_lptr->free_size=(free_lptr->free_size)-size;
            free_lptr->free_end_address=(free_lptr->free_start_address)+(free_lptr->free_size-1);

            new_node->next=heap_lptr;
            new_node->prev=NULL;
            if(heap_lptr!=NULL)
            {
                heap_lptr->prev=new_node;
            }
            heap_lptr=new_node;

            // *free_ptr=free_lptr;
            insert=1;
        }
        else if(free_lptr!=NULL)  //if no above conditions satisfy, move by one step
        {
            free_lptr=free_lptr->next;
        }
    }

    if(free_lptr==NULL)
    {
        printf("\n heap is out of memory and cannot be inserted \n");
        insert=1;
    }
    else if(insert==1)
    {
        printf("memory is allocated\n");
    }
    
    *free_ptr=free_head;
    heap_lptr=sort_heap_list(heap_lptr);
    return heap_lptr;
}

heap_node* insert_in_heap_with_best_fit_allocation(free_node** free_ptr, heap_node* heap_lptr, int size, char name[100])
{
    free_node* free_head=*free_ptr;       //this to to update free_lptr in main
    free_node* free_lptr=free_head;      //this free lptr is for traversing
    free_node* min_fptr=NULL;             //this points to free block which has minimum size just greater than requested size

    int insert=0,min_size=1024;
    while(insert==0&&free_lptr!=NULL)
    {
        if(free_lptr->free_size==size)  //if there is exact size then we will allocate heap block and delete free block
        {
            //allocating heap block
            heap_node* new_node=(heap_node*)malloc(sizeof(heap_node));
            new_node->node_size=size;
            strcpy(new_node->variable_name,name);
            strcpy(new_node->status,"allocated");

            new_node->heap_start_address=free_lptr->free_start_address;  //start and end address are set from free list
            new_node->heap_end_address=(new_node->heap_start_address)+(size-1);

            new_node->prev=NULL;          //its prev and next pointers are set
            new_node->next=heap_lptr;
            
            if(heap_lptr!=NULL)         //condition check if new node weather first node or not, if not previous node's prev should point to new node
            {
                heap_lptr->prev=new_node;
            }
            heap_lptr=new_node;

            //deleting free block
            free_node *dfptr=free_lptr;         //we need to delete free block, if there variable is requesting exact free size block
            
            if(dfptr!=*free_ptr)              //checking weather dptr is any node other than head of freelist
            {
                free_node *prev=dfptr->prev;      //these are needed because, before deleting its prev and next should be set,so that dptr 
                free_node *next=dfptr->next;       //can be easily deleted
                prev->next=next;
                if(next!=NULL)          //checking weather dptr->next is null or not,because if null, then its prev cannot be defined
                {                       //else its prev should point to dptr->prev
                    next->prev=prev;
                }
            }
            else                       //if dptr is head then, free list is moved forward by one step and dptr is deleted
            {
                free_lptr=free_lptr->next;
                if(free_lptr!=NULL)   //this condition for, if in free list only one node is there and after moving free list by one step
                {                       //if freelist becomes null then its prev cannot be defined  
                    free_lptr->prev=NULL;
                }
            }

            dfptr->next=dfptr->prev=NULL;      //deleting dptr
            free(dfptr);
            dfptr=NULL;
            
            insert=1;
        }
        else if((size<free_lptr->free_size)&&(free_lptr->free_size<=min_size))
        {
            min_size=free_lptr->free_size;   //we sore minimum size and its pointer
            min_fptr=free_lptr;
            if(free_lptr!=NULL)
            {
                free_lptr=free_lptr->next;
            }
        }
        else if(free_lptr!=NULL)  //if no above conditions satisfy, move by one step
        {
            free_lptr=free_lptr->next;
        }
    }

    if(insert==0&&min_fptr!=NULL)
    {
        heap_node* new_node=(heap_node*)malloc(sizeof(heap_node));
        new_node->node_size=size;
        strcpy(new_node->variable_name,name);
        strcpy(new_node->status,"allocated");

        new_node->heap_start_address=min_fptr->free_start_address;
        new_node->heap_end_address=new_node->heap_start_address+(size-1);

        min_fptr->free_start_address=new_node->heap_end_address+1;
        min_fptr->free_size= min_fptr->free_size-(size);
        min_fptr->free_end_address=min_fptr->free_start_address+( min_fptr->free_size-1);

        new_node->prev=NULL;
        new_node->next=heap_lptr;
        if(heap_lptr!=NULL)
        {
            heap_lptr->prev=new_node;
        }

        heap_lptr=new_node;
        printf("memory is allocated \n");
    }
    else if(insert==1)
    {
        printf("memory is allocated \n");
    }
    else if(free_lptr==NULL)
    {
        printf("\n heap is out of memory and cannot be inserted \n");
    }

    *free_ptr=free_head;
    heap_lptr=sort_heap_list(heap_lptr);
    return heap_lptr;
}

free_node* combine_consecutive_free_blocks(free_node* free_lptr)  //it combines if consecutive free blocks are present
{
    free_node *fptr=free_lptr;
    while(fptr!=NULL&&fptr->next!=NULL)
    {
        if(fptr->next->free_start_address==fptr->free_end_address+1)
        {
            free_node *dfptr=fptr->next;
            fptr->free_end_address=dfptr->free_end_address;        //here we are updating into one free block and deleting next free block
            fptr->free_size=fptr->free_size+(dfptr->free_size);
            if(dfptr!=fptr)
            {
                free_node *prev=dfptr->prev;      //these are needed because, before deleting its prev and next should be set,so that dptr 
                free_node *next=dfptr->next;       //can be easily deleted
                prev->next=next;
                if(next!=NULL)          //checking weather dptr->next is null or not,because if null, then its prev cannot be defined
                {                       //else its prev should point to dptr->prev
                    next->prev=prev;
                }
            }
            
            dfptr->next=dfptr->prev=NULL;      //deleting dptr
            free(dfptr);
            dfptr=NULL;
        }
        fptr=fptr->next;
    }
    return free_lptr;
}

heap_node* deallocate(heap_node* heap_lptr, free_node** free_lptr, char to_deallocate[100])
{
    //search for variable, if found create new node in free list with same address and space and delete in heap list, if not found display not found
    heap_node* hptr=heap_lptr;
    free_node* fptr=*free_lptr;
    int done=0;
    while(done==0&&hptr!=NULL)
    {
        if(strcmp(hptr->variable_name,to_deallocate)==0)        //searching for variable
        {
            //found and allocating space in free list
            free_node *new_node=(free_node*)malloc(sizeof(free_node));
            new_node->free_size=hptr->node_size;
            new_node->free_start_address=hptr->heap_start_address;
            new_node->free_end_address=hptr->heap_end_address;
            new_node->prev=NULL;
            new_node->next=fptr;
            if(fptr!=NULL)
            {
                fptr->prev=new_node;
            }
            fptr=new_node;

            //deleting in heap list            
            if(hptr!=heap_lptr)   //if hptr is not head we need to set its prev and next node pointers
            {
                heap_node* prev=hptr->prev;
                heap_node* next=hptr->next;
                prev->next=next;
                if(next!=NULL)      //if hptr is last node, then its next is null, so this condition should be handled
                {
                    next->prev=prev;   
                }
            }
            else                 //if hptr is head ,then its prev should be made null
            {
                heap_lptr=hptr->next;
                if(heap_lptr!=NULL)
                {
                    heap_lptr->prev=NULL;
                }
            }

            hptr->next=hptr->prev=NULL; //in deleting node, we need to first set its internal pointers to null and then clear node
            free(hptr);
            hptr=NULL; 

            done=1;
            *free_lptr=fptr;
        }
        else
        {
            hptr=hptr->next;
        }

    }

    if(done==0)
    {
        printf("variable not found in heap\n");
    }
    else                    //after deleting variable ,we are sorting free list, so that we can allocate space for new variables
    {                       //from least starting address
        printf("variable is deallocated successfully\n");
        *free_lptr=sort_free_list(*free_lptr);
        
    }

    *free_lptr=combine_consecutive_free_blocks(*free_lptr);
    return heap_lptr;
}

void display_heap(heap_node* heap_lptr, free_node* free_lptr)
{
    //it displays heap in increasing order of starting address
    heap_node* hptr=heap_lptr;
    free_node* fptr=free_lptr;

    free_lptr=combine_consecutive_free_blocks(free_lptr);

    printf("variable_name\t variable_size\t variable_status\t start_address\t end_address\n");
    while(hptr!=NULL||fptr!=NULL)
    {
        if(hptr!=NULL&&fptr!=NULL)
        {
            if(hptr->heap_start_address<fptr->free_start_address)
            {
                printf("%s\t\t %d\t\t %s\t\t %d\t\t %d\n",hptr->variable_name,hptr->node_size,hptr->status,hptr->heap_start_address,hptr->heap_end_address);
                hptr=hptr->next;
            }
            else
            {
                printf("free\t\t %d\t\t free\t\t\t %d\t\t %d\n",fptr->free_size,fptr->free_start_address,fptr->free_end_address);
                fptr=fptr->next;
            }
        }
        else if(hptr!=NULL)
        {
            printf("%s\t\t %d\t\t %s\t\t %d\t\t %d\n",hptr->variable_name,hptr->node_size,hptr->status,hptr->heap_start_address,hptr->heap_end_address);
            hptr=hptr->next;
        }
        else if(fptr!=NULL)
        {
            printf("free\t\t %d\t\t free\t\t\t %d\t\t %d\n",fptr->free_size,fptr->free_start_address,fptr->free_end_address);
            fptr=fptr->next;
        }
    }
}

void  display_allocated_blocks(heap_node* heap_lptr)
{
    //it displays only allocated blocks in heap
    heap_node* hptr=heap_lptr;
    printf("variable_name\t variable_size\t variable_status\t start_address\t end_address\n");
    if(hptr==NULL)
    {
        printf("no blocks are allocated\n");
    }
    while(hptr!=NULL)
    {
        printf("%s\t\t %d\t\t %s\t\t %d\t\t %d\n",hptr->variable_name,hptr->node_size,hptr->status,hptr->heap_start_address,hptr->heap_end_address);
        hptr=hptr->next;
    }
}

void display_free_blocks(free_node* free_lptr)
{
    //it displays only free blocks in heap
    free_node* fptr=free_lptr;
    printf("variable_name\t variable_size\t variable_status\t start_address\t end_address\n");
    while(fptr!=NULL)
    {
        printf("free\t\t %d\t\t free\t\t\t %d\t\t %d\n",fptr->free_size,fptr->free_start_address,fptr->free_end_address);
        fptr=fptr->next;
    }
}

void free_all_heap(heap_node** heap_lptr, free_node** free_lptr)
{
    heap_node *hptr=*heap_lptr;
    free_node *fptr=*free_lptr;

    heap_node *dhptr=hptr;
    free_node *dfptr=fptr;

    while(hptr!=NULL)
    {
        dhptr=hptr;
        hptr=hptr->next;
        dhptr->next=dhptr->prev=NULL;
        free(dhptr);
        dhptr=NULL;
    }

    while(fptr!=NULL)
    {
        dfptr=fptr;
        fptr=fptr->next;
        dfptr->next=dfptr->prev=NULL;
        free(dfptr);
        dfptr=NULL;
    }

    *heap_lptr=NULL;
    *free_lptr=NULL;
}

int main()
{
    heap_node* heap_lptr=NULL;
    free_node* free_lptr=NULL;

    free_lptr=initilise_heap(free_lptr);

    int program,flag=0;
    
    while(flag==0)
    {
        printf("\nMAX HEAP SIZE IS 1024\n");
        printf("enter 1: to insert in heap with first fit allocation\n");
        printf("enter 2: to insert in heap with best fit allocation\n");
        printf("enter 3: to deallocate variable in heap\n");
        printf("enter 4: to display heap\n");
        printf("enter 5: to display allocated blocks\n");
        printf("enter 6: to display free blocks\n");
        printf("enter 0: to exit program\n");
        printf("enter:\n");
        scanf("%d",&program);
        printf("\n");

        if(program==1)
        {
            int size;
            char name[100];
            printf("enter variable size to be allocated\n");
            scanf("%d",&size);
            printf("variable name to be allocated\n");
            scanf("%s",name);

            heap_lptr=insert_in_heap_with_first_allocation(&free_lptr,heap_lptr,size,name);
        }
        else if(program==2)
        {
            int size;
            char name[100];
            printf("enter variable size to be allocated\n");
            scanf("%d",&size);
            printf("variable name to be allocated\n");
            scanf("%s",name);

            heap_lptr=insert_in_heap_with_best_fit_allocation(&free_lptr,heap_lptr,size,name);
        }
        else if(program==3)
        {
            char to_deallocate[100];
            printf("enter variable name to delete\n");
            scanf("%s",to_deallocate);

            heap_lptr=deallocate(heap_lptr,&free_lptr,to_deallocate);
        }
        else if(program==4)
        {
            display_heap(heap_lptr,free_lptr);
        }
        else if(program==5)
        {
            display_allocated_blocks(heap_lptr);
        }
        else if(program==6)
        {
            display_free_blocks(free_lptr);
        }
        else if(program==0)
        {
            flag=1;
            printf("program exited successfully by freeing all the heap\n");
            free_all_heap(&heap_lptr,&free_lptr);
        }
    }
    return 0;
}