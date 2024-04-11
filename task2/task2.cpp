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

vector<int64_t> bellman_ford(long long int Vertices, vector<vector<int>>& edges, int Start) {
    vector<int64_t> dist(Vertices,INT_MAX);
    vector<int64_t> negative_cycle;
    negative_cycle.push_back(-1);
    dist[Start]=0;
    int num_edges=edges.size();
    for (int i=1;i<=Vertices;i++){
        #pragma omp parallel for 
        for (int j=0;j<num_edges;j++){
            int source=edges[j][0];
            int destination=edges[j][1];
            int weight=edges[j][2];
            if ((dist[source]+weight)<dist[destination]){
                dist[destination]=dist[source]+weight;
            }
        }
    }
    for (int i=0;i<num_edges;i++){
        int source=edges[i][0];
        int destination=edges[i][1];
        int weight=edges[i][2];
        if ((dist[source]+weight)<dist[destination]){
            return negative_cycle;
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
    for (const auto& edge : graph) {
        vert.insert(edge[0]);
        vert.insert(edge[1]);
    }
    long long int v=vert.size();
    for (int num_threads=2;num_threads<=2048;num_threads*=2) {
        omp_set_num_threads(num_threads);
        auto time_start= chrono::high_resolution_clock::now();  
        vector<int64_t> dist=bellman_ford(v,graph,0);
        auto time_end=chrono::high_resolution_clock::now();
        chrono::duration<double> time=time_end-time_start;
        cout<<num_threads<<"\t\t"<<time.count()<<endl;

    }
    return 0;
}