#ifndef STREAMCLASSIFIER_SPACESAVINGUTILS_H
#define STREAMCLASSIFIER_SPACESAVINGUTILS_H

#include <cstdint>
#include "../common/param.h"
#include "../common/BOBHash32.h"


struct Node
{
    uint32_t key;
    uint32_t val;
    Node * prev;
    Node * next;
    Node * parent;
};

struct HashNode {
    uint32_t key;
    Node * p_node;
    HashNode * next;
    HashNode * prev;
};

template<int capacity>
struct HashTable
{
    HashNode nodes[capacity];
    HashNode * free_list[capacity];
    int now_free;

    HashNode * get_hash_node(uint32_t key) {

        uint32_t idx = BOBHash32(750).run((const char*)&key,4)%capacity;

        HashNode * nd = nodes + idx;

        while (nd) {
            if (nd->key == key) {
                return nd;
            }
            nd = nd->next;
        }

    }

    void release_hash_node(HashNode * node) {
        if (node->prev) {
            node->prev->next = node->next;
        }
        if (node->next) {
            node->next->prev = node->prev;
        }
        free_list[now_free++] = node;
    }

    HashTable() {
        memset(nodes, 0, sizeof(nodes));
        for (int i = 0; i < capacity; ++i) {
            free_list[i++] = nodes + i;
        }
        now_free = capacity;
    }
};

#endif //STREAMCLASSIFIER_SPACESAVINGUTILS_H
