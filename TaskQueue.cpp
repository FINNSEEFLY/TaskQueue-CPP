#include "TaskQueue.h"

TaskQueue::TaskQueue(int numOfThreads) : numOfThreads(numOfThreads) {
    taskQueue = new std::queue<std::function<void()>>();
    mutex = CreateMutex(nullptr, false, nullptr);
    InitializeCriticalSection(&criticalSection);
    threads = new HANDLE[numOfThreads];
}

TaskQueue::~TaskQueue() {
    DeleteCriticalSection(&criticalSection);
    CloseHandle(this->mutex);
    delete this->threads;
    delete this->taskQueue;
}

void TaskQueue::Start() {
    for (int i = 0; i < numOfThreads; i++) {
        threads[i] = CreateThread(NULL, 0, Process, this, 0, NULL);
    }
}

void TaskQueue::Push(std::function<void()> task) {
    DWORD dwWaitResult;
    while (true) {
        dwWaitResult = WaitForSingleObject(mutex, INFINITE);

        if (dwWaitResult == WAIT_OBJECT_0) {
            taskQueue->push(task);
            ReleaseMutex(mutex);
            return;
        }
    }
}

bool TaskQueue::IsEmpty() {
    return taskQueue->empty();
}

void TaskQueue::WaitAll() {
    WaitForMultipleObjects(numOfThreads, threads, true, INFINITE);
}

std::function<void()> TaskQueue::Pop() {
    DWORD dwWaitResult;
    while (true) {
        dwWaitResult = WaitForSingleObject(mutex, INFINITE);

        if (dwWaitResult == WAIT_OBJECT_0) {
            auto task = taskQueue->front();
            if (taskQueue->empty()) {
                task = nullptr;
            } else {
                task = taskQueue->front();
                taskQueue->pop();
            }
            ReleaseMutex(mutex);
            return task;
        }
    }
}

DWORD TaskQueue::Process(LPVOID param) {
    auto taskQueue = (TaskQueue *) param;
    do {
        std::function<void()> task;
        EnterCriticalSection(&taskQueue->criticalSection);
        bool wasEmpty = taskQueue->IsEmpty();
        if (!wasEmpty) {
            task = taskQueue->Pop();
        }
        LeaveCriticalSection(&taskQueue->criticalSection);
        if (!wasEmpty) {
            task();
        }
    } while (!taskQueue->IsEmpty());
    return 0;
}

int TaskQueue::getNumOfThreads() const {
    return numOfThreads;
}
