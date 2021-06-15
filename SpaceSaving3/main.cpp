#include "SpaceSaving.h"

#define MAX_INSERT_PACKAGE 320000

void demo_ss( int packet_num)
{
    auto ss = new SpaceSaving<128>();
    int k =4;
    vector<pair<uint32_t, uint32_t>> result(k);
    int data[] = {1,2,3,4,5,5,5,5,6,6,6,6,6,6};
    for(int i =0;i<sizeof(data)/sizeof(int);i++){
        ss->insert(data[i]);
    }
    ss->get_top_k_with_frequency(k,result);
    for(int i =0;i<k;i++)
    cout<<result[i].first<<endl;
}

int main()
{
    int packet_num = 10;
    demo_ss(packet_num);
}
