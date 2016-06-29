
#ifndef _CALCULIB_LIST_H_
#define _CALCULIB_LIST_H_

#include <cstdlib>
#include <cstdio>

#define MAX_LOST_LIMIT 100
#define MAX_PREALLOC_SIZE 100


template <class T> class CalculibList {
private:
    T * table = NULL;
    unsigned int size = 0;
    unsigned int allocSize = 0;

    void changeAllocSize(int addToSize);
    
    
    

public:
    T get(unsigned int id);
    void set(unsigned int id, T);
    void add(T);
    void remove(unsigned int id);
    void searchAndRemove(T data);
    unsigned int getSize();
};

template <class T> T CalculibList<T>::get(unsigned int id)
{
    if(id >= size)
    {
        printf("CalculibList get  out of range id");
    }
    return table[id];
}

template <class T> void CalculibList<T>::set(unsigned int id, T data)
{
    if(id >= size)
    {
        printf("CalculibList get  out of range id");
    }
    table[id] = data;
}

template <class T> void CalculibList<T>::add(T data)
{
    if(size+1 > allocSize) // out of slot
    {
        changeAllocSize(((int)(MAX_PREALLOC_SIZE)/sizeof(T))+1); // add more than enough
    }
    
    table[size] = data;
    size++;
}
template <class T> void CalculibList<T>::searchAndRemove(T data)
{
    for(unsigned int pos = 0 ; pos < size ; pos++)
    {
        if(table[pos] == data)
        {
            remove(pos);
            pos--;
        }
    }
}


template <class T> void CalculibList<T>::remove(unsigned int id)
{
    unsigned int lostSize = ((allocSize-size)+1);
    memmove(table+id,table+id+1,(size-id-1)*sizeof(T));
    if(lostSize*sizeof(T) > MAX_LOST_LIMIT) // too much waste
    {
        changeAllocSize((int)-lostSize); // remove until it's compact
    }
    size--;
}

template <class T> void CalculibList<T>::changeAllocSize(int countToAdd)
{
    allocSize += (unsigned int)countToAdd;
    if(allocSize > 0)
    {
        table = (T*)realloc(table,allocSize*sizeof(T));
        if(table == NULL)
        {
            printf("CalculibList changeSize Out of memory");
        }
    }
    else
    {
        free(table);
        table = (T*)NULL;
    }
}

template <class T> unsigned int CalculibList<T>::getSize()
{
    return size;
}

#endif //_CALCULIB_LIST_H_