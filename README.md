# malloc

## TODO:
* Thread safety
* Sophisticated tests
* Benchmark tools

## Heuristic

Preallocated 'slabs' strategy works great, but beyond that - pure mmap decreases performance signficantly.
As we cannot preallocate infinite amount of memory and we must not be too greedy in that regard
a new algorithm beyond preallocated slabs is required. I have an idea to use so called heuristic
function in order to predict and choose best strategy to allocate memory smarter. For example, we
can track some statistical data such as:

  * Max heap size
  * Peak heap size
  * Current heap size
  * Current memory usage
  * Heap miss counter (How many times we didn't find space in our heap successively)
  * Number of blocks in the heap
  * List of blocks

When memory beyond slabs is requested we can search our huge heap in order to find suitable
block. If we couldn't find such a block due to insufficient heap size or fragmentaion - 
we count this request as a 'miss'. We can use this counter as an indicator that we missing a lot and
it's time to do something about it, like allocating more memory than requested in order to increase 
chance of 'hitting' the size of the next request.
