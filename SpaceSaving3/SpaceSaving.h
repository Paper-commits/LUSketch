#ifndef STREAMCLASSIFIER_SPACESAVING_H
#define STREAMCLASSIFIER_SPACESAVING_H

#include <cstdint>
#include <cstdlib>
#include <unordered_map>
#include <vector>
#include "SpaceSavingUtils.h"


using namespace std;

#define tail_node nodes[0].prev

template<int capacity>
class SpaceSaving
{
    int now_element;
//    uint16_t tail_idx;
    Node nodes[capacity + 1];
    unordered_map<uint32_t, Node *> hash_table;

    void append_new_key(uint32_t key, int freq) {
        if (now_element < capacity) {
            uint16_t idx = ++now_element; // we use 0 to represent header
            nodes[idx].key = key;
            nodes[idx].val = 0;
            hash_table[key] = nodes + idx;

            // append to tail
            nodes[idx].prev = tail_node;
            tail_node->next = nodes + idx;
            nodes[idx].next = nodes;
            nodes[idx].parent = nodes + idx;
            tail_node = &nodes[idx];
            add_counter(tail_node, freq);
        } else {
            uint32_t old_key = tail_node->key;
            hash_table.erase(old_key);
            tail_node->key = key;
            hash_table[key] = tail_node;
            add_counter(tail_node, freq);
        }
    }

    void add_counter(Node * my, int freq) {
        //
        if (my->parent == my && my->next->val == my->val) {
//            std::swap(my->key, my->next->key);
//            std::swap(hash_table[my->key], hash_table[my->next->key]);
//            my = my->next;
            Node * p = my->next, * nt = my->next;
            while (p && p->val == my->val) {
                p->parent = nt;
                p = p->next;
            }
        }

        my->val += freq;
        uint32_t now_freq = my->val;
        Node * prev_node = my->prev;

        if (prev_node->val > now_freq) {
            return;
        }

        Node * next_node = my->next;

        // make next and prev connect
        prev_node->next = my->next;
        next_node->prev = my->prev;

        while (prev_node->val < now_freq) {
            prev_node = prev_node->parent->prev;
        }

        next_node = prev_node->next;

        my->next = prev_node->next;
        prev_node->next = my;

        my->prev = next_node->prev;
        next_node->prev = my;

        my->parent = (prev_node->val == my->val) ? prev_node->parent : my;
    }
public:
    SpaceSaving() : now_element(0) {
        memset(nodes, 0, sizeof(nodes));
        now_element = 0;
        nodes[0].val = -1;
        nodes[0].parent = nodes;
        tail_node = nodes;
        hash_table.reserve(100 * capacity);
    }

    void insert(uint32_t key, int freq = 1)
    {
//        Node *& my_node = hash_table[key];
        auto itr = hash_table.find(key);
        if (itr == hash_table.end()) {
            // key not found
            append_new_key(key, freq);
        } else {
            // key found
            add_counter(itr->second, freq);
        }
    }

    void get_top_k(uint16_t k, uint32_t * result) {
        Node * idx = nodes[0].next;

        int i;
        for (i = 0; i < k && i < capacity && i < now_element; ++i) {
            result[i] = idx->key;
            idx = idx->next;
        }

        for (; i < k; ++i) {
            result[i] = 0;
        }

        return;
    }

    void get_top_k_with_frequency(uint16_t k, vector<pair<uint32_t, uint32_t>> & result)
    {
        Node * idx = nodes[0].next;

        int i;
        for (i = 0; i < k && i < capacity && i < now_element; ++i) {
            result[i].first = idx->key;
            result[i].second = idx->val;
            idx = idx->next;
        }

        for (; i < k; ++i) {
            result[i].first = 0;
            result[i].second = 0;
        }

        return;
    }
    map<uint32_t,int> query_topk(int K)
    {
        Node *idx = nodes[0].next;
        map<uint32_t,int> result;
        cout<<"result:"<<endl;
        int i;
        uint32_t key;
        for( i =0;i<K && i<capacity && i<now_element;i++){
            key = idx->key;
            cout<<"key: "<<((key&0xff000000)>>24)<<"."<<((key&0x00ff0000)>>16)<<"."<<((key&0x0000ff00)>>8)<<"."<<(key&0x000000ff)<<"\t";
            cout<<"value: "<< idx->val <<endl;

            result.insert(make_pair(idx->key,idx->val));
            idx = idx->next;
        }
        for(;i<K;i++){
            result.insert(make_pair(0,0));
        }
        return result;

    }

};

#endif //STREAMCLASSIFIER_SPACESAVING_H
