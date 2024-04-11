#include <iostream>
#include <omp.h>
#include <fstream>
#include <vector>
#include <sstream>
#include <set>
#include <climits>
#include <chrono>
using namespace std;
#define MOD 1000000007

vector<int> bellman_ford(long long int V, vector<vector<int>>& edges, int S) {
    vector<int> dist(V,INT_MAX);
    vector<int> neg;
    neg.push_back(-1);
    dist[S]=0;
    int e=edges.size();
    for (int i=1;i<=V;i++){
        #pragma omp parallel for 
        for (int j=0;j<e;j++){
            int u=edges[j][0];
            int v=edges[j][1];
            int d=edges[j][2];
            if (dist[u]!=INT_MAX && (((dist[u]+(d%MOD))%MOD)<dist[v]%MOD)){
                #pragma omp critical
                dist[v]=(dist[u]+(d%MOD))%MOD;
            }
        }
        if ((i%1000)==0)    cout<<i<<endl;
     }
    for (int i=0;i<e;i++){
        int u=edges[i][0];
        int v=edges[i][1];
        int d=edges[i][2];
        if (dist[u]!=INT_MAX && (dist[u]+(d%MOD)%MOD)<dist[v]){
            return neg;
        }
    }
    return dist;
}

vector<vector<int>> createGraph(const string& filename) {
    ifstream inputFile(filename);
    vector<vector<int>> graph;
    string line;
    while (getline(inputFile, line)) {
        int source, destination, weight;
        istringstream iss(line);
        if (!(iss >> source >> destination >> weight)) { 
            cerr << "Error reading line: " << line << endl;
            continue;
        }
        graph.push_back({source, destination, weight});
    }
    inputFile.close();
    return graph;
}

int main(int argc, char *argv[]) {
    
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <input_filename>" << endl;
        return 1;
    }
    string filename = argv[1];
    vector<vector<int>> graph = createGraph(filename);
    set <int> vert;
    cout<<"hey"<<endl;
    for (const auto& edge : graph) {
        vert.insert(edge[0]);
        vert.insert(edge[1]);
    }
    long long int v=vert.size();
    cout<<"hey"<<endl;
    for (int num_threads=2;num_threads<=2048;num_threads*=2) {
        omp_set_num_threads(num_threads);
        auto s= chrono::high_resolution_clock::now();
        cout<<"hey"<<endl;
        vector<int> dist=bellman_ford(v,graph,0);
        auto e=chrono::high_resolution_clock::now();
        chrono::duration<double> time=e-s;
        cout<<num_threads<<"\t\t"<<time.count()<<endl;

    }
    return 0;
}