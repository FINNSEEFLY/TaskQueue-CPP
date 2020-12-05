#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iterator>
#include "TaskQueue.h"

std::vector<std::string> *LoadStringsFromFile(std::string filename);

void SplitAndSortVectors(std::vector<std::vector<std::string> *> *partsVectors,
                         std::vector<std::string> *strings,
                         TaskQueue *taskQueue);

std::vector<std::string> *MergeVectors(std::vector<std::vector<std::string> *> *vectors);

void SaveStringsInFile(const std::string &filename, std::vector<std::string> *stringVector);

int main() {
    std::cout << "Enter the name of the file to sort: ";
    std::string filename;
    std::cin >> filename;
    auto loadedStrings = LoadStringsFromFile(filename);
    if (loadedStrings == nullptr) {
        std::cout << "Failed to load file";
        return -1;
    }
    int numOfThreads;
    std::cout << "Enter the number of threads: ";
    std::cin >> numOfThreads;
    auto *partsOfVectors = new std::vector<std::vector<std::string> *>();
    auto *taskQueue = new TaskQueue(numOfThreads);
    SplitAndSortVectors(partsOfVectors, loadedStrings, taskQueue);
    taskQueue->Start();
    taskQueue->WaitAll();

    auto *resultStrings = MergeVectors(partsOfVectors);
    SaveStringsInFile(std::string("new_" + filename), resultStrings);
    delete resultStrings;
    for (auto item: *partsOfVectors) {
        delete item;
    }
    delete partsOfVectors;
    delete taskQueue;
}

std::vector<std::string> *LoadStringsFromFile(std::string filename) {
    std::ifstream reader(filename);
    std::string str;
    if (!reader.good())
        return nullptr;
    auto *result = new std::vector<std::string>();
    while (std::getline(reader, str)) {
        if (str.empty()) continue;
        result->push_back(std::string(str + "\n"));
    }
    return result;
}

void SaveStringsInFile(const std::string &filename, std::vector<std::string> *stringVector) {
    std::ofstream writer(filename);
    std::copy(stringVector->begin(), stringVector->end(), std::ostream_iterator<std::string>(writer));
    writer.close();
}

void SplitAndSortVectors(std::vector<std::vector<std::string> *> *partsVectors,
                         std::vector<std::string> *strings,
                         TaskQueue *taskQueue) {
    int part = strings->size() / taskQueue->getNumOfThreads();
    int remains = strings->size() % taskQueue->getNumOfThreads();
    int size = strings->size();
    for (int i = 0; i < size;) {
        auto *sortVector = new std::vector<std::string>();
        partsVectors->push_back(sortVector);
        for (int j = 0; j < part; j++) {
            std::string str = (*strings)[i++];
            sortVector->push_back(str);
        }
        if (remains > 0) {
            std::string str = (*strings)[i++];
            sortVector->push_back(str);
            remains--;
        }
        taskQueue->Push([sortVector]() { std::sort(sortVector->begin(), sortVector->end()); });
    }
}


std::vector<std::string> *MergeVectors(std::vector<std::vector<std::string> *> *vectors) {
    if (vectors->empty()) return nullptr;
    auto resultVector = new std::vector<std::string>();
    std::vector<std::string> *tmpVector;
    for (auto item: *vectors) {
        tmpVector = new std::vector<std::string>();
        int resultSize = resultVector->size();
        int itemSize = item->size();
        int i = 0;
        int j = 0;
        while (i < resultSize && j < itemSize) {
            if (resultVector->data()[i] <= item->data()[j])
                tmpVector->push_back((*resultVector)[i++]);
            else
                tmpVector->push_back(item->data()[j++]);
        }

        while (i < resultSize)
            tmpVector->push_back((*resultVector)[i++]);

        while (j < itemSize)
            tmpVector->push_back((*item)[j++]);

        delete resultVector;
        resultVector = tmpVector;
    }
    return resultVector;
}