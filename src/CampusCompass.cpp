#include "CampusCompass.h"
//#include <string>

using namespace std;

Graph::Graph() {
    //Empty 
}

void Graph::AddEdge(int id1, int id2, int time) {
    //Validate if edge exists already elsewhere
    adjList[id1].push_back(Edge(id2, time));
    adjList[id2].push_back(Edge(id1, time)); 
}

void Graph::SetEdge(int id1, int id2, bool open) {
    // Set the edge state (open/closed) for both directions in the undirected graph.
    auto it1 = adjList.find(id1);
    if (it1 != adjList.end()) {
        for (auto &e : it1->second) {
            if (e.to == id2) {
                e.open = open;
                break; // assume no duplicate parallel edges
            }
        }
    }
    // Do the same for id2 -> id1
    auto it2 = adjList.find(id2);
    if (it2 != adjList.end()) {
        for (auto &e : it2->second) {
            if (e.to == id1) {
                e.open = open;
                break;
            }
        }
    }
}

void Graph::SetLocationName(int id, const string& name) {
    //TODO: may have to add error check if id is not in graph but probably not
    locationIDToName[id] = name; //overrides previous, which is fine
}

string Graph::GetLocationName(int id) const {
    return locationIDToName.at(id);
}

//1 = open, 0 = closed, -1 = edge not found
int Graph::isEdgeOpen(int id1, int id2) const {
    auto it = adjList.find(id1);
    if (it != adjList.end()) {
        for (const auto &e : it->second) {
            if (e.to == id2) {
                return e.open ? 1 : 0;
            }
        }
    }
    return -1; // Edge not found
}

bool Graph::IsConnected(int start, int target) const {
    // Implement BFS to check connectivity
    if (start == target) return true; //TODO: check what to return in this case
    queue<int> q;
    unordered_map<int, bool> visited;
    q.push(start);
    visited[start] = true;
    while(!q.empty()) {
        int current = q.front();
        q.pop();
        auto it = adjList.find(current);
        if (it == adjList.end()) continue;
        for (const auto &edge : it->second) {
            if (edge.open && !visited[edge.to]) {
                if (edge.to == target) return true;
                visited[edge.to] = true;
                q.push(edge.to);
            }
        }
    }
    return false; //No path found
}
//TODO: may want to switch from taking in the source to taking the full student object, but I think not 
tuple<unordered_map<int, int>, unordered_map<int, int>> Graph::Dijkstra(int source) const {
    //How do I iterate through all of the nodes in dijkstras without having the list of 
    //TODO: add print statements, filter out list of distances to only include classes that student is taking 
    //I think implement the above in CC class because dijkstras is used multiple times, so it may not be needed for all the functions
    unordered_map<int, int> parent;
    unordered_map<int, int> dist;
    for(auto& [node, edge] : adjList) {
        //node_to_parent[node] = nullptr;
        dist[node] = numeric_limits<int>::max();
    }
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> minHeap; //Need to use a minheap instead of a regular queue because I need to take the edge with the shortest time cost always 
    dist[source] = 0;
    minHeap.push({0, source}); // (distance, node)
    while(!minHeap.empty()) {
        pair<int, int> current = minHeap.top();
        minHeap.pop();
        
        if(current.first > dist[current.second]) {
            continue; //Skips entry if we already found better before coming back to it 
        }
        if (adjList.find(current.second) == adjList.end()) {
            continue; //No edges from this node 
        }

        auto it = adjList.find(current.second);
        if (it == adjList.end()) continue;
        //vector<Edge> edges = it->second;
        for (const Edge& e : it->second) {
            if(e.open) {
                int newDist = current.first + e.time;
                //Edge relaxation 
                if(newDist < dist[e.to]) {
                    dist[e.to] = newDist;
                    parent[e.to] = current.second;
                    minHeap.push({newDist, e.to});
                }
            }
        }
    }
    return make_tuple(dist, parent);
}

//NOTE: Uses a subgraph
//TODO: need to make method that generates the subgraph to call this and make sure I implement it correctly. 
//TODO: Might just generate another temporary graph object which means I wont need to pass the subgraph as a parameter
int Graph::MST(const unordered_map<int, vector<Edge>>& subGraph) const {
    if(subGraph.empty()) {
        return 0;
    }

    int result = 0;
    unordered_set<int> nodesInMST;
    //unordered_map<int, vector<Edge>> mst; //Don't need because dont have to return full graph 
    //Same as for dijkstras: stored as (distance, id)
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> minHeap; //Need to use a minheap instead of a regular queue because I need to take the edge with the shortest time cost always 
    int start = subGraph.begin()->first;
    minHeap.push(pair<int, int>(0, start));

    while(!minHeap.empty()) {
        auto [cost, node] = minHeap.top();
        minHeap.pop();

        if(nodesInMST.find(node) != nodesInMST.end()) {
            continue; //Node is already in zone, so continue
        }
        nodesInMST.insert(node);
        //First iter: current.first = 0
        result += cost; //Adds cheapest next edge, which is the top of the minHeap. Will not add edge with the same node again. 

            auto it = subGraph.find(node);
            if (it != subGraph.end()) {
                for (const Edge& e : it->second) {
                    if (e.open && subGraph.find(e.to) != subGraph.end() && nodesInMST.find(e.to) == nodesInMST.end()) {
                        minHeap.push(pair<int, int>(e.time, e.to));
                    }
                }
            }
    }

    return result;
}

void Graph::printGraph() const {
    if(adjList.empty()) {
        cout << "Graph is empty." << endl;
        return;
    }
    for (const auto& [node, edges] : adjList) {
        cout << "Node " << node << ":\n";
        for (const auto& edge : edges) {
            cout << "  -> " << edge.to << " (time: " << edge.time << ", open: " << (edge.open ? "yes" : "no") << ")\n";
        }
    }
}

CampusCompass::CampusCompass() {
    //Empty
}


//Next step after implementing dijkstra and other graph algorithms

bool CampusCompass::ParseCSV(const string &edges_filepath, const string &classes_filepath) {
    ifstream f1(edges_filepath), f2(classes_filepath);
    if (!f1.is_open() || !f2.is_open()) return false;

    string line;
    getline(f1, line); // skip header

    //Parse edges.csv to generate graph
    while (getline(f1, line)) {
        stringstream ss(line);
        string a,b,n1,n2,t;
        getline(ss,a,','); getline(ss,b,',');
        getline(ss,n1,','); getline(ss,n2,','); getline(ss,t,',');

        graph.AddEdge(stoi(a), stoi(b), stoi(t));
        //Store location_ID -> name conversion inside graph
        graph.SetLocationName(stoi(a), n1);
        graph.SetLocationName(stoi(b), n2);

    }
    f1.close();

    //Parse classes for class objects
    getline(f2, line); // skip header
    while (getline(f2, line)) {
        stringstream ss(line);
        UFClass c;
        string loc, s, e;

        getline(ss,c.code,',');
        getline(ss,loc,','); getline(ss,s,','); getline(ss,e,',');

        c.location_ID = stoi(loc);

        auto toMin = [](const string& x){
            int h = stoi(x.substr(0,2));
            int m = stoi(x.substr(3,2));
            return h*60 + m;
        };

        c.start_time = toMin(s);
        c.end_time   = toMin(e);

        classes[c.code] = c;
    }
    f2.close();


    //REMOVE AFTER DEBUGGING
    graph.printGraph();
    return true;
}

//Helper for ParseCommand
bool CampusCompass::isValidUFID(const string& id) {
    if (id.size() != 8) return false;
    for (char c : id) if (!isdigit(c)) return false;
    return true;
}

bool CampusCompass::ParseCommand(const string &command) {
    if (command.empty()) {
        cout << "unsuccessful\n";
        return false;
    }

    istringstream ss(command);
    string cmd;
    if (!(ss >> cmd)) {
        cout << "unsuccessful\n";
        return false;
    }

    //CHECK INSERT COMMAND
    if (cmd == "insert") { //Why is this not returning true right now???
        return insert(command);  
    }
    
    //CHECK REMOVE COMMAND
    if (cmd == "remove") {
        string ufid;
        if (!(ss >> ufid) || !isValidUFID(ufid)) {
            cout << "unsuccessful\n";
            return false;
        }
        return removeStudent(ufid);
    }

    //CHECK DROPCLASS COMMAND
    if (cmd == "dropClass") {
        string ufid, code;
        if (!(ss >> ufid >> code) || !isValidUFID(ufid)) {
            cout << "unsuccessful\n";
            return false;
        }
        return dropClass(ufid, code);
    }
    
    //CHECK REPLACECLASS COMMAND
    if (cmd == "replaceClass") {
        string ufid, oldCode, newCode;
        if (!(ss >> ufid >> oldCode >> newCode) || !isValidUFID(ufid)) {
            cout << "unsuccessful\n";
            return false;
        }
        return replaceClass(ufid, oldCode, newCode);
    }

    //CHECK REMOVECLASS COMMAND
    if (cmd == "removeClass") {
        string code;
        if (!(ss >> code)) {
            cout << "unsuccessful\n";
            return false;
        }
        return removeClassGlobal(code);
    }

    //CHECK TOGGLEEDGESCLOSURE COMMAND
    if (cmd == "toggleEdgesClosure") {
        int n;
        if (!(ss >> n) || n <= 0) {
            cout << "unsuccessful\n";
            return false;
        }

        vector<int> ids;
        for (int i = 0; i < 2 * n; i++) {
            int x;
            if (!(ss >> x)) {
                cout << "unsuccessful\n";
                return false;
            }
            ids.push_back(x);
        }

        return toggleEdgesClosure(n, ids);
    }

    //CHECK CHECKEDGESTATUS COMMAND
    if (cmd == "checkEdgeStatus") {
        int a, b;
        if (!(ss >> a >> b)) {
            cout << "unsuccessful\n";
            return false;
        }
        return checkEdgeStatus(a, b);
    }

    //CHECK ISCONNECTED COMMAND
    if (cmd == "isConnected") {
        int a, b;
        if (!(ss >> a >> b)) {
            cout << "unsuccessful\n";
            return false;
        }
        return isConnectedCmd(a, b);
    }

    //CHECK PRINTSHORTESTEDGES COMMAND
    if (cmd == "printShortestEdges") {
        string ufid;
        if (!(ss >> ufid) || !isValidUFID(ufid)) {
            cout << "unsuccessful\n";
            return false;
        }
        return printShortestEdges(ufid);
    }

    //CHECK PRINTSTUDENTZONE COMMAND
    if (cmd == "printStudentZone") {
        string ufid;
        if (!(ss >> ufid) || !isValidUFID(ufid)) {
            cout << "unsuccessful\n";
            return false;
        }
        return printStudentZone(ufid);
    }

    //CHECK VERIFY SCHEDULE COMMAND
    if (cmd == "verifySchedule") {
        string ufid;
        if (!(ss >> ufid) || !isValidUFID(ufid)) {
            cout << "unsuccessful\n";
            return false;
        }
        return verifySchedule(ufid);
    }
    //Invalid command name
    cout << "unsuccessful\n";
    return false;
}


//TODO: methods for all the commands user can pass 

bool CampusCompass::insert(const string& command) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::removeStudent(const string& ufid) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::dropClass(const string& ufid, const string& classCode) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::replaceClass(const string& ufid, const string& oldC, const string& newC) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::removeClassGlobal(const string& classCode) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::toggleEdgesClosure(int n, const vector<int>& ids) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::checkEdgeStatus(int a, int b) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::isConnectedCmd(int a, int b) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::printShortestEdges(const string& ufid) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::printStudentZone(const string& ufid) {
    cout << "successful" << endl;
    return true;
}

bool CampusCompass::verifySchedule(const string& ufid) {
    cout << "successful" << endl;
    return true;
}
