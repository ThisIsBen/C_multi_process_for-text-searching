# C_multiple_processes
Objectives:
fork()  
shared memory 


How to run this program:
gcc shmmatch.c -lrt -o shmmatch 
./shmmatch pig.c(target filename) 121(keywords) 3(Num of child process) 


-Write a text-search program. It spawns multiple processes and have them search a file concurrently.  
The program should spilt the input file into multiple segments.  
Each process searches through a segment and counts the number of occurrences of the key word in that segment. 

-Input 
1st argument:  path name of the input file    (Ex. input.txt)  
2nd argument: the keyword that needs to be searched (Ex. “oslab2015”) 
3rd argument: number of processes performing keyword-searching (Ex. 3)  
  Ex. $./multi_process input.txt "os2015" 3 

-Output 
print the result of occurrences both by each process and by all processes  
  Ex. process1:5 process2:12 process3:2 total:19
  
  
Note:
  fork() system call is used to create a new process 
  Use shared-memory API for communication between processes 
  If you search “121” from the file with content “1212121”, the result will be 3 
 
