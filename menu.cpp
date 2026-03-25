#include <iostream>
#include <string>
#include <sstream>
#include "linked_list.h"

using namespace std;

string getRawBuffer() {
    string line;
    getline(cin, line);
    if (line.empty()) getline(cin, line); 
    return line;
}

void executeEmissionSubMenu(Node* cities[], string cityNames[]) {
    int filterChoice = 0;
    while (true) {
        cout << "\nCARBON EMISSION FILTRATION\n";
        cout << "1. Filter by age\n";
        cout << "2. Filter by transport mode\n";
        cout << "3. Age vs Carbon\n";
        cout << "4. Transport vs Carbon (Cross Dataset)\n";
        cout << "5. Age vs Transport\n";
        cout << "6. Back\n";
        cout << "Vector: ";
        
        string input = getRawBuffer();
        stringstream ss(input);
        ss >> filterChoice;

        if (filterChoice == 6) return;

        if (filterChoice == 3) {
            executeCrossDatasetComparison(cities[0], cities[1], cities[2]);
            continue;
        }

        if (filterChoice == 4) {
            executeCrossDatasetTransportComparison(cities[0], cities[1], cities[2]);
            continue;
        }

        if (filterChoice == 5) {
            executeAgeVsTransportComparison(cities[0], cities[1], cities[2]);
            continue;
        }

        if (filterChoice == 1 || filterChoice == 2) {
            bool activeFlags[6] = {false}; 
            
            if (filterChoice == 1) {
                cout << "\nSelect Demographics (Space-separated, e.g., '1 4 5'):\n";
                cout << "1. 6-17\n2. 18-25\n3. 26-45\n4. 46-60\n5. 61-100\nTargets: ";
                string demoInput = getRawBuffer();
                stringstream dss(demoInput);
                int val;
                while (dss >> val) {
                    if (val >= 1 && val <= 5) activeFlags[val - 1] = true;
                }
            } else {
                cout << "\nSelect Transport (Space-separated, e.g., '1 2'):\n";
                cout << "1. Car\n2. Bus\n3. Bicycle\n4. Walking\n5. School Bus\n6. Carpool\nTargets: ";
                string transInput = getRawBuffer();
                stringstream tss(transInput);
                int val;
                while (tss >> val) {
                    if (val >= 1 && val <= 6) activeFlags[val - 1] = true;
                }
            }

            if (filterChoice == 1) queryEmissionByAge(cities, 3, activeFlags, "Custom Demographic Filter");
            if (filterChoice == 2) queryEmissionByTransport(cities, 3, activeFlags, "Custom Transport Filter");

        } else {
            cout << "please select in range\n";
        }
    }
}

void executeCitySelection(int dsMode, int operationMode, Node* cityA, Node* cityB, Node* cityC) {
    while (true) {
        cout << "\n--- MOUNT TARGET DATASETS ---\n";
        cout << "Please select which city (e.g., '1 3' for City A + City C).\n";
        cout << "1. City A\n2. City B\n3. City C\n4. Back\nTargets: ";
        
        string input = getRawBuffer();
        stringstream ss(input);
        int val;
        
        Node* activeCities[3] = {nullptr, nullptr, nullptr};
        string activeNames[3] = {"", "", ""};
        bool abort = false;
        int listCount = 0;

        while (ss >> val) {
            if (val == 4) return; // Pop stack
            if (val == 1 && activeCities[0] == nullptr) { activeCities[0] = cityA; activeNames[0] = "City A"; listCount++; }
            else if (val == 2 && activeCities[1] == nullptr) { activeCities[1] = cityB; activeNames[1] = "City B"; listCount++; }
            else if (val == 3 && activeCities[2] == nullptr) { activeCities[2] = cityC; activeNames[2] = "City C"; listCount++; }
            else if (val < 1 || val > 4) { abort = true; break; }
        }

        if (abort || listCount == 0) {
            cout << "\nplease select in range\n";
            continue;
        }

        if (dsMode == 1) {
            cout << "\nnot implemented yet\n";
        } else if (dsMode == 2) {
            if (operationMode == 1) {
                Node* compressedTargets[3];
                int idx = 0;
                string combinedName = "";
                for(int i=0; i<3; i++) {
                    if(activeCities[i] != nullptr) {
                        compressedTargets[idx++] = activeCities[i];
                        combinedName += activeNames[i] + " ";
                    }
                }
                executeLinkedListAnalysis(compressedTargets, listCount, combinedName);
            } else if (operationMode == 2) {
                executeEmissionSubMenu(activeCities, activeNames);
            } else {
                cout << "\nnot impelmented yet\n";
            }
        }
    }
}

void executeOperationSelection(int dsMode, Node* cityA, Node* cityB, Node* cityC) {
    int taskChoice = 0;
    while (true) {
        cout << "\n=== COMPUTATIONAL OPERATION ===\n";
        cout << "1. Age Group Categorization\n";
        cout << "2. Carbon Emission Analysis\n";
        cout << "3. Sorting Experiments\n";
        cout << "4. Performance Analysis\n";
        cout << "5. Ascend to Main Terminal\n";
        cout << "Directive: ";
        
        string input = getRawBuffer();
        stringstream ss(input);
        ss >> taskChoice;

        if (taskChoice == 5) return; 
        if (taskChoice >= 1 && taskChoice <= 4) executeCitySelection(dsMode, taskChoice, cityA, cityB, cityC);
        else cout << "\nplease select in range\n";
    }
}

int main() {
    Node* cityA = loadCityData("datasets/dataset1-cityA.csv");
    Node* cityB = loadCityData("datasets/dataset2-cityB.csv");
    Node* cityC = loadCityData("datasets/dataset3-cityC.csv");

    int dsChoice = 0;
    while (true) {
        cout << "\n======================================================\n";
        cout << " URBAN CARBON EMISSION ANALYSIS ENGINE - MAIN TERMINAL\n";
        cout << "======================================================\n";
        cout << "1. Array\n";
        cout << "2. Linked List\n";
        cout << "3. Quit\n";
        cout << "Architecture: ";
        
        string input = getRawBuffer();
        stringstream ss(input);
        ss >> dsChoice;

        if (dsChoice == 1 || dsChoice == 2) executeOperationSelection(dsChoice, cityA, cityB, cityC);
        else if (dsChoice == 3) break;
        else cout << "\nPlease select in range\n";
    }
    
    destroyList(cityA); destroyList(cityB); destroyList(cityC);
    return 0;
}