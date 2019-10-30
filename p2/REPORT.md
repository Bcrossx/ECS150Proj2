# User-level thread library
## Implementation
### Category: Queue
For this project we decided to use a doubly linked-list for our queue 
structure. Since we have the constraint that we need all of our operations 4
(apart from the iterate and delete operations) to be O(1) we made sure not to 
include any loops in these operations as well. If we did include loops in 
these operations, it would most likely lead to a time complexity greater than 
O(1).

#### Why a doubly linked-list?
We tossed around the idea of using an array structured queue but enqueue would 
be amortized O(1) as opposed to O(1) since realloc is an O(n) operation. So a 
doubly linked-list seemed like the most straight forward structure to 
implement our queue and ensure that it will always have a time complexity of 
O(1).

#### Testing
A lot of the testing for our queue was straightforward, with some exceptions. 
A lot of our test cases were just making sure the queue would react correctly 
to certain situations that we thought could be troublesome. Such as when the 
queue is NULL and we try performing the different functions. The testing for 
the queue_interate function proved the most difficult as the iterate function 
has a lot more depth to it and therefore could have a lot more issues we 
needed to worry about which were specified in the header.

### Category: Uthread
Implementing the Uthread proved to be a very challenging problem. We chose 
to us an enum because we feel it is a more readable way of defining the list 
of thread states (Ready, Running, Blocked, Zombie). We realized that we 
needed to add another state we call Uninitialized to satisfy the return 
condition in our join() operation when the thread to be joined cannot be 
found. From there we mainly followed the details in the uthread header file 
for the operations. We added a few extra functions for initializing the queue, 
thread control block, and context. We also included a function for our state 
switching since we found ourselves switching states multiple times in our 
operations.

#### Testing
For the Uthread testing we initially just made sure the testers worked as 
intended. Then for the testing in phase 3 we modified the test cases a bit 
by throwing our join() functions in as many combinations of places as 
possible. After everything worked as intended no matter where we put the 
join() function we could be sure it was working properly.

### Category: Preemption
The most difficult part in implementing this phase was just deciding where we 
needed to disable and enable preemption. We figured that once our uthread was 
initialized we should also enable preemption because the user thread would be 
running at the same time. We also found we need to enable preemption again 
after it has been disabled. Deciding when to disable preemption took a bit 
more thought and for this we walked through our operations step by step in 
order to find where preemption needed to be disabled.

#### Testing
For the testing of this operation we found a good way to show our enable and 
disable operations function properly would be to force a condition using an 
infinite while loop where preemption would be the only way to make it to a 
print function in another thread.

## Outside Sources
For function reference:
* [Linux man pages](https://linux.die.net/man/)
* Cplusplus.com
