#include <iostream>
#include <string>
#include <sstream>
#include <chrono>  // Added for Time Complexity
#include <iomanip> // Added for Text Tables
#include <cstdlib> // for rand/srand
#include "linked_list.h"
#include "array.h"

using namespace std;
using namespace std::chrono;

string getRawBuffer()
{
    string line;
    getline(cin, line);
    if (line.empty())
        getline(cin, line);
    return line;
}

const int BENCH_RUNS = 7; // 1 warm-up + 6 timed; trimmedAvg drops min/max of the 6

// Shuffle with a fixed seed so every run starts from the same randomised ordering.
void shuffleArray(Resident arr[], int n)
{
    srand(42);
    for (int i = n - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        swap(arr[i], arr[j]);
    }
}

// Drop min and max of times[0..n-1], return average of the rest.
long long trimmedAvg(long long times[], int n)
{
    long long sorted[10];
    for (int i = 0; i < n; i++)
        sorted[i] = times[i];
    for (int i = 0; i < n - 1; i++)
        for (int j = i + 1; j < n; j++)
            if (sorted[j] < sorted[i])
                swap(sorted[i], sorted[j]);
    if (n <= 2)
        return sorted[0];
    long long sum = 0;
    for (int i = 1; i < n - 1; i++)
        sum += sorted[i];
    return sum / (n - 2);
}

struct BenchResult
{
    long long arrayTime;
    long long llTime;
};

// Run one sort method BENCH_RUNS times. Shuffles the array and rebuilds the
// linked list each run so both structures always start from an unsorted state.
// sortMethod: 1 = bubble, 2 = insertion  |  sortBy: field index (1-5)
BenchResult benchmarkSort(Resident combinedArr[], Node *cityA, Node *cityB, Node *cityC,
                          int totalSize, int sortMethod, int sortBy)
{
    long long arrRuns[BENCH_RUNS], llRuns[BENCH_RUNS];
    Resident workingArr[600];

    for (int r = 0; r < BENCH_RUNS; r++)
    {
        for (int i = 0; i < totalSize; i++)
            workingArr[i] = combinedArr[i];
        shuffleArray(workingArr, totalSize);
        Node *llHead = buildMergedList(cityA, cityB, cityC);

        auto sA = high_resolution_clock::now();
        if (sortMethod == 1)
            bubbleSortArray(workingArr, totalSize, sortBy);
        else
            insertionSortArray(workingArr, totalSize, sortBy);
        arrRuns[r] = duration_cast<microseconds>(high_resolution_clock::now() - sA).count();

        auto sL = high_resolution_clock::now();
        if (sortMethod == 1)
            bubbleSortLinkedList(llHead, sortBy);
        else
            insertionSortLinkedList(llHead, sortBy);
        llRuns[r] = duration_cast<microseconds>(high_resolution_clock::now() - sL).count();

        destroyList(llHead);
    }

    return {trimmedAvg(arrRuns + 1, BENCH_RUNS - 1), trimmedAvg(llRuns + 1, BENCH_RUNS - 1)};
}

// Run one search method BENCH_RUNS times. Pre-sorts data before the timing loop
// when required; sorting time is excluded from the result.
// searchMethod: 1 = linear, 2 = binary, 3 = jump
BenchResult benchmarkSearch(Resident combinedArr[], Node *cityA, Node *cityB, Node *cityC,
                            int totalSize, int searchMethod, int searchBy,
                            double minVal, double maxVal, string targetStr)
{
    long long arrRuns[BENCH_RUNS], llRuns[BENCH_RUNS];
    Resident workingArr[600];
    for (int i = 0; i < totalSize; i++)
        workingArr[i] = combinedArr[i];
    Node *llHead = buildMergedList(cityA, cityB, cityC);

    if (searchMethod == 2 || searchMethod == 3)
        insertionSortArray(workingArr, totalSize, searchBy);

    if (searchMethod == 2 || searchMethod == 3)
        insertionSortLinkedList(llHead, searchBy);

    for (int r = 0; r < BENCH_RUNS; r++)
    {
        auto sA = high_resolution_clock::now();
        if (searchMethod == 1)
            linearSearchArray(workingArr, totalSize, searchBy, minVal, maxVal, targetStr);
        else if (searchMethod == 2)
            binarySearchArray(workingArr, totalSize, searchBy, minVal, maxVal, targetStr);
        else
            jumpSearchArray(workingArr, totalSize, searchBy, minVal, maxVal, targetStr);

        arrRuns[r] = duration_cast<microseconds>(high_resolution_clock::now() - sA).count();

        auto sL = high_resolution_clock::now();
        if (searchMethod == 1)
            linearSearchLinkedList(llHead, searchBy, minVal, maxVal, targetStr);
        else if (searchMethod == 2)
            binarySearchLinkedList(llHead, searchBy, minVal, maxVal, targetStr);
        else if (searchMethod == 3)
            jumpSearchLinkedList(llHead, searchBy, minVal, maxVal, targetStr);

        llRuns[r] = duration_cast<microseconds>(high_resolution_clock::now() - sL).count();
    }

    destroyList(llHead);

    long long llAvg = trimmedAvg(llRuns + 1, BENCH_RUNS - 1);
    return {trimmedAvg(arrRuns + 1, BENCH_RUNS - 1), llAvg};
}

void executeEmissionSubMenu(Node *cities[], string cityNames[])
{
    int filterChoice = 0;
    while (true)
    {
        cout << "\nCARBON EMISSION FILTRATION\n";
        cout << "1. Filter by age\n";
        cout << "2. Filter by transport mode\n";
        cout << "3. Age vs Carbon\n";
        cout << "4. Transport vs Carbon\n";
        cout << "5. Age vs Transport\n";
        cout << "6. Back\n";

        string input = getRawBuffer();
        stringstream ss(input);
        ss >> filterChoice;

        if (filterChoice == 6)
            return;

        if (filterChoice == 3)
        {
            executeCrossDatasetComparison(cities[0], cities[1], cities[2]);
            continue;
        }
        if (filterChoice == 4)
        {
            executeCrossDatasetTransportComparison(cities[0], cities[1], cities[2]);
            continue;
        }
        if (filterChoice == 5)
        {
            executeAgeVsTransportComparison(cities[0], cities[1], cities[2]);
            continue;
        }

        if (filterChoice == 1 || filterChoice == 2)
        {
            bool activeFlags[6] = {false};

            if (filterChoice == 1)
            {
                cout << "\nSelect Demographics (Space-separated, e.g., '1 4 5'):\n";
                cout << "1. 6-17\n2. 18-25\n3. 26-45\n4. 46-60\n5. 61-100\nTargets: ";
                string demoInput = getRawBuffer();
                stringstream dss(demoInput);
                int val;
                while (dss >> val)
                    if (val >= 1 && val <= 5)
                        activeFlags[val - 1] = true;
            }
            else
            {
                cout << "\nSelect Transport (Space-separated, e.g., '1 2'):\n";
                cout << "1. Car\n2. Bus\n3. Bicycle\n4. Walking\n5. School Bus\n6. Carpool\nTargets: ";
                string transInput = getRawBuffer();
                stringstream tss(transInput);
                int val;
                while (tss >> val)
                    if (val >= 1 && val <= 6)
                        activeFlags[val - 1] = true;
            }

            if (filterChoice == 1)
                queryEmissionByAge(cities, 3, activeFlags, "Custom Demographic Filter");
            if (filterChoice == 2)
                queryEmissionByTransport(cities, 3, activeFlags, "Custom Transport Filter");
        }
        else
        {
            cout << "please select in range\n";
        }
    }
}

void executeCitySelection(int dsMode, int operationMode, Node *cityA, Node *cityB, Node *cityC)
{
    while (true)
    {
        cout << "\n--- MOUNT TARGET DATASETS ---\n";
        cout << "Please select which city (e.g., '1 3' for City A + City C).\n";
        cout << "1. City A\n2. City B\n3. City C\n4. Back\nTargets: ";

        string input = getRawBuffer();
        stringstream ss(input);
        int val;

        Node *activeCities[3] = {nullptr, nullptr, nullptr};
        string activeNames[3] = {"", "", ""};
        bool abort = false;
        int listCount = 0;

        while (ss >> val)
        {
            if (val == 4)
                return;
            if (val == 1 && activeCities[0] == nullptr)
            {
                activeCities[0] = cityA;
                activeNames[0] = "City A";
                listCount++;
            }
            else if (val == 2 && activeCities[1] == nullptr)
            {
                activeCities[1] = cityB;
                activeNames[1] = "City B";
                listCount++;
            }
            else if (val == 3 && activeCities[2] == nullptr)
            {
                activeCities[2] = cityC;
                activeNames[2] = "City C";
                listCount++;
            }
            else if (val < 1 || val > 4)
            {
                abort = true;
                break;
            }
        }

        if (abort || listCount == 0)
        {
            cout << "\nplease select in range\n";
            continue;
        }

        if (dsMode == 1)
        {
            static Resident cityA_Arr[MAX], cityB_Arr[MAX], cityC_Arr[MAX];
            Resident *cityPtrs[3] = {nullptr, nullptr, nullptr};
            int counts[3] = {0, 0, 0};
            string combinedName = "";

            if (activeNames[0] != "")
            {
                counts[0] = loadCSV("datasets/dataset1-cityA.csv", cityA_Arr);
                cityPtrs[0] = cityA_Arr;
                combinedName += "City A ";
            }
            if (activeNames[1] != "")
            {
                counts[1] = loadCSV("datasets/dataset2-cityB.csv", cityB_Arr);
                cityPtrs[1] = cityB_Arr;
                combinedName += "City B ";
            }
            if (activeNames[2] != "")
            {
                counts[2] = loadCSV("datasets/dataset3-cityC.csv", cityC_Arr);
                cityPtrs[2] = cityC_Arr;
                combinedName += "City C ";
            }

            if (operationMode == 1)
                executeArrayAnalysis(cityPtrs, counts, 3, combinedName);
            else if (operationMode == 2)
                executeArrayEmissionSubMenu(cityPtrs, counts, 3);
        }
        else if (dsMode == 2)
        {
            if (operationMode == 1)
            {
                Node *compressedTargets[3];
                int idx = 0;
                string combinedName = "";
                for (int i = 0; i < 3; i++)
                    if (activeCities[i] != nullptr)
                    {
                        compressedTargets[idx++] = activeCities[i];
                        combinedName += activeNames[i] + " ";
                    }
                executeLinkedListAnalysis(compressedTargets, listCount, combinedName);
            }
            else if (operationMode == 2)
                executeEmissionSubMenu(activeCities, activeNames);
            else
                cout << "\nnot impelmented yet\n";
        }
    }
}

void handleSortingBenchmark(Resident workingArr[], Resident combinedArr[], Node *cityA, Node *cityB, Node *cityC, int totalSize, size_t arrayMemory, size_t linkedListMemory)
{
    while (true)
    {
        cout << "\n--- TASK 6: SORTING CONFIGURATION ---\n";
        cout << "Select Data Attribute to Sort By:\n";
        cout << "1. Age\n2. Daily Distance (km)\n3. Carbon Emission Factor\n4. Days\n5. Mode of Transport\n6. Back to Performance Menu\nChoice: ";

        int sortBy;
        string input = getRawBuffer();
        stringstream ssSort(input);
        if (!(ssSort >> sortBy) || sortBy < 1 || sortBy > 6)
        {
            cout << "Invalid choice. Please enter a number between 1 and 6.\n";
            continue;
        }
        if (sortBy == 6)
            break;

        cout << "\nSelect Sorting Algorithm:\n1. Bubble Sort (Slow, O(n^2))\n2. Insertion Sort (Faster, O(n^2))\nChoice: ";
        int sortMethod;
        input = getRawBuffer();
        stringstream ssMethod(input);
        if (!(ssMethod >> sortMethod) || sortMethod < 1 || sortMethod > 2)
        {
            cout << "Invalid choice. Please enter 1 or 2.\n";
            continue;
        }

        cout << "\nBenchmarking on " << totalSize << " records (" << BENCH_RUNS << " runs, avg of middle 5)...\n";
        BenchResult result = benchmarkSort(combinedArr, cityA, cityB, cityC, totalSize, sortMethod, sortBy);

        cout << "\n================= TASK 6: SORTING RESULTS (avg of 5 runs) ================\n";
        cout << left << setw(20) << "Data Structure" << setw(25) << "Execution Time (us)" << setw(25) << "Memory Usage (Bytes)" << endl;
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(20) << "Array" << setw(25) << result.arrayTime << setw(25) << arrayMemory << endl;
        cout << left << setw(20) << "Linked List" << setw(25) << result.llTime << setw(25) << linkedListMemory << endl;

        cout << "\nWould you like to preview the sorted Array? (y/n): ";
        string previewInput = getRawBuffer();
        if (!previewInput.empty() && (previewInput[0] == 'y' || previewInput[0] == 'Y'))
        {
            // Sort a fresh copy from combinedArr for display (outside of timing)
            for (int i = 0; i < totalSize; i++)
                workingArr[i] = combinedArr[i];
            if (sortMethod == 1)
                bubbleSortArray(workingArr, totalSize, sortBy);
            else
                insertionSortArray(workingArr, totalSize, sortBy);

            cout << "\n--- SORTED ARRAY PREVIEW (First 50 Records) ---\n";
            int limit = (totalSize < 50) ? totalSize : 50;
            for (int i = 0; i < limit; i++)
                cout << left << setw(15) << workingArr[i].id << setw(10) << workingArr[i].age
                     << setw(15) << workingArr[i].transport << setw(15) << workingArr[i].distance
                     << setw(15) << workingArr[i].factor << setw(15) << workingArr[i].days << endl;
        }
    }
}

string matchTransportMode(string input)
{
    string lower = input;
    for (char &c : lower)
        c = tolower(c);

    string modes[] = {"Car", "Bus", "Bicycle", "Walking", "School Bus", "Carpool"};
    for (const string &mode : modes)
    {
        string modeLower = mode;
        for (char &c : modeLower)
            c = tolower(c);
        if (lower == modeLower)
            return mode;
    }
    return "";
}

void handleSearchingBenchmark(Resident combinedArr[], Node *cityA, Node *cityB, Node *cityC, int totalSize, size_t arrayMemory, size_t linkedListMemory)
{
    while (true)
    {
        int searchBy, searchMethod;
        double minVal = 0.0, maxVal = 0.0;
        string targetStr = "";

        cout << "\n--- TASK 7: SEARCHING CONFIGURATION ---\n";
        cout << "1. Age Range\n2. Daily Distance (km) Range\n3. Carbon Emission Factor Range\n4. Days Range\n5. Mode of Transport\n6. Back\nChoice: ";

        string input = getRawBuffer();
        stringstream ss(input);
        if (!(ss >> searchBy) || searchBy < 1 || searchBy > 6)
        {
            cout << "Number not in range\n";
            continue;
        }
        if (searchBy == 6)
            break;

        if (searchBy == 5)
        {
            cout << "Enter Transport Mode (Car, Bus, Bicycle, Walking, School Bus, Carpool): ";
            targetStr = matchTransportMode(getRawBuffer());
            if (targetStr.empty())
            {
                cout << "Invalid transport mode.\n";
                continue;
            }
        }
        else
        {
            cout << "\n--- SET RANGE ---\n";
            cout << "Enter Minimum Value: ";
            string minIn = getRawBuffer();
            cout << "Enter Maximum Value: ";
            string maxIn = getRawBuffer();
            try
            {
                minVal = stod(minIn);
                maxVal = stod(maxIn);
                if (minVal > maxVal)
                {
                    cout << "Error: Minimum value cannot be greater than Maximum value.\n";
                    continue;
                }
            }
            catch (const invalid_argument &)
            {
                cout << "Invalid input: Please enter numerical values only.\n";
                continue;
            }
            catch (const out_of_range &)
            {
                cout << "Invalid input: Number is too large or too small.\n";
                continue;
            }
        }

        cout << "\nSelect Searching Algorithm:\n1. Linear Search\n2. Binary Search\n3. Jump Search\nChoice: ";
        string methodInput = getRawBuffer();
        stringstream methodSS(methodInput);
        if (!(methodSS >> searchMethod) || searchMethod < 1 || searchMethod > 3)
        {
            cout << "Number not in range\n";
            continue;
        }

        if (searchMethod == 2)
            cout << "\nBinary Search selected (Array only). Sorting data...\n";
        if (searchMethod == 3)
            cout << "\nJump Search selected. Sorting data...\n";

        cout << "Benchmarking on " << totalSize << " records (" << BENCH_RUNS << " runs, avg of middle 5)...\n";
        BenchResult result = benchmarkSearch(combinedArr, cityA, cityB, cityC, totalSize, searchMethod, searchBy, minVal, maxVal, targetStr);

        // Prepare a sorted display copy for printArrayResults (outside of timing)
        Resident displayArr[600];
        for (int i = 0; i < totalSize; i++)
            displayArr[i] = combinedArr[i];
        if (searchMethod == 2 || searchMethod == 3)
            insertionSortArray(displayArr, totalSize, searchBy);
        printArrayResults(displayArr, totalSize, searchBy, minVal, maxVal, targetStr);

        string searchNames[] = {"", "Linear Search", "Binary Search", "Jump Search"};

        cout << "\n================ TASK 7: SEARCHING RESULTS (avg of 5 runs) ===============\n";
        cout << left << setw(20) << "Data Structure" << setw(25) << "Search Type" << setw(25) << "Execution Time (us)" << setw(25) << "Memory Usage (Bytes)" << endl;
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(20) << "Array" << setw(25) << searchNames[searchMethod] << setw(25) << result.arrayTime << setw(25) << arrayMemory << endl;
        cout << left << setw(20) << "Linked List" << setw(25) << searchNames[searchMethod]
             << setw(25) << result.llTime
             << setw(25) << linkedListMemory << endl;
        cout << "----------------------------------------------------------------------\n";
    }
}

void executeFullPerformanceAnalysis(Resident combinedArr[], Node *cityA, Node *cityB, Node *cityC, int totalSize, size_t arrayMemory, size_t linkedListMemory)
{
    cout << "\n==========================================================\n";
    cout << "         TASK 8: FULL PERFORMANCE ANALYSIS REPORT         \n";
    cout << "==========================================================\n";
    cout << "Running all algorithms on " << totalSize << " records (" << BENCH_RUNS << " runs each, avg of middle 5)...\n";

    cout << "\n==================== SORTING BENCHMARK ====================\n";
    cout << left << setw(20) << "Algorithm" << setw(20) << "Data Structure"
         << setw(25) << "Execution Time (us)" << setw(25) << "Memory Usage (Bytes)"
         << setw(15) << "Time Complexity" << endl;
    cout << "-------------------------------------------------------------------------------------\n";

    const char *sortNames[] = {"Bubble Sort", "Insertion Sort"};
    for (int m = 1; m <= 2; m++)
    {
        BenchResult r = benchmarkSort(combinedArr, cityA, cityB, cityC, totalSize, m, 1);
        cout << left << setw(20) << sortNames[m - 1] << setw(20) << "Array"
             << setw(25) << r.arrayTime << setw(25) << arrayMemory << setw(15) << "O(n^2)" << endl;
        cout << left << setw(20) << sortNames[m - 1] << setw(20) << "Linked List"
             << setw(25) << r.llTime << setw(25) << linkedListMemory << setw(15) << "O(n^2)" << endl;
    }

    // Searching
    cout << "\n==================== SEARCHING BENCHMARK ==================\n";
    cout << left << setw(20) << "Algorithm" << setw(20) << "Data Structure"
         << setw(25) << "Execution Time (us)" << setw(25) << "Memory Usage (Bytes)"
         << setw(15) << "Time Complexity" << endl;
    cout << "-------------------------------------------------------------------------------------\n";

    // Fixed inputs: age 18-60 gives a realistic match set without being trivially empty or full.
    const double minVal = 18, maxVal = 60;
    const int searchBy = 1; // Age

    const char *searchNames[] = {"Linear Search", "Binary Search", "Jump Search"};
    const char *searchComplexArr[] = {"O(n)", "O(log n)", "O(sqrt(n))"};
    const char *searchComplexLL[] = {"O(n)", "O(n log n)", "O(n*sqrt(n))"};
    int searchMethods[] = {1, 2, 3};

    for (int a = 0; a < 3; a++)
    {
        BenchResult r = benchmarkSearch(combinedArr, cityA, cityB, cityC, totalSize, searchMethods[a], searchBy, minVal, maxVal, "");

        cout << left << setw(20) << searchNames[a] << setw(20) << "Array"
             << setw(25) << r.arrayTime << setw(25) << arrayMemory
             << setw(15) << searchComplexArr[a] << endl;

        cout << left << setw(20) << searchNames[a] << setw(20) << "Linked List"
             << setw(25) << r.llTime << setw(25) << linkedListMemory
             << setw(15) << searchComplexLL[a] << endl;
    }

    cout << "\n==================== MEMORY SUMMARY =======================\n";
    cout << left << setw(25) << "Data Structure" << setw(25) << "Total Records"
         << setw(25) << "Memory Per Record" << setw(25) << "Total Memory (Bytes)" << endl;
    cout << "-------------------------------------------------------------------------------------\n";
    cout << left << setw(25) << "Array" << setw(25) << totalSize << setw(25) << sizeof(Resident) << setw(25) << arrayMemory << endl;
    cout << left << setw(25) << "Linked List" << setw(25) << totalSize << setw(25) << sizeof(Node) << setw(25) << linkedListMemory << endl;

    cout << "\n==========================================================\n";
    cout << "                  END OF PERFORMANCE REPORT               \n";
    cout << "==========================================================\n";
    cout << "\nNote: Analysis of when arrays vs linked lists perform better (Task 8b)\n";
    cout << "and their real-world advantages/disadvantages (Task 8c) are discussed\n";
    cout << "in the project documentation.\n";
}

void executePerformanceComparison(Node *cityA, Node *cityB, Node *cityC)
{
    cout << "\n==========================================================\n";
    cout << "       SYSTEM PERFORMANCE & ALGORITHM BENCHMARKING        \n";
    cout << "==========================================================\n";

    Resident combinedArr[600], workingArr[600];
    int totalSize = 0;
    totalSize += loadCSV("datasets/dataset1-cityA.csv", combinedArr + totalSize);
    totalSize += loadCSV("datasets/dataset2-cityB.csv", combinedArr + totalSize);
    totalSize += loadCSV("datasets/dataset3-cityC.csv", combinedArr + totalSize);

    size_t arrayMemory = sizeof(Resident) * totalSize;
    size_t linkedListMemory = (sizeof(Node) + 16) * totalSize;

    while (true)
    {
        cout << "\n=== ALGORITHM SELECTION (Total Records: " << totalSize << ") ===\n";
        cout << "1. Task 6: Compare Sorting Performance\n";
        cout << "2. Task 7: Compare Searching Performance\n";
        cout << "3. Task 8: Full Performance Analysis\n";
        cout << "4. Return to Main Terminal\n";
        cout << "Choice: ";

        int algoChoice;
        string input = getRawBuffer();
        stringstream ssAlgo(input);
        ssAlgo >> algoChoice;

        if (algoChoice == 1)
            handleSortingBenchmark(workingArr, combinedArr, cityA, cityB, cityC, totalSize, arrayMemory, linkedListMemory);
        else if (algoChoice == 2)
            handleSearchingBenchmark(combinedArr, cityA, cityB, cityC, totalSize, arrayMemory, linkedListMemory);
        else if (algoChoice == 3)
            executeFullPerformanceAnalysis(combinedArr, cityA, cityB, cityC, totalSize, arrayMemory, linkedListMemory);
        else if (algoChoice == 4)
            break;
    }
}

void executeOperationSelection(int dsMode, Node *cityA, Node *cityB, Node *cityC)
{
    int taskChoice = 0;
    while (true)
    {
        cout << "\n=== COMPUTATIONAL OPERATION ===\n";
        cout << "1. Age Group Categorization\n";
        cout << "2. Carbon Emission Analysis\n";
        cout << "3. Ascend to Main Terminal\n";
        cout << "Directive: ";

        string input = getRawBuffer();
        stringstream ss(input);
        ss >> taskChoice;

        if (taskChoice == 3)
            return;
        if (taskChoice >= 1 && taskChoice <= 2)
            executeCitySelection(dsMode, taskChoice, cityA, cityB, cityC);
        else
            cout << "\nplease select in range\n";
    }
}

int main()
{
    Node *cityA = loadCityData("datasets/dataset1-cityA.csv");
    Node *cityB = loadCityData("datasets/dataset2-cityB.csv");
    Node *cityC = loadCityData("datasets/dataset3-cityC.csv");

    int dsChoice = 0;
    while (true)
    {
        cout << "\n======================================================\n";
        cout << " URBAN CARBON EMISSION ANALYSIS ENGINE - MAIN TERMINAL\n";
        cout << "======================================================\n";
        cout << "1. Array Analysis Mode (Tasks 1-5)\n";
        cout << "2. Linked List Analysis Mode (Tasks 1-5)\n";
        cout << "3. System Performance & Benchmarking (Tasks 6, 7, 8)\n";
        cout << "4. Insights & Recommendations (Task 9)\n";
        cout << "5. Quit\n";
        cout << "Architecture: ";

        string input = getRawBuffer();
        stringstream ss(input);
        ss >> dsChoice;

        if (dsChoice == 1 || dsChoice == 2)
            executeOperationSelection(dsChoice, cityA, cityB, cityC);
        else if (dsChoice == 3)
            executePerformanceComparison(cityA, cityB, cityC);
        else if (dsChoice == 4)
            executeInsightsReport(cityA, cityB, cityC);
        else if (dsChoice == 5)
            break;
        else
            cout << "\nPlease select in range\n";
    }

    destroyList(cityA);
    destroyList(cityB);
    destroyList(cityC);
    return 0;
}
