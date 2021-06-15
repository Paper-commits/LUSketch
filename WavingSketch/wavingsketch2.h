#ifndef _WAVINGSKETCH2_H
#define _WAVINGSKETCH2_H

#include "../common/param.h"
#include "../common/BOBHash32.h"
#include <stdlib.h>
using namespace std;

#define factor 1
#define hash hash32
typedef uint32_t data_type; // note: according to your dataset
typedef int32_t count_type; // note: according to your dataset
#define slot_num 8
#define INT_MAX 1<<30
#define factor 1

/*
inline uint32_t hash32(data_type item, uint32_t seed = 0) {
    return BOBHash32(750)->run((const char*)&item,4);
//	return Hash::BOBHash64((uint8_t *)&item, sizeof(data_type), seed);
}
*/
static const count_type COUNT[2] = {1, -1};

class WavingSketch2 {
public:
	struct Bucket {
		data_type items[slot_num];
		count_type counters[slot_num];
		count_type incast;

		void Insert(const data_type item) {
			uint32_t choice = hash(item, 17) & 1;
			count_type min_num = INT_MAX;
			uint32_t min_pos = -1;

			for (uint32_t i = 0; i < slot_num; ++i) {
				if (counters[i] == 0) {
					items[i] = item;
					counters[i] = -1;
					return;
				}
				else if (items[i] == item) {
					if (counters[i] < 0)
						counters[i]--;
					else {
						counters[i]++;
						incast += COUNT[choice];
					}
					return;
				}

				count_type counter_val = std::abs(counters[i]);
				if (counter_val < min_num) {
					min_num = counter_val;
					min_pos = i;
				}
			}

			if (incast * COUNT[choice] >= int(min_num * factor)) {
				if (counters[min_pos] < 0) {
					uint32_t min_choice = hash(items[min_pos], 17) & 1;
					incast -= COUNT[min_choice] * counters[min_pos];
				}
				items[min_pos] = item;
				counters[min_pos] = min_num + 1;
			}
			incast += COUNT[choice];
		}

		count_type Query(const data_type item) {
	//		uint32_t choice = hash(item, 17) & 1;

			for (uint32_t i = 0; i < slot_num; ++i) {
				if (items[i] == item) {
					return std::abs(counters[i]);
				}
			}

			return 0; // incast * COUNT[choice];
		}

		uint32_t hash(data_type item, uint32_t seed = 0){
	        return BOBHash32(1000).run((const char*)&item, 4);
	    }


	};

	WavingSketch2(int total_memory, int top_k)
	{
	    K = top_k;
	    heap_memory = sizeof(heap_node) * top_k;
	    bucket_memory = total_memory - heap_memory;
	    BUCKET_NUM = floor(bucket_memory/sizeof(Bucket));
	    buckets = new Bucket[BUCKET_NUM];
	    heap = new heap_node[K];
	    Clear();

	}
/*
	WavingSketch(uint32_t _BUCKET_NUM) {
		buckets = new Bucket[BUCKET_NUM];
		memset(buckets, 0, BUCKET_NUM * sizeof(Bucket));
	}
	*/
	~WavingSketch2() {
	    delete[] buckets;
	     delete[] heap;
	 }

	void Insert(const data_type item) {
		uint32_t bucket_pos = hash(item) % BUCKET_NUM;
		buckets[bucket_pos].Insert(item);
		count_type value;
		value = Query(item);

		int find_key_index = INT_MAX;
        for(int i=0;i<K;i++){
            if(heap[i].key == item){
                find_key_index = i;
                break;
            }
        }

        if(find_key_index !=INT_MAX){
            //pkt held in heap
            heap[find_key_index].count = value;
            MINHEAPIFY(find_key_index);
        }else if(value > heap[0].count){
            heap[0].key = item;
            heap[0].count = value;
            MINHEAPIFY(0);
        }
	}

	count_type Query(const data_type item) {
		uint32_t bucket_pos = hash(item) % BUCKET_NUM;
		return buckets[bucket_pos].Query(item);
	}
	uint32_t hash(data_type item, uint32_t seed = 0){
	    return Hash->run((const char*)&item, 4);
	}
	 void Print_basic_info()
    {
        cout<<"WavingSketch"<<endl<<"heap memory: "<<heap_memory<<"B"<<endl;
        cout<<"\t bucket number: "<<BUCKET_NUM<<endl;
        cout<<"\t memory: "<< BUCKET_NUM*sizeof(Bucket)*8/1024/8.0<<"KB"<<endl;
    }
    map<uint32_t,int> query_topk()
    {
        map<uint32_t,int> result;
        cout<<"result: "<<endl;
        uint32_t key;
        for(int i = 0;i<K;i++){
            key = heap[0].key;
            cout<<"key: "<<((key&0xff000000)>>24)<<"."<<((key&0x00ff0000)>>16)<<"."<<((key&0x0000ff00)>>8)<<"."<<(key&0x000000ff)<<"\t";
            cout<<"value: "<<heap[0].count<<endl;
            result.insert(make_pair(heap[0].key,heap[0].count));
            heap[0].key = 0;
            heap[0].count = 1<<30;
            MINHEAPIFY(0);
        }
        return result;
    }

private:
	Bucket *buckets;
	uint32_t BUCKET_NUM;
	BOBHash32 *Hash = new BOBHash32(750);
	int K;
	int heap_memory;
	int bucket_memory;
	struct heap_node{
	    uint32_t key;
	    int count;
	};
	heap_node *heap;

	void Clear()
	{
	    for(int j = 0;j<K;j++){
	        heap[j].key = 0;
	        heap[j].count = 0;
	    }
	}
/*	inline uint32_t hash(data_type item, uint32_t seed = 0){
	    return Hash->run((const char*)&item, 4);
	}
*/
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

};







#endif
