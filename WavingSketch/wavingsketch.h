#ifndef _WAVINGSKETCH_H
#define _WAVINGSKETCH_H

#include "../common/param.h"
#include "../common/BOBHash32.h"
using namespace std;

#define slot_num 2 //refer to the author of wavingsketch.
#define INT_MAX 1<<30
#define factor 1

class WavingSketch
{
private:
    struct heavy_node{
        uint32_t ID;
        int frequency;
        bool errorflag;
    };

    struct bucket_node{
        int wavingcount;
        heavy_node heavy[slot_num];
    };
    struct heap_node{
        uint32_t key;
        int count;
    };
    BOBHash32 *hash_bucket = new BOBHash32(750);
    BOBHash32 *hash_sign = new BOBHash32(1000);
    int key_len=4;
    int bucket_memory;
    int bucket_size;
    int heap_memory;
    int K;
    bucket_node *bucket;
    heap_node *heap;

    void Clear()
    {
        for (int i =0;i<bucket_size;i++){
            bucket[i].wavingcount = 0;
            for(int j =0;j<slot_num;j++){
                bucket[i].heavy[j].ID = 0;
                bucket[i].heavy[j].frequency = 0;
                bucket[i].heavy[j].errorflag = true;
            }
        }

        for(int j =0;j<K;j++){
            heap[j].key = 0;
            heap[j].count = 0;
        }
    }

    void MINHEAPIFY(int i)
    {
        int left = 2*i +1;
        int right = 2*i + 2;
        int smallest = i;
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
    WavingSketch(int total_memory, int top_k)
    {
        K = top_k;
        total_memory = 50;//for test
        heap_memory = sizeof(heap_node)*top_k;
        bucket_memory = total_memory - heap_memory;
        bucket_size = floor(bucket_memory/sizeof(bucket_node));
        bucket = new bucket_node[bucket_size];
        heap = new heap_node[K];
        Clear();

    }

    void Insert(uint32_t pkt)
    {
        int value;
        value = Insert_bucket3(pkt);
        int find_key_index = INT_MAX;
        for(int i=0;i<K;i++){
            if(heap[i].key == pkt){
                find_key_index = i;
                break;
            }
        }

        if(find_key_index !=INT_MAX){
            //pkt held in heap
            heap[find_key_index].count = value;
            MINHEAPIFY(find_key_index);
        }else if(value > heap[0].count){
            heap[0].key = pkt;
            heap[0].count = value;
            MINHEAPIFY(0);
        }
    }
    int Insert_bucket2(uint32_t pkt)
    {
        int sign;
        if(hash_sign->run((const char*)&pkt, key_len) % 2 == 0)
            sign = -1;
        else sign = 1;

        int min_num = INT_MAX;
        int min_pos = -1;
        int row = hash_bucket->run((const char*)&pkt, key_len) % bucket_size;
        for(int i =0;i<slot_num;i++){
            if(bucket[row].heavy[i].frequency == 0){
                bucket[row].heavy[i].ID = pkt;
                bucket[row].heavy[i].frequency = -1;
                return abs(bucket[row].heavy[i].frequency);
            }else if(bucket[row].heavy[i].ID == pkt){
                if(bucket[row].heavy[i].frequency< 0 )
                    bucket[row].heavy[i].frequency--;
                else{
                    bucket[row].heavy[i].frequency++;
                    bucket[row].wavingcount += sign;
                }
                return abs(bucket[row].heavy[i].frequency);
            }
            if(abs(bucket[row].heavy[i].frequency) < min_num){
                min_num = bucket[row].heavy[i].frequency;
                min_pos = i;
            }
        }

        if(sign * bucket[row].wavingcount >= int(min_num*factor)){
            if(bucket[row].heavy[min_pos].frequency < 0) {
                int min_sign;
                if(hash_sign->run((const char*)&bucket[row].heavy[min_pos], key_len) % 2 == 0)
                    min_sign = -1;
                else min_sign = 1;
                bucket[row].wavingcount -= min_sign * bucket[row].heavy[min_pos].frequency;
            }
            bucket[row].heavy[min_pos].ID = pkt;
            bucket[row].heavy[min_pos].frequency = min_num + 1;
            return abs(bucket[row].heavy[min_pos].frequency);
        }
        bucket[row].wavingcount += sign;
        return 0;


    }

    int Insert_bucket3(uint32_t pkt)
    {
        int sign;
        if(hash_sign->run((const char*)&pkt, key_len) % 2 == 0)
            sign = -1;
        else sign = 1;

        int min_num = INT_MAX;
        int min_pos = -1;
        int row = hash_bucket->run((const char*)&pkt, key_len) % bucket_size;
        for(int i =0;i<slot_num;i++){
            if(bucket[row].heavy[i].frequency == 0){
                bucket[row].heavy[i].ID = pkt;
                bucket[row].heavy[i].frequency = 1;
                bucket[row].heavy[i].errorflag = true;
                return bucket[row].heavy[i].frequency;
            }else if(bucket[row].heavy[i].ID == pkt){

                bucket[row].heavy[i].frequency++;
                bucket[row].wavingcount += sign;
                return abs(bucket[row].heavy[i].frequency);
            }
            if(abs(bucket[row].heavy[i].frequency) < min_num){
                min_num = bucket[row].heavy[i].frequency;
                min_pos = i;
            }
        }
        bucket[row].wavingcount += sign;
        if(sign * bucket[row].wavingcount >= int(min_num*factor)){
            int min_sign;
            if(hash_sign->run((const char*)&bucket[row].heavy[min_pos], key_len) % 2 == 0)
                min_sign = -1;
            else min_sign = 1;
            if(bucket[row].heavy[min_pos].errorflag)
                bucket[row].wavingcount += min_sign * bucket[row].heavy[min_pos].frequency;
            bucket[row].heavy[min_pos].ID = pkt;
            bucket[row].heavy[min_pos].frequency = min_num + 1;
            bucket[row].heavy[min_pos].errorflag = false;
            return abs(bucket[row].heavy[min_pos].frequency);
        }
        else return 0;

    }

    int Insert_bucket(uint32_t pkt)
    {
        int sign;
        int min_value = INT_MAX;
        int min_pos = -1;
        if(hash_sign->run((const char*)&pkt, key_len) % 2 == 0)
            sign = -1;
        else sign = 1;
        int row = hash_bucket->run((const char*)&pkt, key_len) % bucket_size;
        for(int i = 0;i<slot_num;i++){
            //case 1, has empty
            if(bucket[row].heavy[i].frequency == 0){
                bucket[row].heavy[i].ID = pkt;
                bucket[row].heavy[i].frequency = 1;
                bucket[row].heavy[i].errorflag = true;
                return 1;
            }else if(bucket[row].heavy[i].ID == pkt){
                //case 2, pkt has held in bucket[i]
                bucket[row].heavy[i].frequency ++;
                if(bucket[row].heavy[i].errorflag == false)
                    bucket[row].wavingcount += sign;
                return bucket[row].heavy[i].frequency;
            }
            //except case 1 and case2, we need get the min value and corresponding position;
            if(min_value<bucket[row].heavy[i].frequency){
                min_value = bucket[row].heavy[i].frequency;
                min_pos = i;
            }
        }

        if(abs(bucket[row].wavingcount) > min_value){
        //fi is abs(wavingcount); fr is min_value
            if(bucket[row].heavy[min_pos].errorflag == true){
                bucket[row].wavingcount += sign*min_value;
            }
            bucket[row].heavy[min_pos].ID = pkt;
            bucket[row].heavy[min_pos].errorflag = false;
            bucket[row].heavy[min_pos].frequency = abs(bucket[row].wavingcount)+1;
            return bucket[row].heavy[min_pos].frequency;
        }else{
            bucket[row].wavingcount += sign;
            return 0;
            // mouse flow was not stored in bucket, we cannot get the value.
            //luckily, it also won't held in heap, so just return a small value.
        }

    }
    void Print_basic_info()
    {
        cout<<"WavingSketch"<<endl<<"heap memory: "<<heap_memory<<"B"<<endl;
        cout<<"\t bucket number: "<<bucket_size<<endl;
        cout<<"\t memory: "<< bucket_size*sizeof(bucket_node)*8/1024/8.0<<"KB"<<endl;
    }

    void query_topk()
    {
        cout<<"result: "<<endl;
        uint32_t key;
        for(int i = 0;i<K;i++){
            key = heap[0].key;
            cout<<"key: "<<((key&0xff000000)>>24)<<"."<<((key&0x00ff0000)>>16)<<"."<<((key&0x0000ff00)>>8)<<"."<<(key&0x000000ff)<<"\t";
            cout<<"value: "<<heap[0].count<<endl;
            heap[0].key = 0;
            heap[0].count = 1<<30;
            MINHEAPIFY(0);
        }
    }

};

#endif
