#ifndef MYDOUBLELINKEDLISTELEMENT_H
#define MYDOUBLELINKEDLISTELEMENT_H

namespace dflighting
{

//
template<typename T>
class MyDoubleLinkedListElement
{
public:
    int Prev;
    int Next;
    //int MyID;
    T * Data;

    MyDoubleLinkedListElement()
    {
    }
};

}

#endif // MYDOUBLELINKEDLISTELEMENT_H

