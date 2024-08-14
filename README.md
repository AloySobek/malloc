# malloc

Simple malloc implementation based on preallocation stretegy. Several preallocated
clusters of fixed size blocks are available - tiny and small. The idea is that you
can customize the size of this blocks that fits for your application. For example,
if your application uses large number of small memory chunks you can preallocate
more tiny blocks so that every time you request new block of memory there is no
syscalls whatsoever. Calls to mmap(per example) are immensely, extremely expensive,
like dozens if not hundreds times slower than preallocation strategy

## Compatibility

I've tried compiling neovim and a bunch open source C application in order to test
my malloc and it worked great. Sometimes even improving application performances
which was weird considering the simplicity of my malloc

## Ideas

There is a gazilion of algorithms for efficient memory allocation. I guess there is
no silver bullet for everybody. That's probably why gamedevs always write their own allocator.

### Heuristic?

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
block. If we couldn't find such a block due to insufficient heap size or fragmentation - 
we count this request as a 'miss'. We can use this counter as an indicator that we missing a lot and
it's time to do something about it, like allocating more memory than requested in order to increase 
chance of 'hitting' the size of the next request.
