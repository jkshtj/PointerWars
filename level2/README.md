# Pointer Wars 2025: Linked List Edition Level 2

# Have You Completed Level 1 Yet?
If you've emailed us at pointerwars2025@gmail.com, and we've said
you're good to go for Level 2, this document is for you. If not,
please stop reading and complete Level 1 first. The levels are 
intended to be done in order, and while it's not the end of the
world if you read ahead, you might learn more by not doing so.

# Tasks For This Week
This level moves from to the design and implementation of your
linked list in level 1, to the design and implementation of a queue
in level 2. The queue data structure often (but not always) makes
use of a linked list internally.

From here, software is provided to you for performance measurement.
We're moving from the realm of microbenchmarks, which require careful
setup and analysis, to "wall clock time", or simply the amount of time
that it takes for the program to run. It's substantially easier to
do this sort of benchmarking, just make sure (to the best of your ability)
that your computer isn't doing any other work, and run the program.

If you're on Linux, modifying your CPU governors such that all of your
CPU cores are running at their maximum frequencies is a further
benefit. Use ChatGPT, google, whatever, or send us email.

The performance program performs breadth first search over a directed
graph of which Wikipedia entries link to each other, and attempts to
find any path between two psuedo-randomly generated pages. The pages
are read in via the 'nodes' file. 

You can determine the impact of a performance change by simply
re-running your code, and seeing whether the time improves. The best
results (for paid participants), will come from local runs on our 
testing servers, and those runs can be compared against other
participants. Runs on your local machine obviously cannot, because
everyone has different machines :).

Note that the wall clock time is reported by the program, and does not
include the I/O to read in the directed graph, nor the I/O to print
out results after each search. That would skew results, so it isn't done.

If you're running on an ARM system, there is some rudimentary support
for dumping hardware performance counters. Send us email if you're 
interested in this.

## Task 1: Improve Linked List Implementations Focusing on Common Operations
The general suggestion we want to provide here is that you should make
your common operations fast, and generally avoid doing more work than 
needed, especially if a simple optimization exists.

Most participants, whether they be free or paid, implemented suboptimal
algorithms for use in a queue. Those will need to be corrected, otherwise
the execution of the performance program will time out. If it didn't time
out, you'd be waiting for days!

There was a hint in linked_list.h that specifies that you can add members
to the struct linked_list. Adding a size so that you avoid iteration
in linked_list_size() is a great idea. It's generally expected that if
a size() function exists in a data structure, that its worst case time
complexity is O(k), constant time. The tradeoff is an additional 8 bytes
of memory (sizeof(size_t) == 8 on my machine), perhaps a little more based
on how C pads structures, but it's trivial compared to computing the size
of a linked list that is huge. And it's further likely that if you're
in a memory constrained environment where you truly care about 8 bytes,
you're likely in a compute constrained one as well, so iteration to determine
a size is a "no go".

It's important to think about what your code might evolve into, and also
how your code is going to be used as is. A queue and stack are both
data structures that use a linked list. Part of being senior is thinking
about things like this, and solving problems like this before they're
actually problems! It takes years to build this skill; this is only feedback
that you should start thinking about it now so that you benefit from 
your effort later.

More importantly than size, consider the FIFO behavior of a queue. It continually
adds nodes to the end of the queue, and if you have to iterate from the beginning 
to the end of the queue to do so, it's really inefficient. 
One search will push 20,000,000 or so nodes to the queue in a loop, 
which means that around 400,000,000,000,000 iterations will occur, instead 
of 20,000,000. If those were US dollars, that's the difference between 10x 
the current US national debt and being independently wealthy. 
This is also why I say that the performance program would run for days, 
as implemented by many participants.

The typical solution to this is to just build a doubly linked list, but
that introduces another 8 bytes per node. If you know that you're not
going to be iterating backwards over the list, or that random insertion
into the list isn't common, can you avoid that overhead?

There's code complexity overhead both in terms of writing it and
to some extent executing it (the CPU now has to decide whether to iterate
from the head or the tail on insertion in some cases). The memory footprint,
which tends to be the other tradeoff we're using isn't small either:
20,000,000 nodes * 8 bytes per node is 152 MB of additional memory allocated.

Another solution is to implement a tail pointer, which mostly "just works"
with a singly linked list. The problem becomes when you try to remove the 
node that the tail pointer points to, as there is no tail->prev to rely on! 
Think about that, and try to implement it! Functional testing software
will eventually catch the bug.

If not, fall back on the doubly linked list. The implementation of a doubly
linked list might actually not be a bad idea, as it might make you think
of the optimization I'm describing above! 

## Task 2: Implement a Queue
The new code to write this week is to make use of your linked list
code and implement a queue. A queue is a FIFO structure (First In
First Out) which often is implemented as a linked list in its
underlying implementation. The queue will hold data of type
"unsigned int", just like the previous week's linked list, although
you should put some thought into how you might extend your linked
list to hold any C data type. It's not required for this week,
but a good thing to noodle on.

The queue has the following functions specified in queue.h, see
the header file for the exact function signatures.
 x queue_register_malloc()
 x queue_register_free()
 x queue_create()
 x queue_delete()
 x queue_push()
 x queue_pop()
 x queue_size()
 x queue_has_next()
 x queue_next()

Most of these behave exactly as one would expect, with a similar style
API as the linked_list of the previous week. The queue_push() 
inserts a node at the end of the linked_list. The queue_pop() function
removes the node at the end and provides its data to programmer, 
if the queue is non-empty. Note the change in function signature, 
return value is success/failure, and the pointer passed in is where
the value is updated on success.

The queue_has_next() function specifies whether a node can be popped,
and the queue_next() function specifies that value with the pointer
passed into it, subject to its existance. Importantly, queue_next()
does not pop the node off of the queue on success.

## Task 3: Pass Functional And Valgrind Tests
The functional test program has been updated to test both your linked
list and your queue. Note that the functional test program in level 1
didn't test removal from your linked list. Bugs may exist there.

Use 'make run_functional_tests' and 'make run_valgrid_tests' as you did
in the last level.

## Task 4: Run Performance Tests
Use 'make run_performance_tests' if you wish. You'll have to download
some data to your local machine, the program will error out the first
time you run it and give you directions.

# Next Steps for Paying Participants
Upon passing functional and valgrind tests, send us email for code
feedback and performance feedback.

Start looking at the queue_performance.c file. Your job in the next level 
is to optimize this. We will have some very specific directions for
you, so please just don't "go ham" on it. We want you to learn to follow
the scientific method as applied to improving performance, and will
have a blog post to show you how to do it.

# Next Steps Fore Free Participants
Upon passing functional and valgrind tests, send us email to let us
know you're ready for level 3.

Run the performance test program so that you have an idea of how
long it takes to run.

Start looking at the queue_performance.c file. Your job in the next level 
is to optimize this. We will have some very specific directions for
you, so please just don't "go ham" on it. We want you to learn to follow
the scientific method as applied to improving performance, and will
have a blog post to show you how to do it.
