#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>
using namespace std;
void initialize(vector<vector<int>>& matrix,int m,int n) {
    for (int i=0;i<m;i++){
        for (int j=0;j<n;j++) {
            matrix[i][j]=rand()%1000;
        }
    }
}    
auto seqadd(int m,int n,vector<vector<int>> a,vector<vector<int>> b,vector<vector<int>> c){
    auto s= chrono::high_resolution_clock::now();
    for (int i=0;i<m;i++){
        for (int j=0;j<n;j++){
            c[i][j]=a[i][j]+b[i][j];
        }
    }
    auto e=chrono::high_resolution_clock::now();
    chrono::duration<double> time=e-s; 
    return time;
}
auto paradd(int m,int n,vector<vector<int>> a,vector<vector<int>> b,vector<vector<int>> c){
    auto s= chrono::high_resolution_clock::now();
    omp_set_nested(true);
    #pragma omp parallel for collapse(2)
    for (int i=0;i<m;i++){
        for (int j=0;j<n;j++){
            c[i][j]=a[i][j]+b[i][j];
        }
    }
    auto e=chrono::high_resolution_clock::now();
    chrono::duration<double> time=e-s; 
    return time;
}
int main(){
    //VARYING NUMBER OF THREADS
    int m,n;
    cout<<"Enter the number of rows and columns of matrices: "<<endl;
    cin>>m>>n; 
    vector<vector<int>> a(m,vector<int>(n)),b(m,vector<int>(n)),c(m,vector<int>(n));
    initialize(a,m,n);
    initialize(b,m,n);
    cout<<"Number of Threads  Sequential Time\t\tParallel Time"<<endl;
    for (int num_threads=1;num_threads<=4;num_threads++) {
        omp_set_num_threads(num_threads);
        auto seqtime=seqadd(m,n,a,b,c);
        auto partime=paradd(m,n,a,b,c);
        cout<<num_threads<<"\t\t"<<seqtime.count()<<"\t\t\t"<<partime.count()<<endl;
    }
    //VARYING MATRIX SIZES
    cout<<"Matrix Size\tSequential Time\t\tParallel Time"<<endl;
    for (int size=100;size<=1000;size+=100){
        vector<vector<int>> a(size,vector<int>(size)),b(size,vector<int>(size)),c(size,vector<int>(size));
        initialize(a,size,size);
        initialize(a,size,size);
        auto seqtime=seqadd(size,size,a,b,c);
        auto partime=paradd(size,size,a,b,c);
        cout<<size<<"\t\t"<<seqtime.count()<<"\t\t"<<partime.count()<<endl;
    }
    return 0;
}
     