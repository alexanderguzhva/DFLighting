#ifndef MYDOUBLELINKEDLIST_H
#define MYDOUBLELINKEDLIST_H

#include <memory.h>

#include "mydoublelinkedlistelement.h"


namespace dflighting
{

//
template<typename T>
class MyDoubleLinkedList
{
public:
    MyDoubleLinkedList(int initialAmount)
    {
        Items = nullptr;
        FreeNList = nullptr;

        NFreeNList = 0;
        NItems = 0;

        ReallocUp(initialAmount);



        FirstItem = -1;
        LastItem = -1;
    }

    MyDoubleLinkedList() : MyDoubleLinkedList(16000)
    {
    }

    //
    int AddItem(T * item)
    {
        if (NFreeNList == NItems)
        {
            //// increase size
            int newSize = NItems * 3 / 2;
            ReallocUp(newSize);
        }

        ////
        if (NFreeNList == 0)
        {
            //// this is the very first element
            int nextItem = FreeNList[NFreeNList];
            NFreeNList += 1;

            FirstItem = nextItem;
            LastItem = nextItem;

            MyDoubleLinkedListElement<T> * element = &(Items[nextItem]);
            element->Data = item;
            //element->MyID = nextItem;
            element->Prev = -1;
            element->Next = -1;

            return nextItem;
        }
        else
        {
            //// more items
            int nextItem = FreeNList[NFreeNList];
            NFreeNList += 1;

            MyDoubleLinkedListElement<T> & last = Items[LastItem];
            last.Next = nextItem;

            MyDoubleLinkedListElement<T> * element = &(Items[nextItem]);
            element->Data = item;
            //element->MyID = nextItem;
            element->Next = -1;
            element->Prev = LastItem;

            ////
            LastItem = nextItem;

            return nextItem;
        }
    }

    //
    void DeleteItem(int itemID)
    {
        MyDoubleLinkedListElement<T> item = Items[itemID];

        //// affect prev & next
        int prev = item.Prev;
        int next = item.Next;
        if (prev != -1)
        {
            MyDoubleLinkedListElement<T> & prevItem = Items[prev];
            prevItem.Next = next;

            //// item is the last element
            if (next == -1)
                LastItem = prev;
        }

        if (next != -1)
        {
            MyDoubleLinkedListElement<T> & nextItem = Items[next];
            nextItem.Prev = prev;

            //// item is the first element
            if (prev == -1)
                FirstItem = next;
        }

        //// delete
        NFreeNList -= 1;
        FreeNList[NFreeNList] = itemID;
    }

    //
    MyDoubleLinkedListElement<T> * operator() (int itemID)
    {
        return &(Items[itemID]);
    }

    //
    void Clear()
    {
        NFreeNList = 0;
        FirstItem = -1;
        LastItem = -1;
    }

    //
    ~MyDoubleLinkedList()
    {
        if (Items != nullptr)
        {
            delete [] Items;
            Items = nullptr;
        }

        if (FreeNList != nullptr)
        {
            delete [] FreeNList;
            FreeNList = nullptr;
        }
    }

    //
    int Count() const
    {
        return NFreeNList;
    }

    //
    int First() const
    {
        return FirstItem;
    }

    //
    int Last() const
    {
        return LastItem;
    }

private:
    MyDoubleLinkedListElement<T> * Items;
    int NItems;

    int * FreeNList;
    int NFreeNList;

    int FirstItem;
    int LastItem;

    void ReallocUp(int newNItems)
    {
        MyDoubleLinkedListElement<T> * newItems = new MyDoubleLinkedListElement<T>[newNItems];
        if (Items != nullptr)
        {
            memcpy(newItems, Items, sizeof(MyDoubleLinkedListElement<T>) * NItems);
            delete [] Items;
        }

        Items = newItems;

        ////
        int * newFreeNList = new int[newNItems];
        if (FreeNList != nullptr)
        {
            memcpy(newFreeNList, FreeNList, sizeof(int) * NItems);
            delete [] FreeNList;
        }

        for (int i = NItems; i < newNItems; i++)
            newFreeNList[i] = i;

        FreeNList = newFreeNList;

        ////
        NItems = newNItems;
    }
};


}

#endif // MYDOUBLELINKEDLIST_H

