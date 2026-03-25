#include "linked_list.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

int getTransportIndex(string mode) {
    if (mode == "Car") return 0;
    if (mode == "Bus") return 1;
    if (mode == "Bicycle") return 2;
    if (mode == "Walking") return 3;
    if (mode == "School Bus") return 4;
    if (mode == "Carpool") return 5;
    return -1; 
}

string getTransportName(int index) {
    string modes[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    return modes[index];
}

void executeLinkedListAnalysis(Node* heads[], int numLists, string cityName) {
    int ageGroupCounts[5] = {0};
    double ageGroupEmissions[5] = {0.0};
    int transportFreq[5][6] = {0}; 
    
    double totalDatasetEmission = 0.0;
    double modeEmissions[6] = {0.0};

    for (int listIndex = 0; listIndex < numLists; listIndex++) {
        Node* current = heads[listIndex];
        
        if (current == nullptr) continue;

        while (current != nullptr) {
            double monthlyEmission = current->data.dailyDistance * current->data.emissionFactor * current->data.avgDaysPerMonth;
            totalDatasetEmission += monthlyEmission;

            int aGroup = -1;
            if (current->data.age >= 6 && current->data.age <= 17) aGroup = 0;
            else if (current->data.age >= 18 && current->data.age <= 25) aGroup = 1;
            else if (current->data.age >= 26 && current->data.age <= 45) aGroup = 2;
            else if (current->data.age >= 46 && current->data.age <= 60) aGroup = 3;
            else if (current->data.age >= 61 && current->data.age <= 100) aGroup = 4;

            if (aGroup != -1) {
                ageGroupCounts[aGroup]++;
                ageGroupEmissions[aGroup] += monthlyEmission;
                
                int tIndex = getTransportIndex(current->data.transportMode);
                if (tIndex != -1) {
                    transportFreq[aGroup][tIndex]++;
                    modeEmissions[tIndex] += monthlyEmission;
                }
            }
            current = current->next;
        }
    }

    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
    
    cout << " DEMOGRAPHIC & EMISSION ANALYSIS: " << cityName << "\n";
    cout << "----------------------------------------------------------------------\n";
    cout << "Total Carbon Emission for Selection: " << fixed << setprecision(2) << totalDatasetEmission << " kg CO2\n\n";

    cout << left << setw(15) << "Age Group" 
         << setw(18) << "Total Residents"
         << setw(20) << "Preferred Mode" 
         << setw(20) << "Total Emission" 
         << setw(20) << "Avg Emission" << "\n";
    cout << "---------------------------------------------------------------------------------------------\n";

    for (int i = 0; i < 5; i++) {
        if (ageGroupCounts[i] == 0) continue; 

        int maxFreq = -1;
        int preferredModeIndex = 0;
        for (int j = 0; j < 6; j++) {
            if (transportFreq[i][j] > maxFreq) {
                maxFreq = transportFreq[i][j];
                preferredModeIndex = j;
            }
        }

        double avgEmission = ageGroupEmissions[i] / ageGroupCounts[i];

        cout << left << setw(15) << ageLabels[i]
             << setw(18) << ageGroupCounts[i]
             << setw(20) << getTransportName(preferredModeIndex)
             << setw(20) << fixed << setprecision(2) << ageGroupEmissions[i]
             << setw(20) << avgEmission << "\n";
    }

    cout << "\n--- EMISSIONS PER TRANSPORT MODE ---\n";
    for (int j = 0; j < 6; j++) {
        if (modeEmissions[j] > 0) {
            cout << left << setw(15) << getTransportName(j) 
                 << ": " << modeEmissions[j] << " kg CO2\n";
        }
    }
    cout << "----------------------------------------------------------------------\n";
}

Node* loadCityData(string filename) {
    Node* head = nullptr;
    Node* tail = nullptr;

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "WARNING: Target file [" << filename << "] failed to mount. Verify CWD." << endl;
        return nullptr;
    }

    string line;
    getline(file, line); 

    while (getline(file, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string token;
        Commuter temp;

        getline(ss, temp.residentID, ',');
        getline(ss, token, ','); temp.age = stoi(token);
        getline(ss, temp.transportMode, ',');
        getline(ss, token, ','); temp.dailyDistance = stod(token);
        getline(ss, token, ','); temp.emissionFactor = stod(token);
        getline(ss, token, ',');
        token.erase(remove(token.begin(), token.end(), '\r'), token.end());
        temp.avgDaysPerMonth = stoi(token);

        Node* newNode = new Node;
        newNode->data = temp;
        newNode->next = nullptr;

        if (head == nullptr) { head = newNode; tail = newNode; } 
        else { tail->next = newNode; tail = newNode; }
    }
    file.close();
    return head;
}

void queryEmissionByAge(Node* heads[], int numLists, bool activeAges[5], string targetName) {
    double totalCombinedEmission = 0.0;
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
    string ageDescriptors[] = {"(School Students)", "(University Students)", "(Working Adults)", "(Older Adults)", "(Senior Citizens)"};

    for (int ageTarget = 0; ageTarget < 5; ageTarget++) {
        if (!activeAges[ageTarget]) continue;

        double totalAgeEmission = 0.0;
        int transportCounts[6] = {0};
        double transportEmissions[6] = {0.0};

        for (int listIndex = 0; listIndex < numLists; listIndex++) {
            Node* current = heads[listIndex];
            if (current == nullptr) continue;
            
            while (current != nullptr) {
                int aGroup = -1;
                if (current->data.age >= 6 && current->data.age <= 17) aGroup = 0;
                else if (current->data.age >= 18 && current->data.age <= 25) aGroup = 1;
                else if (current->data.age >= 26 && current->data.age <= 45) aGroup = 2;
                else if (current->data.age >= 46 && current->data.age <= 60) aGroup = 3;
                else if (current->data.age >= 61 && current->data.age <= 100) aGroup = 4;

                if (aGroup == ageTarget) {
                    double monthlyEmission = current->data.dailyDistance * current->data.emissionFactor * current->data.avgDaysPerMonth;
                    totalAgeEmission += monthlyEmission;

                    int tIndex = getTransportIndex(current->data.transportMode);
                    if (tIndex != -1) {
                        transportCounts[tIndex]++;
                        transportEmissions[tIndex] += monthlyEmission;
                    }
                }
                current = current->next;
            }
        }

        totalCombinedEmission += totalAgeEmission;

        cout << "\nAge Group: " << ageLabels[ageTarget] << " " << ageDescriptors[ageTarget] << "\n";
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(20) << "Mode of Transport" << setw(10) << "Count" 
             << setw(30) << "Total Emission (kg CO2)" << setw(20) << "Average per Resident" << "\n";

        for (int j = 0; j < 6; j++) {
            if (transportCounts[j] > 0) { 
                double avgEmission = transportEmissions[j] / transportCounts[j];
                cout << left << setw(20) << getTransportName(j) << setw(10) << transportCounts[j] 
                     << setw(30) << fixed << setprecision(2) << transportEmissions[j] << setw(20) << avgEmission << "\n";
            }
        }
        cout << "----------------------------------------------------------------------\n";
        cout << "Total Emission for Age Group: " << fixed << setprecision(2) << totalAgeEmission << " kg CO2\n";
    }
    
    if (totalCombinedEmission > 0) {
        cout << "\n>>> AGGREGATE FILTRATION EMISSION: " << fixed << setprecision(2) << totalCombinedEmission << " kg CO2 <<<\n";
    }
}

void queryEmissionByTransport(Node* heads[], int numLists, bool activeTransports[6], string targetName) {
    double totalCombinedEmission = 0.0;
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};

    for (int transTarget = 0; transTarget < 6; transTarget++) {
        if (!activeTransports[transTarget]) continue;

        double totalTransEmission = 0.0;
        int demographicCounts[5] = {0};
        double demographicEmissions[5] = {0.0};

        for (int listIndex = 0; listIndex < numLists; listIndex++) {
            Node* current = heads[listIndex];
            if (current == nullptr) continue;
            
            while (current != nullptr) {
                int tIndex = getTransportIndex(current->data.transportMode);
                
                if (tIndex == transTarget) {
                    double monthlyEmission = current->data.dailyDistance * current->data.emissionFactor * current->data.avgDaysPerMonth;
                    totalTransEmission += monthlyEmission;

                    int aGroup = -1;
                    if (current->data.age >= 6 && current->data.age <= 17) aGroup = 0;
                    else if (current->data.age >= 18 && current->data.age <= 25) aGroup = 1;
                    else if (current->data.age >= 26 && current->data.age <= 45) aGroup = 2;
                    else if (current->data.age >= 46 && current->data.age <= 60) aGroup = 3;
                    else if (current->data.age >= 61 && current->data.age <= 100) aGroup = 4;

                    if (aGroup != -1) {
                        demographicCounts[aGroup]++;
                        demographicEmissions[aGroup] += monthlyEmission;
                    }
                }
                current = current->next;
            }
        }

        totalCombinedEmission += totalTransEmission;

        cout << "\nTransport Sector: " << getTransportName(transTarget) << "\n";
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(20) << "Demographic" << setw(10) << "Count" 
             << setw(30) << "Total Emission (kg CO2)" << setw(20) << "Average per Resident" << "\n";

        for (int j = 0; j < 5; j++) {
            if (demographicCounts[j] > 0) { 
                double avgEmission = demographicEmissions[j] / demographicCounts[j];
                cout << left << setw(20) << ageLabels[j] << setw(10) << demographicCounts[j] 
                     << setw(30) << fixed << setprecision(2) << demographicEmissions[j] << setw(20) << avgEmission << "\n";
            }
        }
        cout << "----------------------------------------------------------------------\n";
        cout << "Total Emission for Transport Sector: " << fixed << setprecision(2) << totalTransEmission << " kg CO2\n";
    }

    if (totalCombinedEmission > 0) {
        cout << "\n>>> AGGREGATE FILTRATION EMISSION: " << fixed << setprecision(2) << totalCombinedEmission << " kg CO2 <<<\n";
    }
}

void executeCrossDatasetComparison(Node* cityA, Node* cityB, Node* cityC) {
    Node* cities[] = {cityA, cityB, cityC};
    string cityNames[] = {"City A", "City B", "City C"};
    
    double ageGroupEmissions[3][5] = {0.0}; 
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};

    for (int c = 0; c < 3; c++) {
        Node* current = cities[c];
        while (current != nullptr) {
            int aGroup = -1;
            if (current->data.age >= 6 && current->data.age <= 17) aGroup = 0;
            else if (current->data.age >= 18 && current->data.age <= 25) aGroup = 1;
            else if (current->data.age >= 26 && current->data.age <= 45) aGroup = 2;
            else if (current->data.age >= 46 && current->data.age <= 60) aGroup = 3;
            else if (current->data.age >= 61 && current->data.age <= 100) aGroup = 4;

            if (aGroup != -1) {
                ageGroupEmissions[c][aGroup] += (current->data.dailyDistance * current->data.emissionFactor * current->data.avgDaysPerMonth);
            }
            current = current->next;
        }
    }

    cout << " Age vs Carbon\n";
    cout << "----------------------------------------------------------------------\n";
    cout << left << setw(15) << "Age Group" << setw(18) << "City A" << setw(18) << "City B" << setw(18) << "City C" << "\n";
    cout << "------------------------------------------------------------------------\n";
    
    for (int i = 0; i < 5; i++) {
        cout << left << setw(15) << ageLabels[i]
             << setw(18) << fixed << setprecision(2) << ageGroupEmissions[0][i]
             << setw(18) << ageGroupEmissions[1][i]
             << setw(18) << ageGroupEmissions[2][i] << "\n";
    }
}

void executeCrossDatasetTransportComparison(Node* cityA, Node* cityB, Node* cityC) {
    Node* cities[] = {cityA, cityB, cityC};
    
    double transportEmissions[3][6] = {0.0}; 

    for (int c = 0; c < 3; c++) {
        Node* current = cities[c];
        while (current != nullptr) {
            int tIndex = getTransportIndex(current->data.transportMode);
            if (tIndex != -1) {
                transportEmissions[c][tIndex] += (current->data.dailyDistance * current->data.emissionFactor * current->data.avgDaysPerMonth);
            }
            current = current->next;
        }
    }

    cout << " Transport vs Carbon\n";
    cout << "----------------------------------------------------------------------\n";
    cout << left << setw(15) << "Transport Mode" << setw(18) << "City A" << setw(18) << "City B" << setw(18) << "City C" << "\n";
    cout << "------------------------------------------------------------------------\n";
    
    for (int i = 0; i < 6; i++) {
        cout << left << setw(15) << getTransportName(i)
             << setw(18) << fixed << setprecision(2) << transportEmissions[0][i]
             << setw(18) << transportEmissions[1][i]
             << setw(18) << transportEmissions[2][i] << "\n";
    }
}

void executeAgeVsTransportComparison(Node* cityA, Node* cityB, Node* cityC) {
    Node* cities[] = {cityA, cityB, cityC};
    
    int ageTransportCounts[5][6] = {0}; 
    double ageTransportEmissions[5][6] = {0.0};
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};

    for (int c = 0; c < 3; c++) {
        Node* current = cities[c];
        while (current != nullptr) {
            int aGroup = -1;
            if (current->data.age >= 6 && current->data.age <= 17) aGroup = 0;
            else if (current->data.age >= 18 && current->data.age <= 25) aGroup = 1;
            else if (current->data.age >= 26 && current->data.age <= 45) aGroup = 2;
            else if (current->data.age >= 46 && current->data.age <= 60) aGroup = 3;
            else if (current->data.age >= 61 && current->data.age <= 100) aGroup = 4;

            int tIndex = getTransportIndex(current->data.transportMode);

            if (aGroup != -1 && tIndex != -1) {
                ageTransportCounts[aGroup][tIndex]++;
                ageTransportEmissions[aGroup][tIndex] += (current->data.dailyDistance * current->data.emissionFactor * current->data.avgDaysPerMonth);
            }
            current = current->next;
        }
    }

    cout << " Age vs Transport (Headcount & Emissions)\n";
    cout << "--------------------------------------------------------------------------------------------------------------------------------\n";
    cout << left << setw(15) << "Age Group";
    for (int i = 0; i < 6; i++) {
        cout << setw(20) << getTransportName(i);
    }
    cout << "\n--------------------------------------------------------------------------------------------------------------------------------\n";
    
    for (int i = 0; i < 5; i++) {
        cout << left << setw(15) << ageLabels[i];
        for (int j = 0; j < 6; j++) {
            stringstream cell;
            if (ageTransportCounts[i][j] > 0) {
                cell << ageTransportCounts[i][j] << " (" << fixed << setprecision(1) << ageTransportEmissions[i][j] << ")";
            } else {
                cell << "0";
            }
            cout << left << setw(20) << cell.str();
        }
        cout << "\n";
    }
}

void destroyList(Node* head) {
    Node* current = head;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}