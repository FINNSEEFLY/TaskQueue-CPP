#ifndef TASKQUEUE_TASKQUEUE_H
#define TASKQUEUE_TASKQUEUE_H

#include <queue>
#include <functional>
#include "iostream"
#include "Windows.h"

#define DEFAULT_NUM_OF_THREADS 4

class TaskQueue {
private:
    std::queue<std::function<void()>>* taskQueue;
    HANDLE mutex;
    std::function<void()> Pop();
    int numOfThreads;
    bool IsEmpty();
public:
    explicit TaskQueue(int numOfThreads = DEFAULT_NUM_OF_THREADS);
    ~TaskQueue();
    void Push(std::function<void()>  task);
    void Start();
    static DWORD WINAPI Process(LPVOID param);
    void WaitAll();
    HANDLE* threads;
    CRITICAL_SECTION criticalSection;

    [[nodiscard]] int getNumOfThreads() const;
};


#endif //TASKQUEUE_TASKQUEUE_H
