#ifndef SafeQueue_h
#define SafeQueue_h

#include <queue>
#include <mutex>
#include <condition_variable>

// This queue can be safely accessible from different threads
template <class T>
class SafeQueue
{
public:
    SafeQueue() { }
    ~SafeQueue() { }

    void enqueue(T value)
    {
        std::lock_guard<std::mutex> lock(mutexObject);
        
        try{
            baseQueue.push(value);
        }
        catch(std::bad_alloc& ex)
        {
            cout << "Not enough memory to store object in SafeQueue\n";
            return;
        }
        conditional.notify_one();
    }
    
    T dequeue()
    {
        std::unique_lock<std::mutex> lock(mutexObject);
    
        // This function only blocks if queue is empty, and notifications can only unblock the thread
        // when queue becomes not empty (which is specially useful to check against spurious wake-up calls).
        conditional.wait( lock, [&]{ return !baseQueue.empty(); } );
    
        T value = baseQueue.front();
        baseQueue.pop();
        return value;
    }
    
private:
    std::queue<T> baseQueue;
    mutable std::mutex mutexObject;
    std::condition_variable conditional;
};

#endif /* SafeQueue_h */
