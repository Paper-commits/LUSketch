#ifndef _CMSKETCH_H
#define _CMSKETCH_H

#include "../common/param.h"
#include "../common/BOBHash32.h"
using namespace std;
#define single_counter_size 4
#define d 2

class CMSketch
{
private:
    int w = 0;// w is the number of counter
    int row_size = 0;
    int key_len = 4;//BOBHash require the defined length of return key
    BOBHash32 *hash[d];
    int *counter; // counter array
//    int d =2//adjust according to your data

public:
    CMSketch(int memory_in_byte)
    {
        w = floor(memory_in_byte / single_counter_size);
        row_size = floor(w/d);
        counter = new int[w];
        for(int i = 0; i<d; i++)
        {
            hash[i] = new BOBHash32(i+750);
        }
        clear();
    }
    ~CMSketch()
    {
        for(int i =0;i<d;i++)
            delete hash[i];

        delete []counter;
    }

    void print_basic_info()
    {
        cout<<"CM Sketch"<<endl<<"\t counter number: "<<w<<endl<<"\t row size "<<row_size<<endl;
        cout<<"\t memory: "<< w*single_counter_size*8/1024/8.0<<"KB"<<endl;

    }

    void clear()
    {
        for(int i = 0;i<w;i++)
            counter[i] = 0;
    }


    void insert(uint32_t key, int f =1)
    {
        int index;
        for(int i =0;i<d;i++)
        {
            index = (hash[i]->run((const char*)&key,key_len))%row_size;
            counter[i*row_size + index] += f;
        }
        /*for test*/
        /*
        cout<<"insert: \t key: \t"<<key<<"\t index: \t"<< index<<endl;
        for(int i =0;i<16;i++){
            cout<<counter[i]<<endl;
        }
        */
    }

    int query(uint32_t key)
    {
        int index;
        int query_res = 1<<30;
        for(int i =0;i<d;i++)
        {
            index = (hash[i]->run((const char*)&key,key_len))%row_size;
            query_res = min(query_res,counter[i*row_size+index]);
        }
        return query_res;
    }

};
#endif
