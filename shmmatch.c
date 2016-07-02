#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
int matchNum=0;
void match(char* A,char*B,int* pi)
{
    int lenA=strlen(A);
    int lenB=strlen(B);
    int i=0;
    int cur_pos=-1;
    for(;i<lenA;++i)
    {
        while(cur_pos>=0 && A[i]!=B[cur_pos+1])
        {
            cur_pos=pi[cur_pos];
        }
        if(A[i]==B[cur_pos+1]) ++cur_pos;
        if(cur_pos+1==lenB) 
        {
            cur_pos=pi[cur_pos];//match
            //printf("match position %d\n",i);//print match position in string A.
            matchNum++;
        }



    }





}


void fail(char* B,int* pi)
{
    int lenB=strlen(B);
    pi[0]=-1;
    int i=1;
    int cur_pos=-1;
    for(;i<lenB;++i)
    {
        while(cur_pos>=0 && B[i]!=B[cur_pos+1])
        {
            cur_pos=pi[cur_pos];

        }

        if(B[i]==B[cur_pos+1]) ++cur_pos;
        pi[i]=cur_pos;

    }


}

int main(int argc,char* argv[]) {
   
    const char *name = "OS";
    const char *total_match_name = "Total";
    

    int shm_fd,total_fd;
    

    /* create the shared memory segment */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    total_fd = shm_open(total_match_name, O_CREAT | O_RDWR, 0666);


    /* configure the size of the shared memory segment */
    ftruncate(shm_fd,sizeof(int));
    ftruncate(total_fd,sizeof(int));


    /* now map the shared memory segment in the address space of the process */
    //To store the index of the assigned string for each child process.
    //and use begin_pos pointer to point to that space.
    int * begin_pos = mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (begin_pos == MAP_FAILED) {
        printf("Map failed\n");
        return -1;
    }


    //To store the total matches in this search.
    //and use total_match pointer to point to that space.
    int *total_match= mmap(0,sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, total_fd, 0);
    if (total_match == MAP_FAILED) {
        printf("Map failed\n");
        return -1;
    }





    pid_t child;
    int childstate;
    char *target_file=argv[1];
    char *A;//store file content
    char *key=argv[2];//store keywords
    int procNum=atoi(argv[3]);//store how many child process will be spawn
    int Length;
    int Remainder;
    int i;
    int cpt;//get the value of *begin_pos for the bound(Length+*begin_pos) of loop upper 
    

     // read the whole file contents into a buffer A
    FILE *file = fopen(target_file, "r");
    if (file) 
        {
            fseek(file, 0, SEEK_END);
            long fsize = ftell(file);
            fseek(file, 0, SEEK_SET);

            A = malloc(fsize + 1);
            fread(A, fsize, 1, file);
            fclose(file);
            A[fsize] = 0;
        }
        else
        {
            printf("file doesn't exist!\n");
        }
    ///end read the whole file contents





   /*
   /////test : print out all the content in target file.
    int j=0;
    for(;j<strlen(A);j++)
        printf ("%c",A[j]);
    printf ("\n");

    ////////////
    */



    //fork all the demanded child processes.
    for(i=1;i<=procNum;i++)
    {

        if((child=fork())==0)
        {
            //if(i==1)
            //*begin_pos =0;
           
            printf("hi process %d\n",i);
            
            //get the length of assignment for each child process. 
            Length=(strlen(A)-1)/procNum;//use strlen(A)-1 to avoid taking '\0' in to account.
             
            //get the Remainder 
            Remainder=(strlen(A)-1)%procNum;//use strlen(A)-1 to avoid taking '\0' in to account.
            //printf("this is Remainder %d\n",Remainder);
            
            /*if remainder doesn't equal 0,
            let the first Remainder processes handle one more character.
            */
            if(i<=Remainder)
            {
                Length++;



            }
            


            /*若要將一string 分為n個區段用n個process來同步執行
            ,為防止keyword因分段而被拆開,
            可讓除了最後一個process以外的所有processes
            都多處理原本負責的區塊後方的strlen(keyword)-1個字元,
            來solve keyword因分段被拆開而無法正確被搜尋到的缺失!
            */
            if(strlen(key)>1)
            {
                if(i<procNum)
                 {

                    Length=Length+strlen(key)-1;
                
                 }

            }
            
                printf("Length of assignment is %d in process %d\n",Length,i);
                char *H=malloc((Length+1)*(sizeof(char)));//use Length+1 to reserve the place for '\0' in the end of the search array in child process.
                H[Length]=0;//set the reserved place to '\0'.
                int k=0;//index of child process's search array.
                
                //move the assigned characters whose index is pointed by begin_pos to each child process's search array. 
                cpt=*begin_pos;
                for(;*begin_pos<Length+cpt;*begin_pos=*begin_pos+1)
                {
                    if(A[(*begin_pos)]!='\0')
                    {
                        H[k]=A[(*begin_pos)];
                        k++;
                    }
                
                }
                
                if(strlen(key)>1)
                {
                    *begin_pos=*begin_pos-(strlen(key)-1);
                }
                



                
                printf("The following is the child proecss%d's assignment.\n",i);
                //print out this child process's assigned characters.
                int j;
                for(j=0;j<strlen(H);j++)
                {
                    printf("%c",H[j]);
                }
                printf("\n");
                

                ////////////////search
                //create an int array pi with len==strlen(key) to keep track of where the keyword show up.
                int *pi=malloc(strlen(key)*sizeof(int));

                //keep track of where the keyword show up.
                fail(key,pi);

                //to match all the keyword from the assigned characters.
                match(H,key,pi);
                
                printf("There are %d matches in P%d \n\n",matchNum,i);
                free(pi);
                ////end search
                free(H);
                

                //accumulate the match in each child process
                *total_match=*total_match+matchNum;

            
                
            exit(0);
        }


       


        

    }
    
    //wait each child process to come to an end.
    for(i=1;i<=procNum;i++)
    {
        waitpid (child, &childstate, 0);
        //printf("child %d end\n",i) ;

    }

    //parent process print out the match finally result
    printf("Total matches: %d\n",*total_match);
    //*total_match=0;
    

    /* remove the shared memory segment OS ,which is used for begin_pos*/
    if (shm_unlink(name) == -1) {
        printf("Error removing %s\n",name);
        exit(-1);
    }
    /* remove the shared memory segment Total,which is used for total_match*/
     if (shm_unlink(total_match_name) == -1) {
        printf("Error removing %s\n",total_match_name);
        exit(-1);
    }
   
}