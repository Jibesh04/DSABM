/*
Input: 1. Agent Behavior Dependency Graph (ABDG)
        - Number of Vertices, n
        - Vertices, V, where V[i] is either of (S0, Ri, Ai, Pi, Ji) where i is a positive integer, i = 1, 2, 3, ...
        - Number of Edges, e
        - Edges in the format: tail <space> head <space> edge_type
                - tail, head belong to V
                - edge_type belong to {1, 2, 3, 4, 5, 6}
                        - 1: Control Dependency
                        - 2: Inter-agent Message Dependency
                        - 3: Procedure Call Dependency
                        - 4: Return Dependency
                        - 5: Inter-agent Data Dependency
                        - 6: Data Dependency
        2. Slice Criterion
        - Number of slice criterions, t (positive integer)
        - Slice criterion for each iteration over t, s. s belong to V
Output: 1. A list of vertex with their index as prefix
        2. Incedent-Dependency List of |V| vector entries, each representing a vertex Vi
        - Each vector contains the tail, u of incident edges u -> Vi (of which Vi is a head) and their nature (an integer from [1-6] as per the above-mentioned notation)
        3. Dynamic slice as computed using Dynamic Slicing algorithm for Agent Behavior Model (DSABM) corresponding each slice criterion
Author: Jibesh Kumar Panda
        - Version 1.2
        - Date: 11.01.2025
        - Version Desc: Input from File
*/

#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <array>
#include <string>
// #include <cstdlib>
#include <conio.h>
using namespace std;

class ABDG{
        // Number of Vertices (For initialization of Graph Representation)
        int n;
        // Set of Vertices
        string *V;
        // Number of Edges
        int e;
        // Number of rule nodes, activity nodes, and procedure nodes
        int r, a, p;
        // Incident-Dependency List
        vector<vector<pair<string, int>>> IDL;
        // To be computed internally
        void update_r_a_p();
        // Method to sort the nodes
        void sort_V();
        // Method to map a node to index
        int index(const string&);
        // Method to initialize dependency matrix
        void initialize_incident_dependency_list();
        // Method to add dependencies
        void add_edges();
        // Method to compute Dynamic Slice (Implementation of DSABM)
        unordered_set<int> DSABM(const string& s);
        // File Stream
        fstream fs;
    public:
        // Constructor
        ABDG();
        // Destructor
        ~ABDG();
        // Method to show all vertices
        void show_vertices(){
            for(int i = 0; i < n - 1; i++)
                cout<<index(V[i])<<":"<<V[i]<<", ";
            cout<<index(V[n - 1])<<":"<<V[n - 1]<<endl;
        }
        // Method to show dependency matrix
        void show_incident_dependency_list(){
            for(int i = 0; i < n; i++){
                cout<<V[i]<<": ";
                for(auto p : IDL[i])
                    cout<<"- "<<p.first<<"|"<<p.second<<" ";
                cout<<endl;
            }
        }
        // Method to display Dynamic Slice
        void display_dynamic_slice(){
            string s;
            cout<<"Enter the slice criterion (node): ";
            cin>>s;
            unordered_set<int> tempDS = DSABM(s);
            if(tempDS.size())
                cout<<"Dynamic Slice Set corresponding criterion "<<s<<" is:"<<endl;
            for(auto v : tempDS)
                cout<<V[v]<<" ";
            cout<<endl;
        }
};

// Method definition for update_r_a_p()
void ABDG::update_r_a_p(){
    r = a = p = 0;
    for(int i = 0; i < n; i++){
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

// Method definition for initialize_dependency_matrix()
void ABDG::initialize_incident_dependency_list(){
    vector<pair<string, int>> temp;
    for(int i = 0; i < n; i++)
        IDL.push_back(temp);
    add_edges();
}

// Method definition for add_edges()
void ABDG::add_edges(){
    fs>>e;
    string tail, head;
    int edge_type;
    for(int i = 0; i < e; i++){
        fs>>tail>>head>>edge_type;
        IDL[index(head)].push_back({tail, edge_type});
    }
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
        for(auto p : IDL[v]){
            int i = index(p.first);
            if(tempDS.find(i) == tempDS.end()){
                tempDS.insert(i);
                if(p.second == 6 || p.second == 5 || p.second == 4 || p.second == 3)
                    M1.insert(i);
                else if(p.second == 2)
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
        for(auto p : IDL[v]){
            int i = index(p.first);
            if(tempDS.find(i) == tempDS.end()){
                tempDS.insert(i);
                if(p.second == 6 || p.second == 5)
                    M2.insert(i);
                else if(p.second == 4 || p.second == 1)
                    M3.insert(i);
            }
        }
    }
    // Phase 3: Propagation along control dependency
    while(!M3.empty()){
        auto it = M3.begin();
        int v = *it;
        M3.erase(it);
        for(auto p : IDL[v]){
            int i = index(p.first);
            if(tempDS.find(i) == tempDS.end()){
                if(p.second == 6 || p.second == 5 || p.second == 3){
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
    fs.open("input_file.txt", ios::in);
    if(!fs.is_open()){
        cerr << "Error: Could not open input file "<<endl;
        exit(1);
    }
    fs>>n;
    V = new string[n];
    for(int i = 0; i < n; i++){
        fs>>V[i];
    }
    update_r_a_p();
    sort_V();
    initialize_incident_dependency_list();
}

// Destructor Definition
ABDG::~ABDG(){
    for(int i = 0; i < n; i++)
        delete &IDL[i];
    IDL.clear();
    delete[] V;
}

int main(){
    unsigned int t;
    ABDG G;
    // G.show_vertices();
    // G.show_incident_dependency_list();
    // cout<<"Enter the number of slice criterions: ";
    // cin>>t;
    // while(t--)
    G.display_dynamic_slice();
    // system("pause");
    _getch();
    return 0;
}