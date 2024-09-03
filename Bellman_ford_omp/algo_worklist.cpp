#include <iostream>
#include <omp.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <set>
#include <climits>
#include <atomic>
#include <chrono>
#include <algorithm>
typedef long long int ll;
using namespace std;
#define mod 1000000007

ll* bellman_ford(ll Vertices,ll** edges, int Start,ll num_edges){
    ll* dist=new ll[Vertices+1];
    ll* negative_cycle=new ll[1];
    negative_cycle[0]=-1;
    #pragma omp parallel for
    for (int i=0;i<=Vertices;++i)
        dist[i]=LLONG_MAX;
    dist[Start]=0;   
    vector<ll> worklist;
    worklist.push_back(Start);  
    while (!worklist.empty()) {
        vector<atomic<bool>> next_worklist_flags(Vertices);
        vector<ll> local_next_worklist;
        #pragma omp parallel
        {
            #pragma omp for schedule(dynamic,64) nowait
            for (size_t i=0;i<worklist.size();++i){
                ll u=worklist[i];
                for (ll j=0;j<num_edges;++j){
                    if (edges[j][0]==u){
                        ll source=edges[j][0];
                        ll destination=edges[j][1];
                        ll weight=edges[j][2];
                        ll new_distance=(dist[source]+(weight%mod))%mod;
                        
                        if (dist[source]!=LLONG_MAX) {
                            ll current_distance;
                            bool updated=false;
                            #pragma omp atomic read
                            current_distance=dist[destination];
                            if (new_distance<current_distance) {
                                #pragma omp atomic write
                                dist[destination]=new_distance;
                                updated=true;
                            }
                            if (updated){
                                local_next_worklist.push_back(destination);
                             }
                        }
                    }
                }
            }
        }
        for (ll node:local_next_worklist) {
                next_worklist_flags[node]=true;
        }
        worklist.clear();
        for (ll i=0;i<Vertices;++i){
            if (next_worklist_flags[i].load(memory_order_relaxed)){
                worklist.push_back(i);
            }
        }
        sort(worklist.begin(),worklist.end());
        worklist.erase(unique(worklist.begin(),worklist.end()),worklist.end());
    }
    for (ll i=0;i<num_edges;i++){
        ll source=edges[i][0];
        ll destination=edges[i][1];
        ll weight=edges[i][2];
        if (dist[source]!=LLONG_MAX && ((dist[source]+(weight%mod))%mod)<dist[destination]){
            delete[] dist;
            return negative_cycle;
        }
    }
    return dist;
}

pair<ll**,ll> createGraph(const string& filename) {
    ifstream inputFile(filename);
    vector<vector<ll>> graph;
    string line;
    ll num_edges=0;
    while (getline(inputFile,line)) {
        ll source,destination,weight;
        istringstream iss(line);
        if (!(iss >> source >> destination >> weight)) { 
            cerr << "Error reading line: " << line << endl;
            continue;
        }
        graph.push_back({source,destination,weight});
        num_edges++;
    }
    inputFile.close();
    ll** edges=new ll*[num_edges];
    for (int i=0;i<num_edges;i++) {
        edges[i]=new ll[3];
        for (int j=0;j<3;j++) {
            edges[i][j]=graph[i][j];
        }
    }
    return make_pair(edges,num_edges);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input_filename>" << endl;
        return 1;
    }
    string filename = argv[1];
    pair<ll**,ll> graphAndEdge=createGraph(filename);
    ll** graph=graphAndEdge.first;
    ll num_edges=graphAndEdge.second;
    set <int>vert;
    for (ll i=0;i<num_edges;i++){
        vert.insert(graph[i][0]);
        vert.insert(graph[i][1]);
    }
    ll v=vert.size();

    omp_set_num_threads(1);
    auto time_start= chrono::high_resolution_clock::now(); 
    ll* dist=bellman_ford(v,graph,1,num_edges);
    auto time_end=chrono::high_resolution_clock::now();
    chrono::duration<double> time=time_end-time_start;
    cout << "Execution time for 16 threads : " << time.count() << " seconds" << endl;
    delete[] dist;
    for (ll i = 0; i < num_edges; ++i)
        delete[] graph[i];
    delete[] graph;
    return 0;
}
