#ifndef HANDLE_H
#define HANDLE_H
#include "stdint.h"
#include <vector>
#include <queue>
#include <memory>
#include <iostream>

template <class T> class HandleBoundArray {
private:
    struct Handle {
        uint32_t index : 32-8;
        uint32_t generation: 8;

        const uint32_t toInteger() const {
            uint32_t handleInteger;
            *(&handleInteger) = *((uint32_t*)this);
            return handleInteger;
        }

        static const Handle fromInteger(uint32_t integer) {
            return *((Handle*)&integer);
        }
    };

    struct Entry {
        Handle handle;
        T value;
    };
    
    std::vector<Entry> m_entries;
    std::queue<uint32_t> m_freeList;
    T(*m_constructor)();
    
public:
    void resize(size_t size) {
        size_t initialSize = m_entries.size();
        m_entries.resize(size, Entry());

        for(int i = initialSize; i < size; i++) {
            m_freeList.push(i);
        }
    }

    HandleBoundArray(size_t initialSize, T (*constructor)())
    : m_constructor(constructor) {
        resize(initialSize);
    }

    uint32_t allocateHandle() {
        Handle handle;
        if(m_freeList.size() > 0) {
            uint32_t index = m_freeList.front();
            m_freeList.pop();

            m_entries[index].value = m_constructor();
            m_entries[index].handle.index = index;
            
            handle = m_entries[index].handle;           
        }
        else {
            Entry newEntry;
            newEntry.value = m_constructor();
            newEntry.handle.generation = 0;
            newEntry.handle.index = m_entries.size();

            handle = newEntry.handle;
            m_entries.push_back(newEntry);
        }

        return handle.toInteger();
    }

    void deallocateHandle(uint32_t handleInteger) {
        Handle handle = Handle::fromInteger(handleInteger);
<<<<<<< HEAD
        const std::string errorMessage = "Tried to deallocate invalid handle!\n"; 
        if(handle.index > m_entries.size() - 1) {
            std::cerr << errorMessage;
=======
        if(handle.index > m_entries.size() - 1) {
>>>>>>> e5bdf49 (implements an array dealing utilizing generational handles)
            return; // TODO: Throw an exception or something
        }

        const Entry &entry = m_entries[handle.index];
        if(entry.handle.generation != handle.generation) {
<<<<<<< HEAD
            std::cerr << errorMessage;
=======
>>>>>>> e5bdf49 (implements an array dealing utilizing generational handles)
            return; // TODO: Throw an exception or something
        }

        m_freeList.push(handle.index);
        m_entries[handle.index].handle.generation++;
    }

    T *get(uint32_t handleInteger) {
        Handle handle = Handle::fromInteger(handleInteger);
        Entry &entry = m_entries[handle.index];
        if(entry.handle.generation != handle.generation) {
<<<<<<< HEAD
            std::cerr << "Tried dereferencing invalid handle!" << std::endl;
=======
>>>>>>> e5bdf49 (implements an array dealing utilizing generational handles)
            return nullptr;
        }
        
        return &(entry.value);
    }

};

#endif