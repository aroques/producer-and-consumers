## Concurrent UNIX Processes and shared memory

### Producer and Consumers

**Problem:** In many applications, we have a number of processes trying to write into a single file. If any two processes open a file to append something to the file (think of logs as an example), the exact timing at which the file is closed becomes extremely important. This is the critical section problem.

For this assignment, we are going to simulate the writing of logs. In a nutshell, the problem can be specified as follows.There is a process who reads from a single text file, picks up the next line of text, and puts it in a single buffer. Let there be five buffers and our producer process keeps on trying to fill those five buffers. Then, this process sets a flag to indicate that one of the five buffers is full. The process can write into a buffer only if the buffer is empty. There are a number of other processes (consumers) who keep on looking for a buffer to become full. One of these processes should be able to pick up the text in the buffer and write it into the log file. The process must make sure that the file is not opened by another process for writing

### Master
Master will create one producer process and then continuously create consumer processes as long as it does not exceed the max running process limit.

### Producer
The producer when in its critical section will fill a buffer with data from data.txt.

### Consumer
The consumer will when in its critical section will read data from the first full buffer then exit and get in line to read more data at a later time.

### This Project is not Complete
This project is not complete.
- There is no logging 
- No conumers enter their critical sections

To build this program run:
```
make
```

To run:
```    
./master [-n number of consumers] < data.txt
```

To clean up run:
```
make clean
```

#### Below is my git log (in oneline format):
28d5d03 2018-02-21 18:26:07 -0600 Comment out code to debug  
f112225 2018-02-21 00:02:42 -0600 Debug segfault and now debug free  
2a04cda 2018-02-20 15:50:30 -0600 Remove debug printfs  
e20163e 2018-02-20 15:42:44 -0600 Update .gitignore  
cfedbf0 2018-02-20 15:39:37 -0600 Add get_timestamp()  
f640055 2018-02-20 14:47:44 -0600 Debug signal handling, start setting up log file writing  
166c00b 2018-02-20 11:10:57 -0600 Test peterson's  
4a60fea 2018-02-20 10:29:58 -0600 Test shared memory read/write  
a7aa67c 2018-02-19 20:51:58 -0600 Implement petersons algorithm in producer.c  
daee2cb 2018-02-19 20:25:42 -0600 Add include directory  
081a9c4 2018-02-19 20:24:23 -0600 Add include directory  
06fa20e 2018-02-19 20:19:26 -0600 Add printf statements to consumer/producer, test, clenup  
5773446 2018-02-19 20:11:10 -0600 Rename shmem functions, add global ID_STR_IDX constant  
ab97863 2018-02-19 20:00:26 -0600 Consolidate consumer/producer code  
5ed6623 2018-02-19 18:56:37 -0600 Add split_string()  
c53aa64 2018-02-18 19:23:08 -0600 Add data.txt  
9652e83 2018-02-18 18:51:43 -0600 Change SIGTERM to SIGINT  
57138ba 2018-02-18 18:11:43 -0600 Add signal handling  
02d5944 2018-02-18 13:36:16 -0600 Add detach_shmem()  
e2080ee 2018-02-18 13:17:11 -0600 Make clean  
81366a7 2018-02-18 13:15:27 -0600 Add petersons and shared_memory.c  
0188549 2018-02-18 08:15:22 -0600 Add function.c, add child_idx for consumer  
9a39b87 2018-02-17 22:48:46 -0600 Add shared memory stater code  
b5001d5 2018-02-17 21:14:49 -0600 Add exec one producer and n consumers functionalilty  
2738608 2018-02-17 20:54:38 -0600 Replace tabs with spaces  
f679952 2018-02-17 20:17:24 -0600 Edit comment indentation  
a3a49cf 2018-02-17 20:15:07 -0600 Add producer and consumer files and update makefile  
ece29c4 2018-02-17 20:14:21 -0600 Add producer and consumer files and update makefile  
e68001b 2018-02-17 19:43:52 -0600 Update README.md  
8927678 2018-02-17 19:41:19 -0600 Add description of problem  
b1086ea 2018-02-17 19:37:11 -0600 Initial commit  
