#include "array.h"
#include "linked_list.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <utility>
#include <cmath>

using namespace std;

std::string getRawBuffer();

int loadCSV(string filename, Resident arr[])
{
    ifstream file(filename);
    if (!file.is_open())
        return 0;

    string line, token;
    getline(file, line);
    int i = 0;
    while (getline(file, line) && i < MAX)
    {
        stringstream ss(line);
        getline(ss, arr[i].id, ',');
        getline(ss, token, ',');
        arr[i].age = stoi(token);
        getline(ss, arr[i].transport, ',');
        getline(ss, token, ',');
        arr[i].distance = stod(token);
        getline(ss, token, ',');
        arr[i].factor = stod(token);
        getline(ss, token, ',');
        token.erase(remove(token.begin(), token.end(), '\r'), token.end());
        arr[i].days = stoi(token);
        i++;
    }
    return i;
}

double calculateEmission(Resident r)
{
    return r.distance * r.factor * r.days;
}

inline int getTransportIndex_Array(string mode)
{
    if (mode == "Car")
        return 0;
    if (mode == "Bus")
        return 1;
    if (mode == "Bicycle")
        return 2;
    if (mode == "Walking")
        return 3;
    if (mode == "School Bus")
        return 4;
    if (mode == "Carpool")
        return 5;
    return -1;
}

inline string getTransportName_Array(int index)
{
    string modes[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    return modes[index];
}

void executeArrayAnalysis(Resident *cities[], int counts[], int numCities, string cityName)
{
    int transportCounts[5][6] = {0};
    double transportEmissions[5][6] = {0.0};
    double ageGroupTotalEmission[5] = {0.0};

    string ageLabels[] = {
        "6-17 (Children)",
        "18-25 (University Students)",
        "26-45 (Early Career)",
        "46-60 (Late Career)",
        "61-100 (Retirees)"};
    string modeNames[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};

    for (int c = 0; c < numCities; c++)
    {
        if (cities[c] == nullptr)
            continue;
        for (int i = 0; i < counts[c]; i++)
        {
            Resident r = cities[c][i];
            double emission = calculateEmission(r);

            int aGrp = -1;
            if (r.age >= 6 && r.age <= 17)
                aGrp = 0;
            else if (r.age <= 25)
                aGrp = 1;
            else if (r.age <= 45)
                aGrp = 2;
            else if (r.age <= 60)
                aGrp = 3;
            else if (r.age <= 100)
                aGrp = 4;

            int tIdx = getTransportIndex_Array(r.transport);

            if (aGrp != -1 && tIdx != -1)
            {
                transportCounts[aGrp][tIdx]++;
                transportEmissions[aGrp][tIdx] += emission;
                ageGroupTotalEmission[aGrp] += emission;
            }
        }
    }

    cout << "\n--- ANALYSIS FOR: " << cityName << " ---\n";

    for (int i = 0; i < 5; i++)
    {
        int groupTotalCount = 0;
        for (int j = 0; j < 6; j++)
            groupTotalCount += transportCounts[i][j];
        if (groupTotalCount == 0)
            continue;

        cout << "\nAge Group: " << ageLabels[i] << endl;
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(20) << "Mode of Transport"
             << setw(10) << "Count"
             << setw(25) << "Total Emission (kg CO2)"
             << "Average per Resident" << endl;

        for (int j = 0; j < 6; j++)
        {
            if (transportCounts[i][j] > 0)
            {
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

void executeArrayEmissionSubMenu(Resident *cities[], int counts[], int numCities)
{
    int filterChoice = 0;
    while (true)
    {
        cout << "\n--- ARRAY EMISSION FILTRATION ---\n";
        cout << "1. Filter by age group\n";
        cout << "2. Filter by transport mode\n";
        cout << "3. Back\n";
        cout << "Selection: ";

        string input = getRawBuffer();
        stringstream ss(input);
        ss >> filterChoice;

        if (filterChoice == 3)
            return;

        bool activeFlags[6] = {false};

        if (filterChoice == 1)
        {
            cout << "\nSelect Demographics (Space-separated, e.g., '1 4 5'):\n";
            cout << "1. 6-17\n2. 18-25\n3. 26-45\n4. 46-60\n5. 61-100\nTargets: ";
            string demoInput = getRawBuffer();
            stringstream dss(demoInput);
            int val;
            while (dss >> val)
            {
                if (val >= 1 && val <= 5)
                    activeFlags[val - 1] = true;
            }
            queryArrayEmissionByAge(cities, counts, numCities, activeFlags);
        }
        else if (filterChoice == 2)
        {
            cout << "\nSelect Transport (Space-separated, e.g., '1 2'):\n";
            cout << "1. Car\n2. Bus\n3. Bicycle\n4. Walking\n5. School Bus\n6. Carpool\nTargets: ";
            string transInput = getRawBuffer();
            stringstream tss(transInput);
            int val;
            while (tss >> val)
            {
                if (val >= 1 && val <= 6)
                    activeFlags[val - 1] = true;
            }
            queryArrayEmissionByTransport(cities, counts, numCities, activeFlags);
        }
    }
}

void queryArrayEmissionByAge(Resident *cities[], int counts[], int numCities, bool activeAges[5])
{
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
    string modeNames[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    double aggregateTotal = 0;

    for (int a = 0; a < 5; a++)
    {

        if (!activeAges[a])
            continue;

        int modeCounts[6] = {0};
        double modeEmissions[6] = {0.0};
        double ageTotal = 0;
        bool hasDataForThisGroup = false;

        for (int c = 0; c < numCities; c++)
        {
            if (cities[c] == nullptr)
                continue;
            for (int i = 0; i < counts[c]; i++)
            {
                Resident r = cities[c][i];

                int rAgeGrp = -1;
                if (r.age >= 6 && r.age <= 17)
                    rAgeGrp = 0;
                else if (r.age <= 25)
                    rAgeGrp = 1;
                else if (r.age <= 45)
                    rAgeGrp = 2;
                else if (r.age <= 60)
                    rAgeGrp = 3;
                else if (r.age >= 61)
                    rAgeGrp = 4;

                if (rAgeGrp == a)
                {
                    int tIdx = getTransportIndex_Array(r.transport);
                    if (tIdx != -1)
                    {
                        modeCounts[tIdx]++;
                        modeEmissions[tIdx] += calculateEmission(r);
                        hasDataForThisGroup = true;
                    }
                }
            }
        }

        cout << "\nDemographic: " << ageLabels[a] << endl;

        if (hasDataForThisGroup)
        {
            cout << "----------------------------------------------------------------------\n";
            cout << left << setw(20) << "Mode" << setw(10) << "Count"
                 << setw(25) << "Total Emission (kg CO2)" << "Average per Resident" << endl;

            for (int j = 0; j < 6; j++)
            {
                if (modeCounts[j] > 0)
                {
                    double avg = modeEmissions[j] / modeCounts[j];
                    ageTotal += modeEmissions[j];
                    cout << left << setw(20) << modeNames[j] << setw(10) << modeCounts[j]
                         << setw(25) << fixed << setprecision(2) << modeEmissions[j]
                         << fixed << setprecision(2) << avg << endl;
                }
            }
            cout << "----------------------------------------------------------------------\n";
            cout << "Total for Demographic: " << fixed << setprecision(2) << ageTotal << " kg CO2\n";
            aggregateTotal += ageTotal;
        }
        else
        {

            cout << ">>> No valid data found for this demographic in selected cities. <<<\n";
        }
    }
    cout << "\n>>> AGGREGATE FILTRATION EMISSION: " << fixed << setprecision(2) << aggregateTotal << " kg CO2 <<<\n";
}

void queryArrayEmissionByTransport(Resident *cities[], int counts[], int numCities, bool activeTransports[6])
{
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
    string modeNames[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    double aggregateTotal = 0;

    cout << "\n--- FILTERED EMISSION BY TRANSPORT ---\n";

    for (int t = 0; t < 6; t++)
    {
        if (!activeTransports[t])
            continue;

        double transportTotal = 0;
        int ageCounts[5] = {0};
        double ageEmissions[5] = {0.0};
        bool hasData = false;

        for (int c = 0; c < numCities; c++)
        {
            if (cities[c] == nullptr)
                continue;
            for (int i = 0; i < counts[c]; i++)
            {
                Resident r = cities[c][i];
                if (getTransportIndex_Array(r.transport) == t)
                {
                    int aGrp = -1;
                    if (r.age >= 6 && r.age <= 17)
                        aGrp = 0;
                    else if (r.age <= 25)
                        aGrp = 1;
                    else if (r.age <= 45)
                        aGrp = 2;
                    else if (r.age <= 60)
                        aGrp = 3;
                    else if (r.age <= 100)
                        aGrp = 4;

                    if (aGrp != -1)
                    {
                        ageCounts[aGrp]++;
                        ageEmissions[aGrp] += calculateEmission(r);
                        hasData = true;
                    }
                }
            }
        }

        if (hasData)
        {
            cout << "\nTransport Sector: " << modeNames[t] << endl;
            cout << "----------------------------------------------------------------------\n";
            cout << left << setw(20) << "Demographic" << setw(10) << "Count"
                 << setw(25) << "Total Emission (kg CO2)" << "Average per Resident" << endl;

            for (int i = 0; i < 5; i++)
            {
                if (ageCounts[i] > 0)
                {
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
// sortBy: 1 = Age, 2 = Distance, 3 = Carbon Emission, 4= Days, 5 = Transport
// Task 6: Bubble Sort
void bubbleSortArray(Resident arr[], int size, int sortBy)
{
    for (int i = 0; i < size - 1; i++)
    {
        for (int j = 0; j < size - i - 1; j++)
        {
            bool needToSwap = false;

            if (sortBy == 1 && arr[j].age > arr[j + 1].age)
                needToSwap = true;
            else if (sortBy == 2 && arr[j].distance > arr[j + 1].distance)
                needToSwap = true;
            else if (sortBy == 3 && arr[j].factor > arr[j + 1].factor)
                needToSwap = true; // Now uses raw factor!
            else if (sortBy == 4 && arr[j].days > arr[j + 1].days)
                needToSwap = true;
            else if (sortBy == 5 && arr[j].transport > arr[j + 1].transport)
                needToSwap = true;

            if (needToSwap)
            {
                Resident temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Task 6: Insertion Sort
void insertionSortArray(Resident arr[], int size, int sortBy)
{
    for (int i = 1; i < size; i++)
    {
        Resident key = arr[i];
        int j = i - 1;
        bool keepMoving = false;

        while (j >= 0)
        {
            keepMoving = false;
            if (sortBy == 1 && arr[j].age > key.age)
                keepMoving = true;
            else if (sortBy == 2 && arr[j].distance > key.distance)
                keepMoving = true;
            else if (sortBy == 3 && arr[j].factor > key.factor)
                keepMoving = true; // Now uses raw factor!
            else if (sortBy == 4 && arr[j].days > key.days)
                keepMoving = true;
            else if (sortBy == 5 && arr[j].transport > key.transport)
                keepMoving = true;

            if (keepMoving)
            {
                arr[j + 1] = arr[j];
                j = j - 1;
            }
            else
            {
                break;
            }
        }
        arr[j + 1] = key;
    }
}

// Task 7: Linear Search (Range-Based)
int linearSearchArray(Resident arr[], int size, int searchBy, double minVal, double maxVal, string targetStr)
{
    int matchCount = 0;

    for (int i = 0; i < size; i++)
    {
        bool match = false;

        if (searchBy == 1 && arr[i].age >= minVal && arr[i].age <= maxVal)
            match = true;
        else if (searchBy == 2 && arr[i].distance >= minVal && arr[i].distance <= maxVal)
            match = true;
        else if (searchBy == 3 && arr[i].factor >= minVal && arr[i].factor <= maxVal)
            match = true;
        else if (searchBy == 4 && arr[i].days >= minVal && arr[i].days <= maxVal)
            match = true;
        else if (searchBy == 5 && arr[i].transport == targetStr)
            match = true;

        if (match)
            matchCount++;
    }

    return matchCount;
}
//Task 7: BInary Search able for range based
int binarySearchArray(Resident arr[], int size, int searchBy, double minVal, double maxVal, string targetStr)
{
    int left = 0, right = size - 1;
    int anchor = -1;

    while (left <= right)
    {
        int mid = left + (right - left) / 2;
        bool match = false;
        bool goLeft = false;

        if (searchBy == 5)
        {
            if (arr[mid].transport == targetStr)
                match = true;
            else if (arr[mid].transport > targetStr)
                goLeft = true;
        }
        else
        {
            double val = 0;

            if (searchBy == 1)
                val = arr[mid].age;
            else if (searchBy == 2)
                val = arr[mid].distance;
            else if (searchBy == 3)
                val = arr[mid].factor;
            else if (searchBy == 4)
                val = arr[mid].days;

            if (val >= minVal && val <= maxVal)
                match = true;
            else if (val > maxVal)
                goLeft = true;
        }

        if (match)
        {
            anchor = mid;
            break;
        }
        else if (goLeft)
            right = mid - 1;
        else
            left = mid + 1;
    }

    if (anchor == -1)
        return 0;

    int first = anchor, last = anchor;

    while (first > 0)
    {
        bool ok = false;
        if (searchBy == 5 && arr[first - 1].transport == targetStr)
            ok = true;
        else if (searchBy == 1 && arr[first - 1].age >= minVal && arr[first - 1].age <= maxVal)
            ok = true;
        else if (searchBy == 2 && arr[first - 1].distance >= minVal && arr[first - 1].distance <= maxVal)
            ok = true;
        else if (searchBy == 3 && arr[first - 1].factor >= minVal && arr[first - 1].factor <= maxVal)
            ok = true;
        else if (searchBy == 4 && arr[first - 1].days >= minVal && arr[first - 1].days <= maxVal)
            ok = true;

        if (!ok)
            break;
        first--;
    }

    while (last < size - 1)
    {
        bool ok = false;
        if (searchBy == 5 && arr[last + 1].transport == targetStr)
            ok = true;
        else if (searchBy == 1 && arr[last + 1].age >= minVal && arr[last + 1].age <= maxVal)
            ok = true;
        else if (searchBy == 2 && arr[last + 1].distance >= minVal && arr[last + 1].distance <= maxVal)
            ok = true;
        else if (searchBy == 3 && arr[last + 1].factor >= minVal && arr[last + 1].factor <= maxVal)
            ok = true;
        else if (searchBy == 4 && arr[last + 1].days >= minVal && arr[last + 1].days <= maxVal)
            ok = true;

        if (!ok)
            break;
        last++;
    }

    return (last - first + 1);
}

void printArrayResults(Resident arr[], int size, int searchBy, double minVal, double maxVal, string targetStr)
{
    int count = 0;

    cout << "\n"
         << left << setw(15) << "Resident ID"
         << setw(10) << "Age"
         << setw(15) << "Transport"
         << setw(15) << "Distance"
         << setw(15) << "Factor"
         << setw(15) << "Days" << endl;

    cout << "----------------------------------------------------------------------------------\n";

    for (int i = 0; i < size; i++)
    {
        bool match = false;

        if (searchBy == 1 && arr[i].age >= minVal && arr[i].age <= maxVal)
            match = true;
        else if (searchBy == 2 && arr[i].distance >= minVal && arr[i].distance <= maxVal)
            match = true;
        else if (searchBy == 3 && arr[i].factor >= minVal && arr[i].factor <= maxVal)
            match = true;
        else if (searchBy == 4 && arr[i].days >= minVal && arr[i].days <= maxVal)
            match = true;
        else if (searchBy == 5 && arr[i].transport == targetStr)
            match = true;

        if (match)
        {
            cout << left << setw(15) << arr[i].id
                 << setw(10) << arr[i].age
                 << setw(15) << arr[i].transport
                 << setw(15) << arr[i].distance
                 << setw(15) << arr[i].factor
                 << setw(15) << arr[i].days << endl;

            count++;
        }
    }

    cout << "----------------------------------------------------------------------------------\n";
    cout << "Total Found: " << count << "\n";
}
//Task 7: Jump Search
int jumpSearchArray(Resident arr[], int size, int searchBy, double minVal, double maxVal, string targetStr)
{
    int step = (int)std::sqrt((double)size);
    int prev = 0;
    int count = 0;

    //  find the block where maxVal could exist
    while (prev < size)
    {
        int curr = min(prev + step, size) - 1;

        double val = 0.0;
        string sval = "";

        if (searchBy == 1)
            val = arr[curr].age;
        else if (searchBy == 2)
            val = arr[curr].distance;
        else if (searchBy == 3)
            val = arr[curr].factor;
        else if (searchBy == 4)
            val = arr[curr].days;
        else if (searchBy == 5)
            sval = arr[curr].transport;

        bool overshot = (searchBy == 5) ? (sval > targetStr) : (val > maxVal);
        if (overshot)
            break;

        prev += step;
    }

    // Step 3: Linear scan backwards from the jump point
    int start = max(0, prev - step);
    int end = min(prev, size - 1);

    for (int i = start; i <= end; i++)
    {
        bool match = false;

        if (searchBy == 1 && arr[i].age >= minVal && arr[i].age <= maxVal)
            match = true;
        else if (searchBy == 2 && arr[i].distance >= minVal && arr[i].distance <= maxVal)
            match = true;
        else if (searchBy == 3 && arr[i].factor >= minVal && arr[i].factor <= maxVal)
            match = true;
        else if (searchBy == 4 && arr[i].days >= minVal && arr[i].days <= maxVal)
            match = true;
        else if (searchBy == 5 && arr[i].transport == targetStr)
            match = true;

        if (match)
            count++;
    }

    return count;
}

void executeInsightsReport(Node *cityA, Node *cityB, Node *cityC)
{
    Node *cities[] = {cityA, cityB, cityC};
    string cityNames[] = {"City A", "City B", "City C"};
    string ageLabels[] = {"6-17", "18-25", "26-45", "46-60", "61-100"};
    string modeNames[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};

    // Aggregation arrays
    double cityAgeEmission[3][5] = {};
    int cityAgeCount[3][5] = {};
    int cityModeCount[3][6] = {};
    double totalAgeEmission[5] = {};
    int totalAgeCount[5] = {};
    int ageTransportCount[5][6] = {};

    for (int c = 0; c < 3; c++)
    {
        Node *cur = cities[c];
        while (cur != nullptr)
        {
            int aGroup = -1;
            if (cur->data.age >= 6 && cur->data.age <= 17)
                aGroup = 0;
            else if (cur->data.age >= 18 && cur->data.age <= 25)
                aGroup = 1;
            else if (cur->data.age >= 26 && cur->data.age <= 45)
                aGroup = 2;
            else if (cur->data.age >= 46 && cur->data.age <= 60)
                aGroup = 3;
            else if (cur->data.age >= 61 && cur->data.age <= 100)
                aGroup = 4;

            int tIdx = getTransportIndex(cur->data.transportMode);
            double emission = cur->data.dailyDistance * cur->data.emissionFactor * cur->data.avgDaysPerMonth;

            if (aGroup != -1)
            {
                cityAgeEmission[c][aGroup] += emission;
                cityAgeCount[c][aGroup]++;
                totalAgeEmission[aGroup] += emission;
                totalAgeCount[aGroup]++;

                if (tIdx != -1)
                {
                    cityModeCount[c][tIdx]++;
                    ageTransportCount[aGroup][tIdx]++;
                }
            }
            cur = cur->next;
        }
    }

    // TABLE 1 (9a): Avg carbon emission per age group, across cities
    cout << "\n========== TASK 9a: CARBON EMISSION COMPARISON BY AGE GROUP ==========\n";
    cout << "(Average monthly emission per resident)\n\n";
    cout << left
         << setw(10) << "Age Group"
         << setw(18) << "City A (avg)"
         << setw(18) << "City B (avg)"
         << setw(18) << "City C (avg)"
         << setw(18) << "Overall (avg)" << "\n";
    cout << string(82, '-') << "\n";

    for (int i = 0; i < 5; i++)
    {
        double overallAvg = totalAgeCount[i] > 0 ? totalAgeEmission[i] / totalAgeCount[i] : 0.0;
        cout << left << setw(10) << ageLabels[i];
        for (int c = 0; c < 3; c++)
        {
            double avg = cityAgeCount[c][i] > 0 ? cityAgeEmission[c][i] / cityAgeCount[c][i] : 0.0;
            cout << setw(18) << fixed << setprecision(2) << avg;
        }
        cout << setw(18) << fixed << setprecision(2) << overallAvg << "\n";
    }

    // TABLE 2 (9a): Transport mode preference by city
    cout << "\n========== TASK 9a: TRANSPORT MODE PREFERENCE BY CITY ==========\n";
    cout << "(Number of residents using each mode)\n\n";
    cout << left
         << setw(14) << "Mode"
         << setw(12) << "City A"
         << setw(12) << "City B"
         << setw(12) << "City C"
         << setw(10) << "Total" << "\n";
    cout << string(60, '-') << "\n";

    for (int j = 0; j < 6; j++)
    {
        int total = cityModeCount[0][j] + cityModeCount[1][j] + cityModeCount[2][j];
        if (total == 0)
            continue;
        cout << left
             << setw(14) << modeNames[j]
             << setw(12) << cityModeCount[0][j]
             << setw(12) << cityModeCount[1][j]
             << setw(12) << cityModeCount[2][j]
             << setw(10) << total << "\n";
    }

    // TABLE 3 (9b): Age group emission ranking (all cities combined)
    cout << "\n========== TASK 9b: AGE GROUP EMISSION RANKING FOR ALL CITIES ==========\n";
    double grandTotal = 0;
    for (int i = 0; i < 5; i++)
        grandTotal += totalAgeEmission[i];

    // Sort indices by descending total emission
    int rank[5] = {0, 1, 2, 3, 4};
    for (int i = 0; i < 4; i++)
        for (int j = i + 1; j < 5; j++)
            if (totalAgeEmission[rank[j]] > totalAgeEmission[rank[i]])
            {
                int tmp = rank[i];
                rank[i] = rank[j];
                rank[j] = tmp;
            }

    cout << left
         << setw(6) << "Rank"
         << setw(10) << "Age Group"
         << setw(10) << "Count"
         << setw(24) << "Total Emission (kg CO2)"
         << setw(20) << "Avg per Resident"
         << "% of Total\n";
    cout << string(80, '-') << "\n";

    for (int r = 0; r < 5; r++)
    {
        int i = rank[r];
        if (totalAgeCount[i] == 0)
            continue;
        double avg = totalAgeEmission[i] / totalAgeCount[i];
        double pct = grandTotal > 0 ? (totalAgeEmission[i] / grandTotal * 100.0) : 0.0;

        stringstream pctCell;
        pctCell << fixed << setprecision(1) << pct << "%";

        cout << left
             << setw(6) << (r + 1)
             << setw(10) << ageLabels[i]
             << setw(10) << totalAgeCount[i]
             << setw(24) << fixed << setprecision(2) << totalAgeEmission[i]
             << setw(20) << fixed << setprecision(2) << avg
             << pctCell.str() << "\n";
    }
    cout << string(80, '-') << "\n";
    cout << "Highest emitting group: " << ageLabels[rank[0]] << "\n";

    // TABLE 4 (9b): Car vs Bicycle usage by age group
    cout << "\n========== TASK 9b: CAR vs BICYCLE USAGE BY AGE GROUP ==========\n";
    cout << left
         << setw(10) << "Age Group"
         << setw(10) << "Total"
         << setw(8) << "Car"
         << setw(10) << "Car %"
         << setw(10) << "Bicycle"
         << "Bicycle %\n";
    cout << string(58, '-') << "\n";

    int highestCarAge = 0, highestBikeAge = 0;
    double highestCarPct = -1, highestBikePct = -1;

    for (int i = 0; i < 5; i++)
    {
        if (totalAgeCount[i] == 0)
            continue;
        int carCount = ageTransportCount[i][0];
        int bikeCount = ageTransportCount[i][2];
        double carPct = carCount * 100.0 / totalAgeCount[i];
        double bikePct = bikeCount * 100.0 / totalAgeCount[i];

        if (carPct > highestCarPct)
        {
            highestCarPct = carPct;
            highestCarAge = i;
        }
        if (bikePct > highestBikePct)
        {
            highestBikePct = bikePct;
            highestBikeAge = i;
        }

        stringstream carCell, bikeCell;
        carCell << fixed << setprecision(1) << carPct << "%";
        bikeCell << fixed << setprecision(1) << bikePct << "%";

        cout << left
             << setw(10) << ageLabels[i]
             << setw(10) << totalAgeCount[i]
             << setw(8) << carCount
             << setw(10) << carCell.str()
             << setw(10) << bikeCount
             << bikeCell.str() << "\n";
    }
    cout << string(58, '-') << "\n";
    cout << "Highest car usage:     " << ageLabels[highestCarAge]
         << " (" << fixed << setprecision(1) << highestCarPct << "%)\n";
    cout << "Highest bicycle usage: " << ageLabels[highestBikeAge]
         << " (" << fixed << setprecision(1) << highestBikePct << "%)\n";

    cout << "\nNote: Practical recommendations for city planners based on the above\n";
    cout << "patterns (Task 9c) are discussed in the project documentation.\n";
    cout << string(66, '=') << "\n";
}
