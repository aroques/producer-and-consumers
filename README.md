## Concurrent UNIX Processes and shared memory

### Producer and Consumers

**Problem:** In many applications, we have a number of processes trying to write into a single file. If any two processes
open a file to append something to the file (think of logs as an example), the exact timing at which the file is closed
becomes extremely important. This is the critical section problem.

For this assignment, we are going to simulate the writing of logs. In a nutshell, the problem can be specified as
follows.There is a process who reads from a single text file, picks up the next line of text, and puts it in a single buffer. Let there be five buffers and our producer process keeps on trying to fill those five buffers. Then, this process sets a flag
to indicate that one of the five buffers is full. The process can write into a buffer only if the buffer is empty.
There are a number of other processes (consumers) who keep on looking for a buffer to become full. One of these
processes should be able to pick up the text in the buffer and write it into the log file. The process must make sure
that the file is not opened by another process for writing

To build this program run:
```
make
```

To run:
```    
./master [-n number of consumers]
```

To clean up run:
```
make clean
```

### Description of problems encountered:

Below is my git log (in oneline format):
