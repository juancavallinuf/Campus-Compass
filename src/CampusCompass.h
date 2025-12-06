#pragma once
#include <string>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <unordered_set>
#include <tuple>
#include <limits>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace std;

//STEP 1: create skeleton DONE!!
//STEP 2: fill in class information DONE!!
//STEP 3: data extraction from main and test that it is instantiating the methods correctly
//STEP 4: skeleton for methods for main function
//TODO: write skeleton for main, very quickly check that file reading works
//(3 and 4 should be quick: ~1 hour)
//Step 5: write tests (1 hour)
//Step 6: make sure string parsing works (2 hours)
//Step 7: finish main class (2 hours)
//I have about 8 hours left
//Graph is built using an adjacency list

class Graph {
    private:
        struct Edge {
            int to;
            int time; //Edge weight for time 
            bool open; //checks if edge is closed due to construction or other factors or not
            Edge(int _to, int _time) : to(_to), time(_time), open(true) {};
        };
        unordered_map<int, vector<Edge>> adjList; //Because of how edge works, same for residence and other buildings and clases, just need to check if its ID matches input UFCLass ID
        unordered_map<int, string> locationIDToName; //Converts id to name
    public:
        Graph(); 
    
        void AddEdge(int id1, int id2, int time);
        void SetEdge(int id1, int id2, bool open);
        int isEdgeOpen(int id1, int id2) const;;
        //TODO: getEdge function???
        //Graph algorithms
        bool IsConnected(int start, int target) const;
        tuple<unordered_map<int, int>, unordered_map<int, int>> Dijkstra(int source) const;
        int MST(const unordered_map<int, vector<Edge>>& subGraph) const;
        //TODO: Helper functions??
        void printGraph() const; //for debugging
        void SetLocationName(int id, const string& name);
        string GetLocationName(int id) const;

};

struct Student {
        vector<string> classes; //Stores codes, not locations
        string UFID; //Student ID, 8 digits ???save as string
        string name;
        int residence_ID; //Used for algorithms
        Student(string UFID, const string &name, int residence_ID) : UFID(UFID), name(name), residence_ID(residence_ID) {}; //Make sure this is valid for .h
};

struct UFClass {
    string code; //EX:COP3530
    int location_ID; //Used for graph algos
    vector<string> students; //List of student IDs. May not be necessary 
    int start_time; //In minutes. Allows easy comparison
    int end_time;
};

class CampusCompass {
private:
    Graph graph;
    //Logic: will use int ids from adj list to get the full data from these maps
    unordered_map<string, Student> students; //Maps student ID to student object
    unordered_map<string, UFClass> classes; //Maps class ID to UFClass object
    // Think about what member variables you need to initialize
    // perhaps some graph representation?
public:
    // Think about what helper functions you will need in the algorithm
    CampusCompass(); // constructor
    bool ParseCSV(const string &edges_filepath, const string &classes_filepath);
    bool ParseCommand(const string &command);

    //Helper function
    bool isValidUFID(const string& id);
    //Functons to handle each instruction:
    bool insert(const string& command);
    bool removeStudent(const string& ufid);
    bool dropClass(const string& ufid, const string& classCode);
    bool replaceClass(const string& ufid, const string& oldC, const string& newC);
    bool removeClassGlobal(const string& classCode);
    bool toggleEdgesClosure(int n, const vector<int>& ids);
    bool checkEdgeStatus(int a, int b);
    bool isConnectedCmd(int a, int b);
    bool printShortestEdges(const string& ufid);
    bool printStudentZone(const string& ufid);
    bool verifySchedule(const string& ufid);
};
