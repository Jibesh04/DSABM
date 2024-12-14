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
        - Version 1.1
        - Date: 12.12.2024
        - Version Desc: Ideal for new ABDGs (Takes ABDG as input and then displays required slice)
        - Drawback: It can handle 2 character node names only: upto R9 or A9 ..
*/

#include <iostream>
#include <unordered_set>
#include <array>
using namespace std;

// struct ArrayHash{
//     size_t operator()(const array<char, 2> & arr) const{
//         return hash<char>()(arr[0]) ^ hash<char>()(arr[1] << 1);
//     }
// };

class ABDG{
        // Number of Vertices (For initialization of Graph Representation)
        int n;
        // Set of Vertices
        char **V;
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
        int index(char*);
        // Method to initialize dependency matrix
        void initialize_dependency_matrix();
        // Method to add dependencies
        void add_edges();
        // // Method to check presence in a set
        // bool is_in(int, char**);
        // // Method to add an element to a set
        // void add_element(char**, int);
        // Method to compute Dynamic Slice (Implementation of DSABM)
        unordered_set<int> DSABM(char s[2]);
    public:
        // Constructor
        ABDG();
        // Destructor
        ~ABDG();
        // Method to show all vertices
        void show_vertices(){
            for(int i = 0; i < n - 1; i++)
                cout<<index(V[i])<<":"<<V[i][0]<<V[i][1]<<", ";
            cout<<index(V[n - 1])<<":"<<V[n - 1][0]<<V[n - 1][1]<<endl;
        }
        // Method to show dependency matrix
        void show_dependency_matrix(){
            cout<<"    ";
            for(int i = 0; i < n; i++)
                cout<<V[i][0]<<V[i][1]<<" ";
            cout<<endl;
            for(int i = 0; i < n; i++){
                cout<<" "<<V[i][0]<<V[i][1]<<" ";
                for(int j = 0; j < n; j++){
                    cout<<" "<<D[i][j]<<" ";
                }
                cout<<endl;
            }
        }
        // Method to display Dynamic Slice
        void display_dynamic_slice(){
            char s[2];
            cout<<"Enter the slice criterion (node): ";
            cin>>s[0]>>s[1];
            unordered_set<int> tempDS = DSABM(s);
            for(auto v : tempDS)
                cout<<V[v][0]<<V[v][1]<<" ";
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
    V[0][0] = 'S';
    V[0][1] = '0';
    for(int i = 1; i <= r; i++){
        V[i][0] = 'R';
        V[i][1] = (i + '0');
    }
    for(int i = 1; i <= a; i++){
        V[r + i][0] = 'A';
        V[r + i][1] = (i + '0');
    }
    for(int i = 1; i <= p; i++){
        V[r + a + i][0] = 'P';
        V[r + a + i][1] = (i + '0');
    }
    for(int i = 1; i <= n - (1 + r + a + p); i++){
        V[r + a + p + i][0] = 'J';
        V[r + a + p + i][1] = (i + '0');
    }
}

// Method definition for index()
int ABDG::index(char* v){
    switch(v[0]){
        case 'S':
            return 0;
        case 'R':
            return v[1] - '0';
        case 'A':
            return r + v[1] - '0';
        case 'P':
            return r + a + v[1] - '0';
        default:
            return r + a + p + v[1] - '0';
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
    char tail[2], head[2];
    int edge_type;
    cout<<"Enter edges as tail <space> head <space> edge_type: "<<endl;
    for(int i = 0; i < e; i++){
        cin>>tail[0]>>tail[1]>>head[0]>>head[1]>>edge_type;
        D[index(tail)][index(head)] = edge_type;
    }
}

// // Method definition for is_in()
// bool ABDG::is_in(int i, char* DS[2]){
//     for(int j = 0; j < n; j++)
//         if(DS[j][0] == V[i][0] && DS[j][1] == V[i][1])
//             return true;
//     return false;
// }

// // Method definition for add_element()
// void ABDG::add_element(char* DS[2], int i){
//     size_t m = sizeof(DS)/sizeof(DS[0]);
//     DS[m] = new char[2];
//     DS[m][0] = V[i][0];
//     DS[m][1] = V[i][1];
// }

// Method definition for DSABM()
unordered_set<int> ABDG::DSABM(char s[2]){
    unordered_set<int> M1, M2, M3, tempDS;
    M1.insert(index(s));
    while(!M1.empty()){
        auto it = M1.begin();
        int v = *it;
        // cout<<V[v][0]<<V[v][1]<<endl;
        M1.erase(it);
        for(int i = 0; i < n; i++){
            if(!D[i][v])
                continue;
            // cout<<D[i][v];
            if(tempDS.find(i) == tempDS.end()){
                tempDS.insert(i);
                // cout<<"Reached Here";
                if(D[i][v] == 6 || D[i][v] == 5 || D[i][v] == 4 || D[i][v] == 3)
                    M1.insert(i);
                else if(D[i][v] == 2)
                    M2.insert(i);
                else
                    M3.insert(i);
            }
        }
    }
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
    V = new char*[n]; 
    cout<<"Enter the "<<n<<" vertices: "<<endl;
    for(int i = 0; i < n; i++){
        V[i] = new char[2];
        cin>>V[i][0]>>V[i][1];
    }
    update_r_a_p();
    sort_V();
    initialize_dependency_matrix();
}

// Destructor Definition
ABDG::~ABDG(){
    for(int i = 0; i < n; i++){
        delete V[i];
        delete D[i];
    }
    delete V;
    delete D;
}

int main(){
    ABDG G;
    G.show_vertices();
    G.show_dependency_matrix();
    G.display_dynamic_slice();
    return 0;
}