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
1st argument:  path name of the input file    (Ex. testFile.txt)  
2nd argument: the keyword that needs to be searched (Ex. “oslab2015”)  (There are 9   “oslab2015” in testFile.txt in total.)
3rd argument: number of processes performing keyword-searching (Ex. 3)  
  Ex. $./multi_process testFile.txt "os2015" 3 

-Output 
print the result of occurrences both by each process and by all processes  
  Ex. process1:2 process2:3 process3:4 total:9
 (There are 9   “oslab2015” in testFile.txt in total.)
  
Note:
  fork() system call is used to create a new process 
  Use shared-memory API for communication between processes 
  If you search “121” from the file with content “1212121”, the result will be 3 
 
