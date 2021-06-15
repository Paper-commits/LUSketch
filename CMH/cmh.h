// we keep min-heap for find top-k element.
#ifndef _CMH_h
#define _CMH_h

#include "../common/param.h"
#include "../common/BOBHash32.h"
#include "../CountMinSketch/cm.h"
using namespace std;
//#define key_len 4

class CMH
{
//define heap part
private:
    typedef pair <uint32_t, int> KV;// 32 bit key, 32 bit count
    KV *heap;
    CMSketch *cm;
    int heap_memory;
    int cm_memory;
    uint32_t K;
    int key_len = 4;
    int heap_count = 0;
    int hash_count = 0;

    void clear()
    {
        for( uint32_t i = 0; i < K;i++)
        {
            heap[i].first = 0;
            heap[i].second = 0;
        }
    }

    void MINHEAPIFY(uint32_t i)
    {
        uint32_t left = 2*i +1;
        uint32_t right = 2*i + 2;
        uint32_t smallest = i;
        if(left < K && heap[left].second < heap[i].second){
            smallest = left;
        }
        if(right < K && heap[right].second < heap[smallest].second){
            smallest = right;
        }
        if(smallest != i){// swap
            SWAP(i, smallest);
            heap_count++;
            MINHEAPIFY(smallest);
        }
    }
    void SWAP(int i, int j){

        KV temp = heap[i];
        heap[i] = heap[j];
        heap[j] = temp;

    }

public:
    CMH(int total_memory, int top_k)
    {
        K = top_k;
        heap = new KV[top_k];
        heap_memory = sizeof(KV) * top_k ;
        cm_memory = total_memory - heap_memory;
        cm = new CMSketch(cm_memory);
        clear();
    }

    ~CMH()
    {
        delete []heap;
    }

    void print_basic_info()
    {
        cout<<"CMH"<<endl<<"heap memory: "<<heap_memory<<"B"<<endl;
        cm->print_basic_info();
 //       for(uint32_t i = 0;i < K;i++)
  //          cout<<"key: "<<heap[i].first<<"\t value: "<<heap[i].second<<endl;

    }

    void insert(uint32_t key)
    {
        cm->insert(key);
        hash_count += d;
        int tmp_key_count = cm->query(key);
        hash_count += d;
     //   cout<<"insert key: "<<key<<"\t tmp_key_count: "<<tmp_key_count<<endl;
        uint32_t find_key_index = 1 << 30;// K will not equal to this

        for(uint32_t i =0;i <K;i++){
            if(heap[i].first == key){
                find_key_index = i;
                break;
            }
        }
        if(find_key_index != (1<<30)){//inserting key has stored in heap
            heap[find_key_index].second = tmp_key_count;
            MINHEAPIFY(find_key_index);
            heap_count ++;
        } else if(tmp_key_count > heap[0].second){//value of inserting key is larger than minvalue in heap
            heap[0].first = key;
            heap[0].second = tmp_key_count;
            MINHEAPIFY(0);
            heap_count ++;
        }
     }

     map<uint32_t,int> query_topk()
     {
        map<uint32_t,int> result;
        cout<<"result:"<<endl;
        uint32_t key ;
        for(uint32_t i = 0;i < K;i++){
            key = heap[0].first;
            cout<<"key: "<<((key&0xff000000)>>24)<<"."<<((key&0x00ff0000)>>16)<<"."<<((key&0x0000ff00)>>8)<<"."<<(key&0x000000ff)<<"\t";
            cout<<"value: "<<heap[0].second<<endl;
            result.insert(make_pair(heap[0].first, heap[0].second));
            heap[0].first = 0;
            heap[0].second = 1<<30;
            MINHEAPIFY(0);
        }
        return result;
     }

     void query_heapnum()
    {
        cout<<"heapify operation number: "<<heap_count<<endl;
        cout<<"hash operation number: "<<hash_count<<endl;
    }


};
#endif
