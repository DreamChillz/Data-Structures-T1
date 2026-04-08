#include "array.h"
#include "linked_list.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <utility>


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

inline int getTransportIndex_Array(string mode) {
     if (mode == "Car") return 0;
     if (mode == "Bus") return 1;
    if (mode == "Bicycle") return 2;
     if (mode == "Walking") return 3;
     if (mode == "School Bus") return 4;
     if (mode == "Carpool") return 5;
     return -1;
}

inline string getTransportName_Array(int index) {
     string modes[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
     return modes[index];
 }

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

// Task 6 bubble sort
// sortBy: 1 = Age, 2 = Distance, 3 = Carbon Emission
// Task 6: Bubble Sort
void bubbleSortArray(Resident arr[], int size, int sortBy) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            bool needToSwap = false;

            if (sortBy == 1 && arr[j].age > arr[j + 1].age) needToSwap = true;
            else if (sortBy == 2 && arr[j].distance > arr[j + 1].distance) needToSwap = true;
            else if (sortBy == 3 && arr[j].factor > arr[j + 1].factor) needToSwap = true; // Now uses raw factor!
            else if (sortBy == 4 && arr[j].days > arr[j + 1].days) needToSwap = true;
            else if (sortBy == 5 && arr[j].transport > arr[j + 1].transport) needToSwap = true;

            if (needToSwap) {
                Resident temp = arr[j];
                arr[j] = arr[j+1];
                arr[j+1] = temp;
            }
        }
    }
}

// Task 6: Insertion Sort
void insertionSortArray(Resident arr[], int size, int sortBy) {
    for (int i = 1; i < size; i++) {
        Resident key = arr[i];
        int j = i - 1;
        bool keepMoving = false;

        while (j >= 0) {
            keepMoving = false;
            if (sortBy == 1 && arr[j].age > key.age) keepMoving = true;
            else if (sortBy == 2 && arr[j].distance > key.distance) keepMoving = true;
            else if (sortBy == 3 && arr[j].factor > key.factor) keepMoving = true; // Now uses raw factor!
            else if (sortBy == 4 && arr[j].days > key.days) keepMoving = true;
            else if (sortBy == 5 && arr[j].transport > key.transport) keepMoving = true;

            if (keepMoving) {
                arr[j + 1] = arr[j];
                j = j - 1;
            } else {
                break; 
            }
        }
        arr[j + 1] = key;
    }
}

// Task 7: Linear Search (Range-Based)
void linearSearchArray(Resident arr[], int size, int searchBy, double minVal, double maxVal, string targetStr) {
    int matchCount = 0;
    cout << "\n" << left << setw(15) << "Resident ID" << setw(10) << "Age" 
         << setw(15) << "Transport" << setw(15) << "Distance" 
         << setw(15) << "Factor" << setw(15) << "Days" << endl;
    cout << "----------------------------------------------------------------------------------\n";

    for (int i = 0; i < size; i++) {
        bool isMatch = false;

        if (searchBy == 1 && arr[i].age >= minVal && arr[i].age <= maxVal) isMatch = true;
        else if (searchBy == 2 && arr[i].distance >= minVal && arr[i].distance <= maxVal) isMatch = true;
        else if (searchBy == 3 && arr[i].factor >= minVal && arr[i].factor <= maxVal) isMatch = true;
        else if (searchBy == 4 && arr[i].days >= minVal && arr[i].days <= maxVal) isMatch = true;
        else if (searchBy == 5 && arr[i].transport == targetStr) isMatch = true;

        if (isMatch) {
            cout << left << setw(15) << arr[i].id << setw(10) << arr[i].age 
                 << setw(15) << arr[i].transport << setw(15) << arr[i].distance 
                 << setw(15) << arr[i].factor << setw(15) << arr[i].days << endl;
            matchCount++;
        }
    }
    cout << "----------------------------------------------------------------------------------\n";
    cout << "Total Found: " << matchCount << "\n";
}

// Task 7: Binary Search (Range-Based with Total Count)
int binarySearchArray(Resident arr[], int size, int searchBy, double minVal, double maxVal, string targetStr) {
    int left = 0;
    int right = size - 1;
    int anchorIndex = -1;

    // STEP 1: Fast Binary Search to find AT LEAST ONE match
    while (left <= right) {
        int mid = left + (right - left) / 2;
        bool isMatch = false;
        bool goLeft = false;

        if (searchBy == 5) { // String exact match
            if (arr[mid].transport == targetStr) isMatch = true;
            else if (arr[mid].transport > targetStr) goLeft = true;
        } else { // Numeric Range match
            double midValue = 0.0;
            if (searchBy == 1) midValue = arr[mid].age;
            else if (searchBy == 2) midValue = arr[mid].distance;
            else if (searchBy == 3) midValue = arr[mid].factor;
            else if (searchBy == 4) midValue = arr[mid].days;

            if (midValue >= minVal && midValue <= maxVal) isMatch = true;
            else if (midValue > maxVal) goLeft = true;
        }

        if (isMatch) {
            anchorIndex = mid; // We found one! Save its location.
            break;             // Stop jumping around!
        } else if (goLeft) {
            right = mid - 1;
        } else {
            left = mid + 1;
        }
    }

    // STEP 2: If we didn't find anything, just return 0
    if (anchorIndex == -1) return 0;

    // STEP 3: Expand Outward (Scan Left and Right to find the rest)
    int firstMatch = anchorIndex;
    int lastMatch = anchorIndex;

    // Walk Left to find where the block of matches starts
    while (firstMatch > 0) {
        bool stillMatch = false;
        if (searchBy == 5 && arr[firstMatch - 1].transport == targetStr) stillMatch = true;
        else if (searchBy == 1 && arr[firstMatch - 1].age >= minVal && arr[firstMatch - 1].age <= maxVal) stillMatch = true;
        else if (searchBy == 2 && arr[firstMatch - 1].distance >= minVal && arr[firstMatch - 1].distance <= maxVal) stillMatch = true;
        else if (searchBy == 3 && arr[firstMatch - 1].factor >= minVal && arr[firstMatch - 1].factor <= maxVal) stillMatch = true;
        else if (searchBy == 4 && arr[firstMatch - 1].days >= minVal && arr[firstMatch - 1].days <= maxVal) stillMatch = true;
        
        if (!stillMatch) break; // We hit a non-matching record, stop walking left.
        firstMatch--;
    }

    // Walk Right to find where the block of matches ends
    while (lastMatch < size - 1) {
        bool stillMatch = false;
        if (searchBy == 5 && arr[lastMatch + 1].transport == targetStr) stillMatch = true;
        else if (searchBy == 1 && arr[lastMatch + 1].age >= minVal && arr[lastMatch + 1].age <= maxVal) stillMatch = true;
        else if (searchBy == 2 && arr[lastMatch + 1].distance >= minVal && arr[lastMatch + 1].distance <= maxVal) stillMatch = true;
        else if (searchBy == 3 && arr[lastMatch + 1].factor >= minVal && arr[lastMatch + 1].factor <= maxVal) stillMatch = true;
        else if (searchBy == 4 && arr[lastMatch + 1].days >= minVal && arr[lastMatch + 1].days <= maxVal) stillMatch = true;
        
        if (!stillMatch) break; // We hit a non-matching record, stop walking right.
        lastMatch++;
    }

    // STEP 4: Print all the matches we found!
    int matchCount = (lastMatch - firstMatch) + 1;
    
    cout << "\n" << left << setw(15) << "Resident ID" << setw(10) << "Age" 
         << setw(15) << "Transport" << setw(15) << "Distance" 
         << setw(15) << "Factor" << setw(15) << "Days" << endl;
    cout << "----------------------------------------------------------------------------------\n";
    
    for (int i = firstMatch; i <= lastMatch; i++) {
        cout << left << setw(15) << arr[i].id << setw(10) << arr[i].age 
             << setw(15) << arr[i].transport << setw(15) << arr[i].distance 
             << setw(15) << arr[i].factor << setw(15) << arr[i].days << endl;
    }
    
    cout << "----------------------------------------------------------------------------------\n";
    cout << "Total Found: " << matchCount << "\n";

    return matchCount; 
}