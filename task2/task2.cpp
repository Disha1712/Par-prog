#include <iostream>
#include <omp.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <set>
#include <climits>
#include <chrono>
using namespace std;
typedef long long ll;

int64_t* bellman_ford(int Vertices,int** edges, int Start,ll num_edges) {
    int64_t* dist=new int64_t[Vertices];
    int64_t* negative_cycle=new int64_t[1];
    negative_cycle[0]=-1;
    for (int i=0;i<Vertices;++i)
        dist[i]=LLONG_MAX;
    dist[Start]=0;
    #pragma omp parallel for collapse(2)
    for (int i=1;i<=Vertices;i++){
        for (ll j=0;j<num_edges;j++){
            int source=edges[j][0];
            int destination=edges[j][1];
            int weight=edges[j][2];
            if (dist[source]!=LLONG_MAX && (dist[source]+weight)<dist[destination]){
                dist[destination]=dist[source]+weight;
            }
        }
    }
    for (ll i=0;i<num_edges;i++){
        int source=edges[i][0];
        int destination=edges[i][1];
        int weight=edges[i][2];
        if (dist[source]!=LLONG_MAX && (dist[source]+weight)<dist[destination]){
            delete[] dist;
            return negative_cycle;
        }
    }
    return dist;
}

pair<int**,ll> createGraph(const string& filename) {
    ifstream inputFile(filename);
    vector<vector<int>> graph;
    string line;
    ll num_edges=0;
    while (getline(inputFile,line)) {
        int source,destination,weight;
        istringstream iss(line);
        if (!(iss >> source >> destination >> weight)) { 
            cerr << "Error reading line: " << line << endl;
            continue;
        }
        graph.push_back({source,destination,weight});
        num_edges++;
    }
    inputFile.close();
    int** edges=new int*[num_edges];
    for (int i=0;i<num_edges;i++) {
        edges[i]=new int[3];
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
    pair<int**,ll> graphAndEdge=createGraph(filename);
    int** graph=graphAndEdge.first;
    int num_edges=graphAndEdge.second;
    set <int>vert;
    for (ll i=0;i<num_edges;i++){
        vert.insert(graph[i][0]);
        vert.insert(graph[i][1]);
    }
    int v=vert.size();
    for (int num_threads=2;num_threads<=2048;num_threads*=2) {
        omp_set_num_threads(num_threads);
        auto time_start= chrono::high_resolution_clock::now(); 
        int64_t* dist=bellman_ford(v,graph,0,num_edges);
        auto time_end=chrono::high_resolution_clock::now();
        chrono::duration<double> time=time_end-time_start;
        cout<<num_threads<<"\t\t"<<time.count()<<endl;
        delete [] dist;
   }
    for (ll i = 0; i < num_edges; ++i)
        delete[] graph[i];
    delete[] graph;
    return 0;
}