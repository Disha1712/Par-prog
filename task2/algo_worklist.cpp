    vector<ll> worklist;
    worklist.push_back(Start);  
    while (!worklist.empty()) {
        vector<ll> next_worklist;
        #pragma omp parallel
        {
            vector<ll> local_next_worklist;
            #pragma omp for schedule(dynamic,64) nowait
            for (size_t i=0;i<worklist.size();++i){
                ll u=worklist[i];
                for (ll j=0;j<num_edges;++j){
                    if (edges[j][0]==u){
                        ll source=edges[j][0];
                        ll destination=edges[j][1];
                        ll weight=edges[j][2];
                        bool updated=false;
                        ll new_distance =(dist[source]+(weight%mod))%mod;
                        #pragma omp critical
                        {
                            if (dist[source]!=LLONG_MAX && new_distance<dist[destination]){
                                dist[destination]=new_distance;
                                updated=true;
                            }
                        }
                        if (updated) {
                            local_next_worklist.push_back(destination);
                        }
                    }
                }
            }
            #pragma omp critical
            {
                next_worklist.insert(next_worklist.end(),local_next_worklist.begin(),local_next_worklist.end());
            }
        }
        // Remove duplicates from next_worklist
        sort(next_worklist.begin(),next_worklist.end());
        next_worklist.erase(unique(next_worklist.begin(),next_worklist.end()),next_worklist.end()); 
        worklist=next_worklist;
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
    ll* dist=bellman_ford(v,graph,0,num_edges);
    auto time_end=chrono::high_resolution_clock::now();
    chrono::duration<double> time=time_end-time_start;
    cout << "Execution time for serial : " << time.count() << " seconds" << endl;
    delete[] dist;
    omp_set_num_threads(16);
    time_start= chrono::high_resolution_clock::now(); 
    dist=bellman_ford(v,graph,0,num_edges);
    time_end=chrono::high_resolution_clock::now();
    time=time_end-time_start;
    cout << "Execution time for 16 threads : " << time.count() << " seconds" << endl;
    delete[] dist;
    for (ll i = 0; i < num_edges; ++i)
        delete[] graph[i];
    delete[] graph;
    return 0;
}
