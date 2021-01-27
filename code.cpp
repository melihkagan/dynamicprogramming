/* Melih Kaðan Özçelik
25.05.2020 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>


using namespace std;
int freq_dis(vector<int> , vector<int> , int , int );

class test{
    int ID;
    int bugs, runtime;
    vector< vector<int> > freq_profile;
    vector<int> ordered;
protected:
    int coverage(int i);
public:
    void push_profile( vector<int> &vec_in){ freq_profile.push_back(vec_in) ; }
    test( int ID_in,int  b_in, int r_in ) { ID = ID_in; bugs = b_in; runtime = r_in;}
    int get_ID(){return ID; }
    int get_bugs(){return bugs;}
    int get_runtime(){return runtime;}
    void print_profile();
    void ordered_seq(int k);
};
int test::coverage(int i){ // it gives number of statements executed
    int total=0;
    for(int k=0; k < freq_profile[i].size(); k++){
        if ( freq_profile[i][k] != 0 ){
            total++;
        }
    }
    return total;
}
void test::ordered_seq(int k){ //convert freq profiles to
    vector< pair<int,int> > vec;  //Ordered sequence of statement execution frequencies
    for(int i=1; i<= freq_profile[k].size(); i++){
        vec.push_back( make_pair(i,freq_profile[k][i-1]));
    }
    for(int i=0; i<vec.size(); i++){
        for( int j=i; j>0; j--){
            if( vec[j].second < vec[j-1].second ){
                swap(vec[j],vec[j-1]);
            }
        }
    }
    freq_profile[k].clear();
    for(int i=0; i<vec.size(); i++ ){
        freq_profile[k].push_back(vec[i].first);
    }
}
void test::print_profile(){ //it re-orders Ordered sequence of statement execution frequencies
    cout<<"TS"<<ID<<": ";   // using distance function and prints solution for selected test suite
    for( int i =0; i< freq_profile.size(); i++){
        ordered.push_back(i+1);
    }
    int max_covered = 0;
    int index;
    for( int i =0; i< freq_profile.size(); i++){
        int a = coverage(i);
        if( a > max_covered){
            max_covered = a;
            index = i;
        }
    }
    swap( ordered[0],ordered[index] ); //max covered test case founded
    swap( freq_profile[0], freq_profile[index]);
    for( int i =0; i< freq_profile.size(); i++){
        ordered_seq(i); //convert freq profiles to ordered statement sequence.
    }
    int x,y;
    for(int i=1; i<ordered.size(); i++ ){
        for( int j=i; j>1; j--){
            x = freq_dis(freq_profile[0],freq_profile[j],freq_profile[0].size(), freq_profile[j].size() );
            y = freq_dis(freq_profile[0],freq_profile[j-1],freq_profile[0].size(), freq_profile[j-1].size() );
            if ( x > y){  //compare distances to first test case ( ordered sequence)
                swap( freq_profile[j],freq_profile[j-1]);
                swap( ordered[j],ordered[j-1]);
            }
        }
    }
    for( int i=0; i<ordered.size(); i++){
        cout<<ordered[i]<<" ";
    }
    cout<<endl;
}
int freq_dis(vector<int> v1, vector<int> v2, int l1, int l2){ //to calculate differences between
    int table[l1+1][l2+1];                                    //Ordered sequence of statement execution frequencies
    for( int i=0; i<=l1; i++){
        for( int j=0; j<=l2; j++){
            if ( i == 0){ //first vector empty
                table[i][j] = j;
            }
            else if ( j== 0){ //second vector empty
                table[i][j] = i;
            }
            else if ( v1[i-1] == v2[j-1] ){ //last values same
                table[i][j] = table[i-1][j-1];
            }
            else{            //replacement cost         insertion cost           removing cost
                table[i][j] = min( 4 + table[i-1][j-1], min( 3 + table[i][j-1], 2 + table[i-1][j]) );
            }
        }
    }
    return table[l1][l2];
}
void split_to_vector(string s, vector<int> &vec ){ //to split freq sequences to integers
    stringstream ss(s);
    string mid;
    int x;
    while ( getline(ss,mid, '-') ){
        stringstream(mid)>>x;
        vec.push_back(x);
    }
}
int knapsack_algo( vector<int> R, vector<int> D, int R_lim, int n, vector<int> &result ){ //returns max runtime and indexes of test suites
    int i,j;
    int table[n+1][R_lim +1];
    for (i = 0; i <= n; i++) { //Knapsack bottum-up filling table
        for (j = 0; j <= R_lim; j++) {
            if (i == 0 || j == 0){
                table[i][j] = 0;
            }
            else if (R[i - 1] <= j){  //item i selected    j-R[i-1]->new runtime limit
                table[i][j] = max( D[i - 1] + table[i - 1][j - R[i - 1]], table[i - 1][j]);
            }
            else{
                table[i][j] = table[i - 1][j]; //item i not selected
            }
        }
    }
    int max_bug = table[n][R_lim];
    j= R_lim;
    for(i = n; max_bug>0 && i>0 ; i--){
        if ( max_bug != table[i-1][j] ){ //this means bug limit not equal to upper row with same runtime amount
            result.push_back(i-1);          // so this item included to solution
            max_bug = max_bug - D[i-1]; //pick item set new max_bug
            j= j - R[i-1];  //set new runtime limit
        }
    }
    return table[n][R_lim];
}

int main(int argc, char** argv){
    //read from file
    ifstream data_file(argv[1]);
    string temp;
    getline(data_file,temp);
    int runtime_lim ;
    stringstream ss(temp);
    while ( !ss.eof() ){
        temp = "";
        ss >> temp ;
        if ( stringstream(temp)>> runtime_lim )
            break;
    }
    getline(data_file,temp); getline(data_file,temp);
    vector<test*> list_of_tests;
    string temp2;
    int num_of_ts = 0;
    int B_D, R_T;
    vector<int> bugs, rtime;
    while ( getline(data_file,temp) ){
        stringstream line(temp);
        line>>temp2>>B_D>>R_T;
        bugs.push_back(B_D);
        rtime.push_back(R_T);
        num_of_ts++;
        test* ptr = new test(num_of_ts,B_D,R_T);
        list_of_tests.push_back(ptr);
        vector<int> freq;
        while ( !line.eof() ){
            line>>temp2;
            split_to_vector(temp2,freq);
            ptr->push_profile(freq);
            freq.clear();
        }
    }
    //printing results
    vector<int> res;
    cout<<runtime_lim<<endl;
    for(int k=0; k<bugs.size(); k++){
        cout<<bugs[k]<<"  "<<rtime[k]<<endl;
    }
    int ach = knapsack_algo(rtime,bugs, runtime_lim, bugs.size(), res);
    cout<<"Total amount of running time: "<<ach<<endl<<endl;
    for(int k= 0; k<res.size(); k++){
        list_of_tests[res[k]]->print_profile();
    }
    for(int k=0; k<list_of_tests.size(); k++ ){
        delete list_of_tests[k];
    }
    return 0 ;
}
