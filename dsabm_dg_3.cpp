/*
Input:  1. Slice Criterion
        - Number of slice criterions, t (positive integer)
        - Slice criterion for each iteration over t, s. s belong to V
Output: 1. A list of vertex with their index as prefix
        2. Dependency Matrix (|V| x |V|), with each non-zero entry corresponding an edge_type
        - 0 as an entry to a cell in the dependency matrix represents no edge
        3. Dynamic slice as computed using Dynamic Slicing algorithm for Agent Behavior Model (DSABM) corresponding each slice criterion
Author: Jibesh Kumar Panda
        - Version 1.3
        - Date: 14.12.2024
        - Version Desc: The ABDG discussed in the report has been provided as fixed input for ABDG
*/

#include <iostream>
#include <iomanip>
#include <unordered_set>
#include <array>
#include <string>
using namespace std;

class ABDG{
        // Number of Vertices (For initialization of Graph Representation)
        int n = 33;
        // Set of Vertices
        string V[33] = {"S0", "R1", "A1", "R2", "A2", "R3", "R4", "A3", "P1", "A4", "R5", "A5", "A6", "R6", "A7", "R7", "A8", "R8", "A9", "R9", "A10", "P2", "J1", "R10", "A11", "P3", "R11", "A12", "A13", "R12", "A14", "P4", "A15"};
        // Number of rule nodes, activity nodes, and procedure nodes
        int r, a, p;
        // Dependency Matrix
        int D[33][33] = {
            {0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0},
            {0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0},
            {0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
        };
        // To be computed internally
        void update_r_a_p();
        // Method to sort the nodes
        void sort_V();
        // Method to map a node to index
        int index(const string&);
        // Method to compute Dynamic Slice (Implementation of DSABM)
        unordered_set<int> DSABM(const string& s);
    public:
        // Constructor
        ABDG();
        // Destructor
        ~ABDG(){}
        // Method to show all vertices
        void show_vertices(){
            cout<<"Vertices of the ABDG are: "<<endl;
            for(int i = 0; i < n - 1; i++)
                cout<<index(V[i])<<":"<<V[i]<<", ";
            cout<<index(V[n - 1])<<":"<<V[n - 1]<<endl;
            cout<<endl;
        }
        // Method to show dependency matrix
        void show_dependency_matrix(){
            cout<<"Dependency Matrix for the ABDG: "<<endl;
            cout<<"    ";
            for(int i = 0; i < n; i++)
                cout<<setw(4)<<V[i]<<" ";
            cout<<endl;
            for(int i = 0; i < n; i++){
                cout<<setw(4)<<V[i]<<" ";
                for(int j = 0; j < n; j++){
                    cout<<setw(3)<<D[i][j]<<"  ";
                }
                cout<<endl;
            }
            cout<<endl;
        }
        // Method to display Dynamic Slice
        void display_dynamic_slice(){
            string s;
            cout<<"Enter the slice criterion (node): ";
            cin>>s;
            unordered_set<int> tempDS = DSABM(s);
            for(auto v : tempDS)
                cout<<V[v]<<" ";
            cout<<endl;
        }
};

// Method definition for index()
int ABDG::index(const string& v){
    char type = v[0];
    int id = stoi(v.substr(1)); // Extract numeric part

    switch(type){
        case 'S':
            return 0;
        case 'R':
            return id;
        case 'A':
            return r + id;
        case 'P':
            return r + a + id;
        default:
            return r + a + p + id;
    }
}

// Method definition for update_r_a_p()
void ABDG::update_r_a_p(){
    r = a = p = 0;
    // cout<<"r, a, p set to zero"<<endl;
    for(int i = 0; i < n; i++){
        // cout<<i<<"th vertex is: "<<V[i]<<endl;
        if(V[i][0] == 'R')
            r++;
        else if(V[i][0] == 'A')
            a++;
        else if(V[i][0] == 'P')
            p++;
    }
}

// Method definition for sort_V()
void ABDG::sort_V(){
    V[0] = "S0";
    int index = 1;

    for(int i = 1; i <= r; i++)
        V[index++] = "R" + to_string(i);

    for(int i = 1; i <= a; i++)
        V[index++] = "A" + to_string(i);

    for(int i = 1; i <= p; i++)
        V[index++] = "P" + to_string(i);

    for(int i = 1; index < n; i++)
        V[index++] = "J" + to_string(i);
}

// Method definition for DSABM()
unordered_set<int> ABDG::DSABM(const string& s){
    unordered_set<int> M1, M2, M3, tempDS;
    M1.insert(index(s));
    // Phase 1: Traversal along direct data transactions
    while(!M1.empty()){
        auto it = M1.begin();
        int v = *it;
        M1.erase(it);
        for(int i = 0; i < n; i++){
            if(!D[i][v])
                continue;
            if(tempDS.find(i) == tempDS.end()){
                tempDS.insert(i);
                if(D[i][v] == 6 || D[i][v] == 5 || D[i][v] == 4 || D[i][v] == 3)
                    M1.insert(i);
                else if(D[i][v] == 2)
                    M2.insert(i);
                else
                    M3.insert(i);
            }
        }
    }
    // Phase 2: Propagation along inter-agent message dependency
    while(!M2.empty()){
        auto it = M2.begin();
        int v = *it;
        M2.erase(it);
        for(int i = 0; i < n; i++){
            if(!D[i][v])
                continue;
            if(tempDS.find(i) == tempDS.end()){
                tempDS.insert(i);
                if(D[i][v] == 6 || D[i][v] == 5)
                    M2.insert(i);
                else if(D[i][v] == 4 || D[i][v] == 1)
                    M3.insert(i);
            }
        }
    }
    // Phase 3: Propagation along control dependency
    while(!M3.empty()){
        auto it = M3.begin();
        int v = *it;
        M3.erase(it);
        for(int i = 0; i < n; i++){
            if(!D[i][v])
                continue;
            if(tempDS.find(i) == tempDS.end()){
                if(D[i][v] == 6 || D[i][v] == 5 || D[i][v] == 3){
                    M3.insert(i);
                    tempDS.insert(i);
                }
            }
        }
    }
    auto it = tempDS.find(index(s));
    if(it != tempDS.end())
        tempDS.erase(it);
    return tempDS;
}

// Constructor Definition
ABDG::ABDG(){
    update_r_a_p();
    cout<<"Updated r, a and p"<<endl;
    sort_V();
    cout<<"Sorted Vertices"<<endl;
}

int main(){
    unsigned int t;
    ABDG G;
    G.show_vertices();
    G.show_dependency_matrix();
    cout<<"Enter the number of slice criterions: ";
    cin>>t;
    while(t--)
        G.display_dynamic_slice();
    return 0;
}