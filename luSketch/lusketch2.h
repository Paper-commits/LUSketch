#ifndef _LUSKETCH2_H
#define _LUSKETCH2_H

#include "../common/param.h"
#include "../common/BOBHash32.h"
//#include "lu_cm.h"
using namespace std;

#define single_counter_size 4
#define lu2_d 1

class LUSketch2
{
private:

    uint32_t K;
    int heap_memory;
    int cm_memory;
 //   int d = 1;
//    CMSketch *cm;

//count min variant
    int w = 0;
    int row_size = 0;
    int key_len = 4;
    BOBHash32 *hash[lu2_d];
    int hash_index_cm[lu2_d];
    int *bucket;
    //heap structure
    struct heap_node{
        uint32_t key;
        uint32_t value;
        uint32_t index[lu2_d];

    };
    heap_node *heap;
//   priority_queue<heap_node, vector<heap_node>,compare> Heap;

    void Clear()
    {
        for(uint32_t i = 0; i< K; i++){
            heap[i].key = 0;
            heap[i].value = 0;
            for( uint32_t j = 0;j < lu2_d;j++) heap[i].index[j] = 0;
        }
        for(int i = 0;i<w;i++) bucket[i] = 0;
    }

    void CM_Update_flag(uint32_t index)
    {
        bucket[index] = 0x40000000 | bucket[index]; // set the first bit of bucket[i] to 1.
    }
    int CM_Get_flag(uint32_t key)
    {
        int index;
        int flag = 1;
        for(int i =0;i<lu2_d;i++){
            index = (hash[i]->run((const char*)&key,key_len))%row_size;
            flag = min(flag,bucket[i*row_size + index]>>30);
        }
        return flag;
    }
    /*
    uint32_t min_count()
    {
        uint32_t min_value = 0;
        for(int i =0;i<d;i++) min_value = min(min_value,c)
    }
    */
    void cm_print_basic_info()
    {
        cout<<"CM Sketch"<<endl<<"\t counter number: "<<w<<endl<<"\t row size "<<row_size<<endl;
        cout<<"\t memory: "<< w*single_counter_size*8/1024/8.0<<"KB"<<endl;
    }
    int CM_insert(uint32_t key, int f =1)
    {
        int index;
        int flag = 1;
        for(int i=0;i<lu2_d;i++){
            index = (hash[i]->run((const char*)&key,key_len))%row_size;
            hash_index_cm[i] = index;
            bucket[i*row_size + index] += f;
            flag = min(flag, bucket[i*row_size + index]& 0x40000000);//&0x40000000 extract flsg.
        }
        return flag;
    }

    int CM_query(uint32_t key)
    {
        int index;
        int query_res = 0x3fffffff;
        int tmp_value;
        for(int i =0;i<lu2_d;i++)
        {
            index = (hash[i]->run((const char*)&key,key_len))%row_size;
            tmp_value = bucket[i*row_size+index] &0x3fffffff;
 //           tmp_value = tmp_value >> 1;
            query_res = min(query_res,tmp_value);
        }
        return query_res;
    }

    void MINHEAPIFY(uint32_t i)
    {
        uint32_t left = 2*i +1;
        uint32_t right = 2*i + 2;
        uint32_t smallest = i;
        if(left < K && heap[left].value < heap[i].value){
            smallest = left;
        }
        if(right < K && heap[right].value < heap[smallest].value){
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
    LUSketch2(int total_memory, int top_k)
    {
        K = top_k;
        heap = new heap_node[top_k];
        heap_memory = sizeof(heap_node) * top_k;
        cm_memory = total_memory - heap_memory;
        w = floor(cm_memory/single_counter_size);
        row_size = floor(w/lu2_d);
        bucket = new int[w];
        for(int i =0;i<lu2_d;i++){
            hash[i] = new BOBHash32(i+750);
        }

        Clear();

    }
    void Print_basic_info()
    {
        cout<<"LUSketch2"<<endl<<"heap memory: "<<heap_memory<<"B"<<endl;
        cm_print_basic_info();
        /***********test*************/
        /*
        for(uint32_t i = 0; i< K; i++){
            cout<<"key: "<<heap[i].key<<"\t value: "<<heap[i].value<< endl;

           for( uint32_t j = 0;j < d;j++) cout<<"index: "<<heap[i].index[j]<<"\t";
           cout<<endl;
        }
        */
    }

    void Insert(uint32_t packet)
    {
        uint32_t flag;
        flag = CM_insert(packet);
        //flag == 1 means that the packet stored in heap, so don't need other operations.
        if(flag == 0){
            uint32_t value = CM_query(packet);
            if(heap[0].value < value){
               Update_heap();//update HEAP, and flag in CM
               for( int t = floor(K/2); t>=0;t--){
                    MINHEAPIFY(t);
               }
               flag = CM_Get_flag(packet);
               if(flag == 0 && value>heap[0].value){
                    for(int i =0;i<lu2_d;i++){
                        bucket[heap[0].index[i]] = bucket[heap[0].index[i]] & 0x3fffffff; //set flag = 0;
                        heap[0].index[i] = hash_index_cm[i];
                    //    heap[0].index[i] = (hash[i]->run((const char*)&packet,key_len))%row_size;
                    }
                    heap[0].key = packet;
                    heap[0].value = value;
                    MINHEAPIFY(0);
               }
            }
        }
    }

    map<uint32_t,int> query_topk()
    {
        map<uint32_t,int> result;
        cout<<"result:"<<endl;
        //update
        Update_heap();
        for(int t = floor(K/2);t>=0;t--) MINHEAPIFY(t);
        //output result
        uint32_t key;
        for(uint32_t i = 0;i<K;i++){
            key = heap[0].key;
            cout<<"key: "<<((key&0xff000000)>>24)<<"."<<((key&0x00ff0000)>>16)<<"."<<((key&0x0000ff00)>>8)<<"."<<(key&0x000000ff)<<"\t";
            cout<<"value: "<<heap[0].value<<endl;
            result.insert(make_pair(heap[0].key,heap[0].value));
            heap[0].key = 0;
            heap[0].value = 1<<29;
            MINHEAPIFY(0);
        }
        return result;
    }
    void Update_heap()
    {
        uint32_t tmp_value ;
        uint32_t tmp_index;
        for(uint32_t i = 0; i < K;i ++){
            heap[i].value = 0x3fffffff;
            for(uint32_t j = 0;j<lu2_d;j++){
                tmp_index = heap[i].index[j] + row_size*j;
                tmp_value = bucket[tmp_index] & 0x3fffffff;
//                tmp_value = tmp_value >> 2;
                heap[i].value = min(heap[i].value, tmp_value);
                CM_Update_flag(tmp_index);
            }
        }
    }

};
#endif
