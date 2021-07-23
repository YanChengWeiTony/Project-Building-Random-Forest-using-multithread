# [System Programming, 2018] Project-Building-Random-Forest-using-multithread
Description:
This is from the final project of the system programming at National Taiwan Universirt (NTU) CSIE department in 2018. The course was teached by CS Shih.

Building Random Forest using multithread

### Problem Description: 
  Write a random forest model using multiple threads, which finishs the following 4 tasks: reading training data from file, training the model, reading testing data from file, predicting the results. The execution time does decrease significantly with parrellel programming.
  
### What I have done:

  1. When building random forest, I use threads to build different decision
trees simultaneously. In each run, I first assign each thread a root,
which is for one tree. And then I wait for its result when I need to assign
new tasks to this thread.

  2. When I want to use random forest to predict result of test data, I use
thread to assign different test data to different thread at the same time.
In order to pursue better efficiency, I want to avoid creating and joining
threads too often. Thus, I maintain a read buffer (Capacity 4096 test
data) and divide these data to different threads equally. For example, if
there are 256 threads, each thread gets 16 test data and calculate them.
After reclaiming all data from all thread, I re-assign all threads again.

  3. Please refer to Report.pdf inside the folder "sphw4" for more details.

<!-- ![imag](https://github.com/YanChengWeiTony/Project-Building-Random-Forest-using-multithread/blob/master/thread_num_time.png) -->

<img src="https://github.com/YanChengWeiTony/Project-Building-Random-Forest-using-multithread/blob/master/thread_num_time.png" width="600" height="400">

<!-- ### Usage
```
git clone https://github.com/YanChengWeiTony/Project-Building-Random-Forest-using-multithread.git
cd Project-Building-Random-Forest-using-multithread
``` -->
