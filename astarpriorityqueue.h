#ifndef ASTARPRIORITYQUEUE_H
#define ASTARPRIORITYQUEUE_H

#include <map>
#include <queue>


namespace dflighting
{

// A*
template<typename U, typename V>
class AStarPriorityQueue
{
public:
    std::map<U, std::queue<V *> *> * List;

    AStarPriorityQueue()
    {
        List = new std::map<U, std::queue<V *> *>;
    }

    ~AStarPriorityQueue()
    {
        for (auto x : *List)
            delete x.second;

        delete List;
    }

    void Enqueue(U priority, V * value)
    {
        std::queue<V *> * q;

        auto iter = List->find(priority);
        if (iter == List->end())
        {
            //// not found
            q = new std::queue<V *>;
            List->insert(std::pair<U, std::queue<V *> *>(priority, q));
        }
        else
        {
            q = iter->second;
        }

        ////
        q->push(value);
    }

    V * Dequeue()
    {
        auto iter = List->begin();

        U priority = iter->first;
        std::queue<V *> * theQueue = iter->second;

        V * v = theQueue->front();
        theQueue->pop();

        if (theQueue->size() == 0)
        {
            List->erase(priority);
            delete theQueue;
        }

        return v;
    }

    bool IsEmpty()
    {
        return (List->size() == 0);
    }
};


}

#endif // ASTARPRIORITYQUEUE_H

