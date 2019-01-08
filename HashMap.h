#pragma once
#include <iostream>

using namespace std;


template<typename KeyType>
using hashFn = size_t (*) (const KeyType& key, size_t tableSize);


template<typename KeyType, typename ValType>
struct TableNode
{
    KeyType key;
    ValType value;
    TableNode<KeyType, ValType> *next;
    TableNode(const KeyType& _key, const ValType& _value,
              TableNode<KeyType, ValType> *_next = nullptr)
              : key(_key), value(_value), next(_next){}
};



template<typename KeyType, typename ValType>
class HashMap
{
private:
    hashFn<KeyType> hashfunc;
    TableNode<KeyType, ValType> **table;
    size_t size;
    size_t count;

    TableNode<KeyType, ValType>* locate(const KeyType& _key)const;
    void copy(const HashMap<KeyType, ValType>& other);
    void erase();
public:
    HashMap(size_t tableSize, hashFn<KeyType> _hashfunc);
    HashMap(const HashMap<KeyType, ValType>& other);
    HashMap<KeyType, ValType>& operator=(const HashMap<KeyType, ValType>& other);
    ~HashMap();

    void put(const KeyType& _key, const ValType& _value);
    bool putIfAbsent(const KeyType& _key, const ValType& _value);
    void remove(const KeyType& _key);
    void clear();
    ValType get(const KeyType& _key)const;
    bool containsKey(const KeyType& _key)const;
    bool containsValue(const ValType& _value)const;
    bool isEmpty()const;
    size_t getSize()const;
    size_t elementsCount()const;
    void print()const;
};



template<typename KeyType, typename ValType>
TableNode<KeyType, ValType>* HashMap<KeyType, ValType>::locate(const KeyType& _key)const
{
    size_t hashcode = hashfunc(_key, size);
    TableNode<KeyType, ValType> *crr = table[hashcode];
    while(crr && crr->key != _key)
    {
        crr = crr->next;
    }
    return crr;
}

template<typename KeyType, typename ValType>
void HashMap<KeyType, ValType>::copy(const HashMap<KeyType, ValType>& other)
{
    hashfunc = other.hashfunc;
    size = other.size;
    table = new TableNode<KeyType, ValType>*[size];
    for(size_t i = 0; i < size; i++)
    {
        if(other.table[i])
        {
            table[i] = new TableNode<KeyType, ValType>(other.table[i]->key, other.table[i]->value);
            TableNode<KeyType, ValType> *ptrToThis = table[i];
            TableNode<KeyType, ValType> *ptrToOther = other.table[i]->next;

            while(ptrToOther)
            {
                ptrToThis->next = new TableNode<KeyType, ValType>(ptrToOther->key, ptrToOther->value, ptrToOther->next);
                ptrToThis = ptrToThis->next;
                ptrToOther = ptrToOther->next;
            }
        }
        else
        {
            table[i] = nullptr;
        }
    }
    count = other.count;
}

template<typename KeyType, typename ValType>
void HashMap<KeyType, ValType>::erase()
{
    for(size_t i = 0; i < size; i++)
    {
        TableNode<KeyType, ValType> *crr = table[i];
        while(crr)
        {
            TableNode<KeyType, ValType> *todelete = crr;
            crr = crr->next;
            delete todelete;
        }
        table[i] = nullptr;
    }

    count = 0;
}

template<typename KeyType, typename ValType>
HashMap<KeyType, ValType>::HashMap(size_t tableSize, hashFn<KeyType> _hashfunc)
{
    hashfunc = _hashfunc;
    size = tableSize;
    table = new TableNode<KeyType, ValType>*[size];
    for(size_t i = 0; i < size; i++)
    {
        table[i] = nullptr;
    }
    count = 0;
}

template<typename KeyType, typename ValType>
HashMap<KeyType, ValType>::HashMap(const HashMap<KeyType, ValType>& other)
{
    copy(other);
}

template<typename KeyType, typename ValType>
HashMap<KeyType, ValType>& HashMap<KeyType, ValType>::operator=(const HashMap<KeyType, ValType>& other)
{
    if(this != &other)
    {
        erase();
        copy(other);
    }
    return *this;
}

template<typename KeyType, typename ValType>
HashMap<KeyType, ValType>::~HashMap()
{
    erase();
}

template<typename KeyType, typename ValType>
void HashMap<KeyType, ValType>::put(const KeyType& _key, const ValType& _value)
{
    TableNode<KeyType, ValType> *crr = locate(_key);
    if(crr)
    {
        crr->value = _value;
    }
    else
    {
        size_t hashcode = hashfunc(_key, size);
        table[hashcode] = new TableNode<KeyType, ValType>(_key, _value, table[hashcode]);
    }
    count++;
}

template<typename KeyType, typename ValType>
bool HashMap<KeyType, ValType>::putIfAbsent(const KeyType& _key, const ValType& _value)
{
    TableNode<KeyType, ValType> *crr = locate(_key);
    if(!crr)
    {
        size_t hashcode = hashfunc(_key, size);
        table[hashcode] = new TableNode<KeyType, ValType>(_key, _value);
        count++;
        return true;
    }
    return false;
}

template<typename KeyType, typename ValType>
void HashMap<KeyType, ValType>::remove(const KeyType& _key)
{
    if(!containsKey(_key))
    {
        throw "No such element";
    }
    size_t hashcode = hashfunc(_key, size);
    TableNode<KeyType, ValType> *crr = table[hashcode];
    if(crr->key == _key)
    {
        TableNode<KeyType, ValType> *todelete = crr;
        crr = crr->next;
        delete todelete;
        if(!crr)
            table[hashcode] = nullptr;
        else
            table[hashcode] = crr;
        count--;
        return;
    }
    while(crr->next->key != _key)
    {
        crr = crr->next;
    }
    TableNode<KeyType, ValType> *todelete = crr->next;
    crr->next = crr->next->next;
    delete todelete;
    count--;
}

template<typename KeyType, typename ValType>
void HashMap<KeyType, ValType>::clear()
{
    erase();
}

template<typename KeyType, typename ValType>
ValType HashMap<KeyType, ValType>::get(const KeyType& _key)const
{
    TableNode<KeyType, ValType> *el = locate(_key);
    if(!el)
        throw "Non existing key";
    return el->value;
}

template<typename KeyType, typename ValType>
bool HashMap<KeyType, ValType>::containsKey(const KeyType& _key)const
{
    return locate(_key) != nullptr;
}

template<typename KeyType, typename ValType>
bool HashMap<KeyType, ValType>::containsValue(const ValType& _value)const
{
    for(size_t i = 0; i < size; i++)
    {
        TableNode<KeyType, ValType> *crr = table[i];
        while(crr)
        {
            if(crr->value == _value)
                return true;
            crr = crr->next;
        }
    }
    return false;
}

template<typename KeyType, typename ValType>
bool HashMap<KeyType, ValType>::isEmpty()const
{
    return count > 0;
}

template<typename KeyType, typename ValType>
size_t HashMap<KeyType, ValType>::getSize()const
{
    return size;
}

template<typename KeyType, typename ValType>
size_t HashMap<KeyType, ValType>::elementsCount()const
{
    return count;
}

template<typename KeyType, typename ValType>
void HashMap<KeyType, ValType>::print()const
{
    for(size_t i = 0; i < size; i++)
    {
        TableNode<KeyType, ValType> *crr = table[i];
        while(crr)
        {
            cout << "(" << crr->key << ", " << crr->value << ")" << endl;
            crr = crr->next;
        }
    }
}

