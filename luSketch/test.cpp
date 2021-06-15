#include "lusketch.h"
#define MAX_INSERT_PACKAGE 32000000
#include <algorithm>

#include "../common/traceRead.h"
#include "../common/param.h"

uint32_t insert_data[MAX_INSERT_PACKAGE];
unordered_map<uint32_t,int> benchmark_data;

int main()
{
/*
    int MEM_Byte = 10*1024;
    int K = 10;
    auto lusketch = new LUSketch(MEM_Byte,K);

    int data[MAX_INSERT_PACKAGE];
    int t = 0;
    for(int i =0;i<50;i++){
        for(int j = 0;j<i;j++)
         data[t++] = i;
    }

    random_shuffle(data,data+t);
    for(int i =0;i<t;i++)
        lusketch->Insert(data[i]);
*/
//    char path[100];
 //   cout<<"input data path:(for example: ../data/wide_1000000_s10.txt"<<endl;
 //   cin >> path;
    char path[100] = "../data/wide_1000.txt";
    int packet_num;
    packet_num = TraceRead(path,insert_data,benchmark_data);
    float MEM,K;
    cout<<"input memory(KB) and K (for example: 20 100)"<<endl;
    cin >> MEM >>K;
    cout<<"Memory Size = "<<MEM <<"KB"<<endl<<"Find Top "<<K<<endl<<endl;
    cout<<"********************************"<<endl;
    int MEM_Byte = MEM * 1024;


    auto lusketch = new LUSketch(MEM_Byte,K);

    for(int i = 0;i<packet_num;i++)
        lusketch->Insert(insert_data[i]);
    lusketch->Print_basic_info();
    lusketch->query_topk();
    lusketch->query_heapnum();
}
