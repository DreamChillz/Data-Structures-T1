#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <string>

using namespace std;

struct Commuter {
    string residentID;
    int age;
    string transportMode;
    double dailyDistance;
    double emissionFactor;
    int avgDaysPerMonth;
};

struct Node {
    Commuter data;
    Node* next;
};

// Core Infrastructure
Node* loadCityData(string filename);
void destroyList(Node* head);

// Analytical Queries
void executeLinkedListAnalysis(Node* heads[], int numLists, string cityName);
void queryEmissionByAge(Node* heads[], int numLists, bool activeAges[5], string targetName);
void queryEmissionByTransport(Node* heads[], int numLists, bool activeTransports[6], string targetName);
void executeCrossDatasetComparison(Node* cityA, Node* cityB, Node* cityC);
void executeCrossDatasetTransportComparison(Node* cityA, Node* cityB, Node* cityC);
void executeAgeVsTransportComparison(Node* cityA, Node* cityB, Node* cityC);

#endif