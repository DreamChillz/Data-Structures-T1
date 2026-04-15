#include "array.h"
#include "linked_list.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <utility>


using namespace std;

std::string getRawBuffer();

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
    int transportCounts[5][6] = {0};
    double transportEmissions[5][6] = {0.0};
    double ageGroupTotalEmission[5] = {0.0};

    string ageLabels[] = {
        "6-17 (Children)", 
        "18-25 (University Students)", 
        "26-45 (Early Career)", 
        "46-60 (Late Career)", 
        "61-100 (Retirees)"
    };
    string modeNames[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};

    // 1. Data Aggregation
    for (int c = 0; c < numCities; c++) {
        if (cities[c] == nullptr) continue;
        for (int i = 0; i < counts[c]; i++) {
            Resident r = cities[c][i];
            double emission = calculateEmission(r);

            int aGrp = -1;
            if (r.age >= 6 && r.age <= 17) aGrp = 0;
            else if (r.age <= 25) aGrp = 1;
            else if (r.age <= 45) aGrp = 2;
            else if (r.age <= 60) aGrp = 3;
            else if (r.age <= 100) aGrp = 4;

            int tIdx = getTransportIndex_Array(r.transport); // Fixed function name

            if (aGrp != -1 && tIdx != -1) {
                transportCounts[aGrp][tIdx]++;
                transportEmissions[aGrp][tIdx] += emission;
                ageGroupTotalEmission[aGrp] += emission;
            }
        }
    }

    // 2. Table Rendering (Matches Sample Image)
    cout << "\n--- ANALYSIS FOR: " << cityName << " ---\n";

    for (int i = 0; i < 5; i++) {
        int groupTotalCount = 0;
        for(int j=0; j<6; j++) groupTotalCount += transportCounts[i][j];
        if (groupTotalCount == 0) continue;

        cout << "\nAge Group: " << ageLabels[i] << endl;
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(20) << "Mode of Transport" 
             << setw(10) << "Count" 
             << setw(25) << "Total Emission (kg CO2)" 
             << "Average per Resident" << endl;

        for (int j = 0; j < 6; j++) {
            if (transportCounts[i][j] > 0) {
                double avg = transportEmissions[i][j] / transportCounts[i][j];
                cout << left << setw(20) << modeNames[j] 
                     << setw(10) << transportCounts[i][j] 
                     << setw(25) << fixed << setprecision(2) << transportEmissions[i][j] 
                     << fixed << setprecision(1) << avg << endl;
            }
        }
        cout << "----------------------------------------------------------------------\n";
        cout << "Total Emission for Age Group: " << (int)ageGroupTotalEmission[i] << " kg CO2\n";
    }
}

void executeArrayEmissionSubMenu(Resident* cities[], int counts[], int numCities) {
    int filterChoice = 0;
    while (true) {
        cout << "\n--- ARRAY EMISSION FILTRATION ---\n";
        cout << "1. Filter by age group\n";
        cout << "2. Filter by transport mode\n";
        cout << "3. Back\n";
        cout << "Selection: ";

        string input = getRawBuffer();
        stringstream ss(input);
        ss >> filterChoice;

        if (filterChoice == 3) return;

        bool activeFlags[6] = {false}; 

        if (filterChoice == 1) {
            cout << "\nSelect Demographics (Space-separated, e.g., '1 4 5'):\n";
            cout << "1. 6-17\n2. 18-25\n3. 26-45\n4. 46-60\n5. 61-100\nTargets: ";
            string demoInput = getRawBuffer();
            stringstream dss(demoInput);
            int val;
            while (dss >> val) { if (val >= 1 && val <= 5) activeFlags[val - 1] = true; }
            queryArrayEmissionByAge(cities, counts, numCities, activeFlags);
        } 
        else if (filterChoice == 2) {
                cout << "\nSelect Transport (Space-separated, e.g., '1 2'):\n";
                cout << "1. Car\n2. Bus\n3. Bicycle\n4. Walking\n5. School Bus\n6. Carpool\nTargets: ";
            string transInput = getRawBuffer();
            stringstream tss(transInput);
            int val;
            while (tss >> val) { if (val >= 1 && val <= 6) activeFlags[val - 1] = true; }
            queryArrayEmissionByTransport(cities, counts, numCities, activeFlags);
        }
    }
}

void queryArrayEmissionByAge(Resident* cities[], int counts[], int numCities, bool activeAges[5]) {
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
    string modeNames[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    double aggregateTotal = 0;

    for (int a = 0; a < 5; a++) {
        if (!activeAges[a]) continue;

        int modeCounts[6] = {0};
        double modeEmissions[6] = {0.0};
        double ageTotal = 0;
        bool hasData = false;

        for (int c = 0; c < numCities; c++) {
            for (int i = 0; i < counts[c]; i++) {
                Resident r = cities[c][i];
                int rAgeGrp = (r.age >= 6 && r.age <= 17) ? 0 : (r.age <= 25) ? 1 : (r.age <= 45) ? 2 : (r.age <= 60) ? 3 : 4;

                if (rAgeGrp == a) {
                    int tIdx = getTransportIndex_Array(r.transport);
                    if (tIdx != -1) {
                        modeCounts[tIdx]++;
                        modeEmissions[tIdx] += calculateEmission(r);
                        hasData = true;
                    }
                }
            }
        }

        if (hasData) {
            cout << "\nDemographic: " << ageLabels[a] << endl;
            cout << "----------------------------------------------------------------------\n";
            cout << left << setw(20) << "Mode" << setw(10) << "Count" 
                 << setw(25) << "Total Emission (kg CO2)" << "Average per Resident" << endl;

            for (int j = 0; j < 6; j++) {
                if (modeCounts[j] > 0) {
                    double avg = modeEmissions[j] / modeCounts[j];
                    ageTotal += modeEmissions[j];
                    cout << left << setw(20) << modeNames[j] << setw(10) << modeCounts[j] 
                         << setw(25) << fixed << setprecision(2) << modeEmissions[j] << avg << endl;
                }
            }
            cout << "----------------------------------------------------------------------\n";
            cout << "Total for Demographic: " << ageTotal << " kg CO2\n";
            aggregateTotal += ageTotal;
        }
    }
    cout << "\n>>> AGGREGATE FILTRATION EMISSION: " << aggregateTotal << " kg CO2 <<<\n";
}

void queryArrayEmissionByTransport(Resident* cities[], int counts[], int numCities, bool activeTransports[6]) {
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
    string modeNames[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    double aggregateTotal = 0;

    cout << "\n--- FILTERED EMISSION BY TRANSPORT ---\n";

    for (int t = 0; t < 6; t++) {
        if (!activeTransports[t]) continue;

        double transportTotal = 0;
        int ageCounts[5] = {0};
        double ageEmissions[5] = {0.0};
        bool hasData = false;

        // Collect data for this specific transport mode
        for (int c = 0; c < numCities; c++) {
            if (cities[c] == nullptr) continue;
            for (int i = 0; i < counts[c]; i++) {
                Resident r = cities[c][i];
                if (getTransportIndex_Array(r.transport) == t) {
                    int aGrp = -1;
                    if (r.age >= 6 && r.age <= 17) aGrp = 0;
                    else if (r.age <= 25) aGrp = 1;
                    else if (r.age <= 45) aGrp = 2;
                    else if (r.age <= 60) aGrp = 3;
                    else if (r.age <= 100) aGrp = 4;

                    if (aGrp != -1) {
                        ageCounts[aGrp]++;
                        ageEmissions[aGrp] += calculateEmission(r);
                        hasData = true;
                    }
                }
            }
        }

        if (hasData) {
            cout << "\nTransport Sector: " << modeNames[t] << endl;
            cout << "----------------------------------------------------------------------\n";
            cout << left << setw(20) << "Demographic" << setw(10) << "Count" 
                 << setw(25) << "Total Emission (kg CO2)" << "Average per Resident" << endl;

            for (int i = 0; i < 5; i++) {
                if (ageCounts[i] > 0) {
                    double avg = ageEmissions[i] / ageCounts[i];
                    transportTotal += ageEmissions[i];
                    cout << left << setw(20) << ageLabels[i] 
                         << setw(10) << ageCounts[i] 
                         << setw(25) << fixed << setprecision(2) << ageEmissions[i] 
                         << fixed << setprecision(2) << avg << endl;
                }
            }
            cout << "----------------------------------------------------------------------\n";
            cout << "Total Emission for Transport Sector: " << transportTotal << " kg CO2\n";
            aggregateTotal += transportTotal;
        }
    }

    cout << "\n>>> AGGREGATE FILTRATION EMISSION: " << fixed << setprecision(2) << aggregateTotal << " kg CO2 <<<\n";
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