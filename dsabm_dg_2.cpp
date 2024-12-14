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
        2. Dependency Matrix (|V| x |V|), with each non-zero entry corresponding an edge_type
        - 0 as an entry to a cell in the dependency matrix represents no edge
        3. Dynamic slice as computed using Dynamic Slicing algorithm for Agent Behavior Model (DSABM) corresponding each slice criterion
Author: Jibesh Kumar Panda
        - Version 1.2
        - Date: 13.12.2024
        - Version Desc: No limit on number of characters per vertex
*/

#include <iostream>
#include <iomanip>
#include <unordered_set>
#include <array>
#include <string>
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
        // Dependency Matrix
        int **D;
        // To be computed internally
        void update_r_a_p();
        // Method to sort the nodes
        void sort_V();
        // Method to map a node to index
        int index(const string&);
        // Method to initialize dependency matrix
        void initialize_dependency_matrix();
        // Method to add dependencies
        void add_edges();
        // Method to compute Dynamic Slice (Implementation of DSABM)
        unordered_set<int> DSABM(const string& s);
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
        void show_dependency_matrix(){
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
void ABDG::initialize_dependency_matrix(){
    D = new int*[n];
    for(int i = 0; i < n; i++)
        D[i] = new int[n];
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            D[i][j] = 0;
    add_edges();
}

// Method definition for add_edges()
void ABDG::add_edges(){
    cout<<"Enter the number of edges: ";
    cin>>e;
    string tail, head;
    int edge_type;
    cout<<"Enter edges as tail <space> head <space> edge_type: "<<endl;
    for(int i = 0; i < e; i++){
        cin>>tail>>head>>edge_type;
        D[index(tail)][index(head)] = edge_type;
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
    cout<<"Enter the number of vertices: ";
    cin>>n;
    V = new string[n];
    cout<<"Enter the "<<n<<" vertices: "<<endl;
    for(int i = 0; i < n; i++){
        cin>>V[i];
    }
    update_r_a_p();
    sort_V();
    initialize_dependency_matrix();
}

// Destructor Definition
ABDG::~ABDG(){
    for(int i = 0; i < n; i++){
        delete[] D[i];
    }
    delete[] D;
    delete[] V;
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