#include "array.h"
#include "linked_list.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace std;

int loadCSV(string filename, Resident arr[]) {
    ifstream file(filename);
    if (!file.is_open()) return 0;

    string line, token;
    getline(file, line); 
    int i = 0;
    while (getline(file, line) && i < MAX) {
        stringstream ss(line);
        getline(ss, arr[i].id, ',');
        getline(ss, token, ','); arr[i].age = stoi(token);
        getline(ss, arr[i].transport, ',');
        getline(ss, token, ','); arr[i].distance = stod(token);
        getline(ss, token, ','); arr[i].factor = stod(token);
        getline(ss, token, ','); 
        token.erase(remove(token.begin(), token.end(), '\r'), token.end());
        arr[i].days = stoi(token);
        i++;
    }
    return i;
}

double calculateEmission(Resident r) {
    return r.distance * r.factor * r.days;
}

// inline int getTransportIndex_Array(string mode) {
//     if (mode == "Car") return 0;
//     if (mode == "Bus") return 1;
//     if (mode == "Bicycle") return 2;
//     if (mode == "Walking") return 3;
//     if (mode == "School Bus") return 4;
//     if (mode == "Carpool") return 5;
//     return -1;
// }

// inline string getTransportName_Array(int index) {
//     string modes[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
//     return modes[index];
// }

void executeArrayAnalysis(Resident* cities[], int counts[], int numCities, string cityName) {
    int ageGroupCounts[5] = {0};
    double ageGroupEmissions[5] = {0.0};
    int transportFreq[5][6] = {0}; 
    double modeEmissions[6] = {0.0};
    double grandTotal = 0.0;

    for (int c = 0; c < numCities; c++) {
        if (cities[c] == nullptr) continue;
        for (int i = 0; i < counts[c]; i++) {
            Resident r = cities[c][i];
            double emission = calculateEmission(r);
            grandTotal += emission;

            int aGrp = -1;
            if (r.age >= 6 && r.age <= 17) aGrp = 0;
            else if (r.age <= 25) aGrp = 1;
            else if (r.age <= 45) aGrp = 2;
            else if (r.age <= 60) aGrp = 3;
            else if (r.age <= 100) aGrp = 4;

            if (aGrp != -1) {
                ageGroupCounts[aGrp]++;
                ageGroupEmissions[aGrp] += emission;
                int tIdx = getTransportIndex(r.transport);
                if (tIdx != -1) {
                    transportFreq[aGrp][tIdx]++;
                    modeEmissions[tIdx] += emission;
                }
            }
        }
    }

    string labels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
    cout << "\n--- ANALYSIS FOR: " << cityName << " ---\n";
    cout << left << setw(15) << "Age Group" << setw(15) << "Count" << setw(20) << "Preferred" << setw(20) << "Total (kg)" << "Avg (kg)\n";
    
    for (int i = 0; i < 5; i++) {
        if (ageGroupCounts[i] == 0) continue;
        int prefIdx = 0, maxF = -1;
        for (int j = 0; j < 6; j++) {
            if (transportFreq[i][j] > maxF) { maxF = transportFreq[i][j]; prefIdx = j; }
        }
        cout << left << setw(15) << labels[i] << setw(15) << ageGroupCounts[i] 
             << setw(20) << getTransportName(prefIdx) << setw(20) << fixed << setprecision(2) << ageGroupEmissions[i] 
             << (ageGroupEmissions[i]/ageGroupCounts[i]) << endl;
    }
    cout << "\nTotal Dataset Emission: " << grandTotal << " kg CO2\n";
}