#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <map>
#include <queue>
// #include <chrono>
#include <fstream>
#include <conio.h>

using namespace std;

class ABDG {
    int n, e, r, a, p;
    string *V;
    vector<vector<pair<string, int>>> IDL;
    fstream fs;

    const map<int, double> EDGE_WEIGHTS = {
        {6, 1.0}, {5, 0.9}, {4, 0.7}, {3, 0.5}, {2, 0.3}, {1, 0.1}
    };

    void update_r_a_p();
    void sort_V();
    int getNodeIndex(const string& v);
    void initialize_incident_dependency_list();
    void add_edges();
    int index(const string&);
    unordered_set<int> DSABM_Integrated(const string& s, double tau);

public:
    ABDG();
    ~ABDG();
    void display_dynamic_slice(double tau = 0.4);
};

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

int ABDG::getNodeIndex(const string& v) {
    if (v == "S0") return 0;
    char type = v[0];
    int id = stoi(v.substr(1));

    switch(type) {
        case 'R': return id;
        case 'A': return r + id;
        case 'P': return r + a + id;
        default:  return r + a + p + id; // Join nodes (J)
    }
}

unordered_set<int> ABDG::DSABM_Integrated(const string& s, double tau) {
    unordered_set<int> tempDS;
    map<int, double> node_relevance;
    queue<pair<int, double>> M1, M2, M3;

    int start_node = getNodeIndex(s);
    M1.push({start_node, 1.0});
    node_relevance[start_node] = 1.0;

    // Phase 1: Traversal along direct data transactions and procedures
    while (!M1.empty()) {
        pair<int, double> current = M1.front();
        M1.pop();

        for (auto& edge : IDL[current.first]) {
            int u = getNodeIndex(edge.first);
            double new_w = current.second * EDGE_WEIGHTS.at(edge.second);

            if (new_w >= tau && new_w > node_relevance[u]) {
                node_relevance[u] = new_w;
                tempDS.insert(u);

                if (edge.second >= 3) M1.push({u, new_w}); // Types 6, 5, 4, 3
                else if (edge.second == 2) M2.push({u, new_w});
                else M3.push({u, new_w});
            }
        }
    }

    // Phase 2: Propagation along inter-agent message dependency
    while (!M2.empty()) {
        pair<int, double> current = M2.front();
        M2.pop();

        for (auto& edge : IDL[current.first]) {
            int u = getNodeIndex(edge.first);
            double new_w = current.second * EDGE_WEIGHTS.at(edge.second);

            if (new_w >= tau && new_w > node_relevance[u]) {
                node_relevance[u] = new_w;
                tempDS.insert(u);

                if (edge.second == 6 || edge.second == 5) M2.push({u, new_w});
                else if (edge.second == 4 || edge.second == 1) M3.push({u, new_w});
            }
        }
    }

    // Phase 3: Propagation along control dependency
    while (!M3.empty()) {
        pair<int, double> current = M3.front();
        M3.pop();

        for (auto& edge : IDL[current.first]) {
            int u = getNodeIndex(edge.first);
            double new_w = current.second * EDGE_WEIGHTS.at(edge.second);

            if (new_w >= tau && new_w > node_relevance[u]) {
                if (edge.second == 6 || edge.second == 5 || edge.second == 3) {
                    node_relevance[u] = new_w;
                    tempDS.insert(u);
                    M3.push({u, new_w});
                }
            }
        }
    }

    tempDS.erase(start_node);
    return tempDS;
}

void ABDG::display_dynamic_slice(double tau) {
    string s;
    cout << "\nThreshold (tau): " << tau << "\nEnter the slice criterion (node): ";
    cin >> s;

    // auto start = chrono::high_resolution_clock::now();
    unordered_set<int> result = DSABM_Integrated(s, tau);
    // auto stop = chrono::high_resolution_clock::now();

    cout << "Dynamic Slice Set: ";
    for(int idx : result) cout << V[idx] << " ";
    
    // auto duration = chrono::duration_cast<chrono::microseconds>(stop - start);
    // cout << "\nSlicing Time: " << duration.count() << " microseconds" << endl;
}

int main() {
    ABDG G;
    // Test multiple thresholds as per your loop
    // for(int i = 1; i <= 10; i++) {
    //     G.display_dynamic_slice(0.1 * i);
    // }
    G.display_dynamic_slice(0.15);
    _getch();
    return 0;
}