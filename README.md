# 2Q cache on C++

## Algorithm description
Detailed description can be find there: http://www.vldb.org/conf/1994/P439.PDF.
In theory 2Q Caching algorithm uses two queues and hashtable to remember most hot pages. First queue devided into two parts: input part and output part. In practice there are two different queue, i.e. this realization of algorithm uses three queues. If page added to cache, it goes to input queue. If page displaced from input queue, it goes to output queue. If page displaced from output queue, it goes to second queue, named "main". There are most hot pages. And it works like FIFO queue.
## How to use
There are makefile. To compilу cache test program use make or make cache_test. Test format: cache_size number_of_pages page1 page2 .... page_[number_of_pages]. There are standard test and result of testing (some with comments)
