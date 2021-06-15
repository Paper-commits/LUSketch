#ifndef _HEAVYKEEPER_H
#define _HEAVYKEEPER_H

#include "../common/param.h"
#include "../common/BOBHash32.h"
using namespace std;

#define HK_d 1
#define HK_b 1.08
//#define d 2

class HeavyKeeper
{
private:
  //  int d =2;

    struct node{
        int C;//Count
        uint32_t FP;//fingerprint
    };
    node *bucket;

    struct heap_node{
        uint32_t key;
        int count;
    };
    heap_node *heap;

    int single_node_size = 8;//8 Byte;
    uint32_t K;
    BOBHash32 *hash[HK_d];
    int w;
    int row_size;
    int cm_memory;
    int heap_memory;
    uint32_t heap_size = 0;
    int key_len = 4;//4 byte;

    void Clear()
    {
        for(int i =0;i<w;i++)
            bucket[i].C = bucket[i].FP = 0;
        for(uint32_t i =0;i<K;i++)
            heap[i].key = heap[i].count = 0;
    }
    void CM_print_basic_info()
    {
        cout<<"HeavyKeeper part1(similar CMsketch): "<<endl<<"bucket number: "<<w<<endl<<"\t row size: "<<row_size<<endl;
        cout<<"\t memory: "<< w*single_node_size*8/1024/8.0<<"KB"<<endl;
    }
    int HEAP_find(uint32_t key)
    {
        int p = -1;
        for(uint32_t t = 0;t<K;t++){
            if(heap[t].key == key) return t;
        }
        return p;
    }
    int HEAP_getmin()
    {
        if(heap_size < K) return 0;
        return heap[0].count;
    }

    void MINHEAPIFY(uint32_t i)
    {
        uint32_t left = 2*i +1;
        uint32_t right = 2*i + 2;
        uint32_t smallest = i;
        if(left < K && heap[left].count < heap[i].count){
            smallest = left;
        }
        if(right < K && heap[right].count < heap[smallest].count){
            smallest = right;
        }
        if(smallest != i){// swap
            SWAP(i, smallest);
            MINHEAPIFY(smallest);
        }
    }
      void SWAP(int i, int j){

        heap_node temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;

    }


public:
    HeavyKeeper(int total_memory, int top_k)
    {
        K = top_k;
        heap = new heap_node[K];
        heap_memory = sizeof(heap_node) * top_k;
        cm_memory = total_memory - heap_memory;
        w = floor(cm_memory/single_node_size);
        row_size = floor(w/HK_d);
        bucket = new node[w];
        for(int i=0;i<HK_d;i++)
            hash[i] = new BOBHash32(i+750);
        Clear();
    }

    void Print_basic_info()
    {
        cout<<"HeavyKeeper"<<endl<<"heap memory: "<<heap_memory<<"B"<<endl;
        CM_print_basic_info();
    }

    void Insert(uint32_t pkt)
    {
        bool flag = false;
        int position = HEAP_find(pkt);
        if( position != -1) flag = true;// p== -1 means that the packet isn't held in heap;
        int max_count = 0;
        int hash_index;
        int tmp_count;

        for(int i = 0;i<HK_d;i++){
            hash_index = row_size*i + hash[i]->run((const char*)&pkt,key_len)%row_size;
            tmp_count = bucket[hash_index].C;
            if(bucket[hash_index].FP == pkt){
                if(flag || tmp_count <= HEAP_getmin())
                    bucket[hash_index].C++;
                max_count = max(max_count,bucket[hash_index].C);
            } else
            {
                if(!(rand()%int(pow(HK_b,bucket[hash_index].C)))){//larger C is, more likely to be false
                    bucket[hash_index].C --;
                    if(bucket[hash_index].C<=0){
                        bucket[hash_index].C = 1;
                        bucket[hash_index].FP = pkt;
                        max_count = max(max_count,1);
                    }
                }
            }
        }
        if(!flag){//packet not held in heap
            if(heap_size < K){
                heap[0].key = pkt;
                heap[0].count = max_count;
                MINHEAPIFY(0);
                heap_size ++;
            }else if(max_count - HEAP_getmin() == 1){
                //replace pkt with min value in heap;
                heap[0].key = pkt;
                heap[0].count = max_count;
                MINHEAPIFY(0);
            }
        }else if (max_count > heap[position].count){
            //pkt was held in min heap; update correspond count
            heap[position].count = max_count;
            MINHEAPIFY(position);
            }


    }

    map<uint32_t,int> query_topk()
    {
        map<uint32_t,int> result;
        cout<<"result: "<<endl;
        int key;
        for(uint32_t i = 0;i<K;i++){
            key = heap[0].key;
            cout<<"key: "<<((key&0xff000000)>>24)<<"."<<((key&0x00ff0000)>>16)<<"."<<((key&0x0000ff00)>>8)<<"."<<(key&0x000000ff)<<"\t";
            cout<<"value: "<<heap[0].count<<endl;
            result.insert(make_pair(heap[0].key,heap[0].count));
            heap[0].key = 0;
            heap[0].count = 1<<29;
            MINHEAPIFY(0);
        }
        return result;
    }

};

#endif
