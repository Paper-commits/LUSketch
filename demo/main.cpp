#include "../common/traceRead.h"
#include "../common/param.h"
#include "../CMH/cmh.h"
#include "../luSketch/lusketch2.h"
#include "../HeavyKeeper/heavykeeper.h"
#include "../WavingSketch/wavingsketch2.h"
#include "../SpaceSaving3/SpaceSaving.h"

#include <algorithm>

using namespace std;
uint32_t insert_data[MAX_PACKET];
unordered_map<uint32_t,int> benchmark_data;

int main()
{
    int packet_num;
    /********************/
    /**case 1** fix data*/

  char path[100] = "../data/sample.txt";

  /**case 2: as a function for call**/
  //  char path[100];
   // cout<<"input data path:(for example: ../data/sample.txt"<<endl;
  //  cin >> path;
/***********************/
    packet_num = TraceRead(path, insert_data, benchmark_data);

    float MEM,K;
    cout<<"input memory(KB) and K (for example: 20 100)"<<endl;
    cin >> MEM >>K;
    cout<<"Memory Size = "<<MEM <<"KB"<<endl<<"Find Top "<<K<<endl<<endl;
    cout<<"********************************"<<endl;
    int MEM_Byte = MEM * 1024;

    timespec start_time, end_time;
    long long cmh_timediff,lusketch2_timediff,heavykeeper_timediff,wavingsketch_timediff,spacesaving_timediff;
    double PRECISION = 0;
    double insert_throughout;
    map<uint32_t,int> cmh_topk;
    map<uint32_t,int> lusketch_topk;
    map<uint32_t,int> lusketch2_topk;
    map<uint32_t,int> heavykeeper_topk;
    map<uint32_t,int> wavingsketch_topk;
 //   map<uint32_t,int> spacesaving_topk;

    vector<pair<uint32_t,int>> TopKRes;
 //   unordered_map<uint32_t,int> benchmark_data2 = benchmark_data;
    TopKRes= topKFrequent(benchmark_data,K);
    uint32_t tmp_key;
    for(int i = 0;i<K;i++){
        tmp_key = TopKRes[i].second;
        cout<<"key: "<<((tmp_key&0xff000000)>>24)<<"."<<((tmp_key&0x00ff0000)>>16)<<"."<<((tmp_key&0x0000ff00)>>8)<<"."<<(tmp_key&0x000000ff)<<"\t";
        cout<<"value: "<<TopKRes[i].first<<endl;
    }



 /****************************CMH*******************************/
    cout<<"****************CMH****************"<<endl;
    CMH *cmh = new CMH(MEM_Byte,K);
    cmh->print_basic_info();
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for(int i =0;i<packet_num;i++){
        cmh->insert(insert_data[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    cmh_timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);

    cmh_topk = cmh->query_topk();
 /****************************LUSKETCH*******************************/
    cout<<"***************LUSKETCH2*************"<<endl;
    LUSketch2 *lusketch2 = new LUSketch2(MEM_Byte,K);
    lusketch2->Print_basic_info();
    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i = 0;i<packet_num;i++){
        lusketch2->Insert(insert_data[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    lusketch2_timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
 //   insert_throughout = (double)1000.0*packet_num / timediff;
 //   cout<<"throughput of LUSketch2 (insert): "<<insert_throughout<<endl;
    lusketch2_topk = lusketch2->query_topk();

  /****************************HEAVYKEEPER*******************************/
    cout<<"********************HeavyKeeper********"<<endl;
    HeavyKeeper *heavykeeper = new HeavyKeeper(MEM_Byte,K);
    heavykeeper->Print_basic_info();
    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i =0;i<packet_num;i++){
        heavykeeper->Insert(insert_data[i]);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    heavykeeper_timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
 //   insert_throughout = (double)1000.0*packet_num / timediff;
//    cout<<"throughput of LUSketch2 (insert): "<<insert_throughout<<endl;
    heavykeeper_topk = heavykeeper->query_topk();

/****************************WAVINGSKETCH*******************************/
    cout<<"***************WavingSketch*************"<<endl;
    WavingSketch2 *wavingsketch2 = new WavingSketch2(MEM_Byte,K);
    wavingsketch2->Print_basic_info();
    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i = 0;i<packet_num;i++){
        wavingsketch2->Insert(insert_data[i]);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    wavingsketch_timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
//    insert_throughout = (double)1000.0*packet_num / timediff;
//    cout<<"throughput of WavingSketch (insert): "<<insert_throughout<<endl;
    wavingsketch_topk = wavingsketch2->query_topk();

/****************SPACESAVING**************************/
    cout<<"****************SpaceSaving***********"<<endl;
    auto spacesaving = new SpaceSaving<601>();
    clock_gettime(CLOCK_MONOTONIC,&start_time);
    for(int i = 0;i<packet_num;i++){
        spacesaving->insert(insert_data[i]);
    }
    clock_gettime(CLOCK_MONOTONIC,&end_time);
    spacesaving_timediff = (long long)(end_time.tv_sec - start_time.tv_sec) * 1000000000LL + (end_time.tv_nsec - start_time.tv_nsec);
    insert_throughout = (double)1000.0*packet_num / spacesaving_timediff;
    cout<<"throughput of SpaceSaving (insert): "<<insert_throughout<<endl;
  //  vector<pair<uint32_t,uint32_t>> spacesaving_topk;
    map<uint32_t,int> spacesaving_topk;
    spacesaving_topk = spacesaving->query_topk(int(K));
 //   spacesaving->get_top_k_with_frequency(int(K),spacesaving_topk);

    /*************************************************************************/
    /******************RESULT(precision, ARE, AAE)******************/
    ofstream outFile;
    outFile.open("result.csv",ios::app);

    cout<<"********************************RESULT******************"<<endl;
    /****CMH******/
    cout<<endl<<"RESULT of CMH"<<endl;
    uint32_t key_t;
    int count_t = 0;

    for(auto itr:TopKRes)
    {
        key_t = itr.second;
        if( cmh_topk.find(key_t) != cmh_topk.end()){
            //if the topk flow also held in cmh_topk;
            count_t ++;

        }
    }

    PRECISION = count_t/K;
//    cout<<"precision: "<< PRECISION<<"\t AAE: "<<log(AAE)<<"\t ARE: "<<log(ARE)<<endl;
    cout<<"precision: "<< PRECISION<<endl;

    insert_throughout = (double)1000.0*packet_num / cmh_timediff;
    cout<<"throughput of CMH (insert): "<<insert_throughout<<endl;
    outFile<<path<<","<<MEM<<","<<K<<","<<"CMH"<<","<<insert_throughout <<","<<PRECISION<<endl;

    /****LUSKETCH2******/
    cout<<endl<<"RESULT of LUSKETCH2"<<endl;
    count_t = 0;
    for(auto itr:TopKRes)
    {
        key_t = itr.second;
        if( lusketch2_topk.find(key_t) != lusketch2_topk.end()){
            count_t ++;

        }
    }

    PRECISION = count_t/K;
    //cout<<"precision: "<< PRECISION<<"\t AAE: "<<log(AAE)<<"\t ARE: "<<log(ARE)<<endl;
    cout<<"precision: "<< PRECISION<<endl;

    insert_throughout = (double)1000.0*packet_num / lusketch2_timediff;
    cout<<"throughput of LUSketch2 (insert): "<<insert_throughout<<endl;
    outFile<<path<<","<<MEM<<","<<K<<","<<"LUSKETCH2"<<","<<insert_throughout <<","<<PRECISION<<endl;

    /****HEAVYKEEPER**/
    cout<<endl<<"RESULT of HEAVYKEEPER"<<endl;
    count_t = 0;
    for(auto itr:TopKRes)
    {
        key_t = itr.second;
        if( heavykeeper_topk.find(key_t) != heavykeeper_topk.end()){
            count_t ++;
       }
    }
    PRECISION = count_t/K;
  //  cout<<"precision: "<< PRECISION<<"\t AAE: "<<log(AAE)<<"\t ARE: "<<log(ARE)<<endl;
    cout<<"precision: "<< PRECISION<<endl;
    insert_throughout = (double)1000.0*packet_num / heavykeeper_timediff;
    cout<<"throughput of HeavyKeeper (insert): "<<insert_throughout<<endl;
    outFile<<path<<","<<MEM<<","<<K<<","<<"HEAVYKEEPER"<<","<<insert_throughout <<","<<PRECISION<<endl;

    /****WEAVINGSKETCH******/
    cout<<endl<<"RESULT of WAVINGSKETCH"<<endl;
    count_t = 0;
    for(auto itr:TopKRes)
    {
        key_t = itr.second;
        if( wavingsketch_topk.find(key_t) != wavingsketch_topk.end()){
            count_t ++;
        }
    }
    PRECISION = count_t/K;
    cout<<"precision: "<< PRECISION<<endl;
    insert_throughout = (double)1000.0*packet_num / wavingsketch_timediff;
    cout<<"throughput of WavingSketch (insert): "<<insert_throughout<<endl;
    outFile<<path<<","<<MEM<<","<<K<<","<<"WAVINGSKETCH"<<","<<insert_throughout <<","<<PRECISION<<endl;

    /*****SPACESAVING*****/
    cout<<endl<<"RESULT of SPACESAVING"<<endl;
    count_t = 0;
    for( auto itr:TopKRes)
    {
        key_t = itr.second;
        if(spacesaving_topk.find(key_t) != spacesaving_topk.end()){
            count_t ++;
         }
    }

    PRECISION = count_t/K;
    cout<<"precision: "<< PRECISION<<endl;
    insert_throughout = (double)1000.0*packet_num / spacesaving_timediff;
    cout<<"throughput of SpaceSaving (insert): "<<insert_throughout<<endl;
    outFile<<path<<","<<MEM<<","<<K<<","<<"SPACESAVING"<<","<<insert_throughout <<","<<PRECISION<<endl;

}
