#include <iostream>
#include <string>
#include <sstream>
#include <chrono>  // Added for Time Complexity
#include <iomanip> // Added for Text Tables
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
                {
                    if (val >= 1 && val <= 5)
                        activeFlags[val - 1] = true;
                }
            }
            else
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
                return; // Pop stack
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

            // Direct to the correct operation
            if (operationMode == 1)
            {
                executeArrayAnalysis(cityPtrs, counts, 3, combinedName);
            }
            else if (operationMode == 2)
            {
                executeArrayEmissionSubMenu(cityPtrs, counts, 3);
            }
        }
        else if (dsMode == 2)
        {
            if (operationMode == 1)
            {
                Node *compressedTargets[3];
                int idx = 0;
                string combinedName = "";
                for (int i = 0; i < 3; i++)
                {
                    if (activeCities[i] != nullptr)
                    {
                        compressedTargets[idx++] = activeCities[i];
                        combinedName += activeNames[i] + " ";
                    }
                }
                executeLinkedListAnalysis(compressedTargets, listCount, combinedName);
            }
            else if (operationMode == 2)
            {
                executeEmissionSubMenu(activeCities, activeNames);
            }
            else
            {
                cout << "\nnot impelmented yet\n";
            }
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

        cout << "\nSelect Sorting Algorithm:\n";
        cout << "1. Bubble Sort (Slow, O(n^2))\n";
        cout << "2. Insertion Sort (Faster, O(n^2))\n";
        cout << "Choice: ";

        int sortMethod;
        input = getRawBuffer();
        stringstream ssMethod(input);

        if (!(ssMethod >> sortMethod) || sortMethod < 1 || sortMethod > 2)
        {
            cout << "Invalid choice. Please enter 1 or 2.\n";
            continue;
        }

        cout << "\nExecuting Sorting Algorithms on " << totalSize << " records... Please wait.\n";

        // Reset to fresh data for fair benchmarking
        for (int i = 0; i < totalSize; i++)
            workingArr[i] = combinedArr[i];
        Node *combinedLLHead = buildMergedList(cityA, cityB, cityC);

        // --- Benchmark Array ---
        auto startArr = high_resolution_clock::now();
        if (sortMethod == 1)
            bubbleSortArray(workingArr, totalSize, sortBy);
        else
            insertionSortArray(workingArr, totalSize, sortBy);
        auto stopArr = high_resolution_clock::now();
        auto arrTime = duration_cast<microseconds>(stopArr - startArr).count();

        // --- Benchmark Linked List ---
        auto startLL = high_resolution_clock::now();
        if (sortMethod == 1)
            bubbleSortLinkedList(combinedLLHead, sortBy);
        else
            insertionSortLinkedList(combinedLLHead, sortBy);
        auto stopLL = high_resolution_clock::now();
        auto llTime = duration_cast<microseconds>(stopLL - startLL).count();

        // --- Results Table ---
        cout << "\n================= TASK 6: SORTING RESULTS ================\n";
        cout << left << setw(20) << "Data Structure" << setw(25) << "Execution Time (us)" << setw(25) << "Memory Usage (Bytes)" << endl;
        cout << "----------------------------------------------------------------------\n";
        cout << left << setw(20) << "Array" << setw(25) << arrTime << setw(25) << arrayMemory << endl;
        cout << left << setw(20) << "Linked List" << setw(25) << llTime << setw(25) << linkedListMemory << endl;

        // Preview Logic
        cout << "\nWould you like to preview the sorted Array? (y/n): ";
        string previewInput = getRawBuffer();
        if (!previewInput.empty() && (previewInput[0] == 'y' || previewInput[0] == 'Y'))
        {
            cout << "\n--- SORTED ARRAY PREVIEW (First 50 Records) ---\n";
            int limit = (totalSize < 50) ? totalSize : 50;
            for (int i = 0; i < limit; i++)
            {
                cout << left << setw(15) << workingArr[i].id << setw(10) << workingArr[i].age
                     << setw(15) << workingArr[i].transport << setw(15) << workingArr[i].distance
                     << setw(15) << workingArr[i].factor << setw(15) << workingArr[i].days << endl;
            }
        }
        destroyList(combinedLLHead);
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

        // add validation
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
            string rawTransport = getRawBuffer();
            targetStr = matchTransportMode(rawTransport);
            if (targetStr.empty())
            {
                cout << "Invalid transport mode. Please enter one of: Car, Bus, Bicycle, Walking, School Bus, Carpool\n";
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
            catch (const invalid_argument &e)
            {
                cout << "Invalid input: Please enter numerical values only.\n";
                continue;
            }
            catch (const out_of_range &e)
            {
                cout << "Invalid input: Number is too large or too small.\n";
                continue;
            }
        }
        cout << "\nSelect Searching Algorithm:\n1. Linear Search\n2. Binary Search (Array Only)\n3. Jump Search\nChoice: ";
        string methodInput = getRawBuffer();
        stringstream methodSS(methodInput);

        Resident workingArr[600];

        // Copy fresh data every time (IMPORTANT)
        for (int i = 0; i < totalSize; i++)
        {
            workingArr[i] = combinedArr[i];
        }

        // validation
        if (!(methodSS >> searchMethod) || searchMethod < 1 || searchMethod > 3)
        {
            cout << "Number not in range\n";
            continue;
        }

        Node *combinedLLHead = buildMergedList(cityA, cityB, cityC);

        // Benchmarking variables
        int arrCount;
        string searchTypeArr = "";
        auto startArr = high_resolution_clock::now();
        auto stopArr = high_resolution_clock::now();
        auto startLL = high_resolution_clock::now();
        auto stopLL = high_resolution_clock::now();

        // Benchmark Array
        if (searchMethod == 1)
        {
            searchTypeArr = "Linear Search";
            startArr = high_resolution_clock::now();
            arrCount = linearSearchArray(workingArr, totalSize, searchBy, minVal, maxVal, targetStr);
            stopArr = high_resolution_clock::now();
        }
        else if (searchMethod == 2)
        {
            searchTypeArr = "Binary Search";
            cout << "\nBinary Search selected (Array only).\nSorting data...\n";
            insertionSortArray(workingArr, totalSize, searchBy);

            startArr = high_resolution_clock::now();
            arrCount = binarySearchArray(workingArr, totalSize, searchBy, minVal, maxVal, targetStr);
            stopArr = high_resolution_clock::now();
        }
        else if (searchMethod == 3)
        {
            searchTypeArr = "Jump Search";
            insertionSortArray(workingArr, totalSize, searchBy);
            startArr = high_resolution_clock::now();
            arrCount = jumpSearchArray(workingArr, totalSize, searchBy, minVal, maxVal, targetStr);
            stopArr = high_resolution_clock::now();
        }

        // Benchmark Linked List
        if (searchMethod == 1)
        {
            startLL = high_resolution_clock::now();
            linearSearchLinkedList(combinedLLHead, searchBy, minVal, maxVal, targetStr);
            stopLL = high_resolution_clock::now();
        }
        else if (searchMethod == 3)
        {
            cout << "\nJump Search selected.\nSorting data...\n";
            insertionSortLinkedList(combinedLLHead, searchBy); // sort first for jump search

            startLL = high_resolution_clock::now();
            jumpSearchLinkedList(combinedLLHead, searchBy, minVal, maxVal, targetStr);
            stopLL = high_resolution_clock::now();
        }

        printArrayResults(workingArr, totalSize, searchBy, minVal, maxVal, targetStr);

        // Results Table
        cout << "\n================ TASK 7: SEARCHING RESULTS ===============\n";
        cout << left << setw(20) << "Data Structure"
             << setw(25) << "Search Type"
             << setw(25) << "Execution Time (us)"
             << setw(25) << "Memory Usage (Bytes)" << endl;

        cout << "----------------------------------------------------------------------\n";

        cout << left << setw(20) << "Array"
             << setw(25) << searchTypeArr
             << setw(25) << duration_cast<microseconds>(stopArr - startArr).count()
             << setw(25) << arrayMemory << endl;

        // LINKED LIST RESULT
        bool isLLSupported = (searchMethod == 1 || searchMethod == 3); // Linear and Jump supported
        string llSearchType = isLLSupported ? (searchMethod == 1 ? "Linear Search" : "Jump Search") : "N/A";
        string llTime = isLLSupported ? to_string(duration_cast<microseconds>(stopLL - startLL).count()) : "N/A";
        string llMem = isLLSupported ? to_string(linkedListMemory) : "N/A";

        cout << left << setw(20) << "Linked List"
             << setw(25) << llSearchType
             << setw(25) << llTime
             << setw(25) << llMem << endl;

        cout << "----------------------------------------------------------------------\n";

        if (!isLLSupported)
            cout << "\n*Binary Search not supported on Linked List (no random access)\n";

        destroyList(combinedLLHead);
    }
}

void executeFullPerformanceAnalysis(Resident combinedArr[], Node *cityA, Node *cityB, Node *cityC, int totalSize, size_t arrayMemory, size_t linkedListMemory)
{
    cout << "\n==========================================================\n";
    cout << "         TASK 8: FULL PERFORMANCE ANALYSIS REPORT         \n";
    cout << "==========================================================\n";
    cout << "Running all algorithms on " << totalSize << " records...\n";

    // Sorting Benchmark
    cout << "\n==================== SORTING BENCHMARK ====================\n";
    cout << left << setw(20) << "Algorithm"
         << setw(20) << "Data Structure"
         << setw(25) << "Execution Time (us)"
         << setw(25) << "Memory Usage (Bytes)"
         << setw(15) << "Time Complexity" << endl;
    cout << "-------------------------------------------------------------------------------------\n";

    // Bubble Sort
    {
        Resident workingArr[600];
        for (int i = 0; i < totalSize; i++)
            workingArr[i] = combinedArr[i];
        Node *llHead = buildMergedList(cityA, cityB, cityC);

        auto startArr = high_resolution_clock::now();
        bubbleSortArray(workingArr, totalSize, 1); // sortBy: 1 = Age
        auto stopArr = high_resolution_clock::now();

        auto startLL = high_resolution_clock::now();
        bubbleSortLinkedList(llHead, 1); // sortBy: 1 = Age
        auto stopLL = high_resolution_clock::now();

        cout << left << setw(20) << "Bubble Sort"
             << setw(20) << "Array"
             << setw(25) << duration_cast<microseconds>(stopArr - startArr).count()
             << setw(25) << arrayMemory
             << setw(15) << "O(n^2)" << endl;

        cout << left << setw(20) << "Bubble Sort"
             << setw(20) << "Linked List"
             << setw(25) << duration_cast<microseconds>(stopLL - startLL).count()
             << setw(25) << linkedListMemory
             << setw(15) << "O(n^2)" << endl;

        destroyList(llHead);
    }

    // Insertion Sort
    {
        Resident workingArr[600];
        for (int i = 0; i < totalSize; i++)
            workingArr[i] = combinedArr[i];
        Node *llHead = buildMergedList(cityA, cityB, cityC);

        auto startArr = high_resolution_clock::now();
        insertionSortArray(workingArr, totalSize, 1); // sortBy: 1 = Age
        auto stopArr = high_resolution_clock::now();

        auto startLL = high_resolution_clock::now();
        insertionSortLinkedList(llHead, 1); // sortBy: 1 = Age
        auto stopLL = high_resolution_clock::now();

        cout << left << setw(20) << "Insertion Sort"
             << setw(20) << "Array"
             << setw(25) << duration_cast<microseconds>(stopArr - startArr).count()
             << setw(25) << arrayMemory
             << setw(15) << "O(n^2)" << endl;

        cout << left << setw(20) << "Insertion Sort"
             << setw(20) << "Linked List"
             << setw(25) << duration_cast<microseconds>(stopLL - startLL).count()
             << setw(25) << linkedListMemory
             << setw(15) << "O(n^2)" << endl;

        destroyList(llHead);
    }

    // Searching Benchmark
    cout << "\n==================== SEARCHING BENCHMARK ==================\n";
    cout << left << setw(20) << "Algorithm"
         << setw(20) << "Data Structure"
         << setw(25) << "Execution Time (us)"
         << setw(25) << "Memory Usage (Bytes)"
         << setw(15) << "Time Complexity" << endl;
    cout << "-------------------------------------------------------------------------------------\n";

    // Test on age range 18-60 as a fixed benchmark input
    double minVal = 18, maxVal = 60;
    int searchBy = 1; // Age

    // Linear Search
    {
        Resident workingArr[600];
        for (int i = 0; i < totalSize; i++)
            workingArr[i] = combinedArr[i];
        Node *llHead = buildMergedList(cityA, cityB, cityC);

        auto startArr = high_resolution_clock::now();
        linearSearchArray(workingArr, totalSize, searchBy, minVal, maxVal, "");
        auto stopArr = high_resolution_clock::now();

        auto startLL = high_resolution_clock::now();
        linearSearchLinkedList(llHead, searchBy, minVal, maxVal, "");
        auto stopLL = high_resolution_clock::now();

        cout << left << setw(20) << "Linear Search"
             << setw(20) << "Array"
             << setw(25) << duration_cast<microseconds>(stopArr - startArr).count()
             << setw(25) << arrayMemory
             << setw(15) << "O(n)" << endl;

        cout << left << setw(20) << "Linear Search"
             << setw(20) << "Linked List"
             << setw(25) << duration_cast<microseconds>(stopLL - startLL).count()
             << setw(25) << linkedListMemory
             << setw(15) << "O(n)" << endl;

        destroyList(llHead);
    }

    // Binary Search (Array only)
    {
        Resident workingArr[600];
        for (int i = 0; i < totalSize; i++)
            workingArr[i] = combinedArr[i];
        insertionSortArray(workingArr, totalSize, searchBy);

        auto startArr = high_resolution_clock::now();
        binarySearchArray(workingArr, totalSize, searchBy, minVal, maxVal, "");
        auto stopArr = high_resolution_clock::now();

        cout << left << setw(20) << "Binary Search"
             << setw(20) << "Array"
             << setw(25) << duration_cast<microseconds>(stopArr - startArr).count()
             << setw(25) << arrayMemory
             << setw(15) << "O(log n)" << endl;

        cout << left << setw(20) << "Binary Search"
             << setw(20) << "Linked List"
             << setw(25) << "N/A"
             << setw(25) << "N/A"
             << setw(15) << "N/A" << endl;

        cout << "  *Binary Search not supported on Linked List (no random access)\n";
    }

    // Jump Search
    {
        Resident workingArr[600];
        for (int i = 0; i < totalSize; i++)
            workingArr[i] = combinedArr[i];
        insertionSortArray(workingArr, totalSize, searchBy);
        Node *llHead = buildMergedList(cityA, cityB, cityC);
        insertionSortLinkedList(llHead, searchBy);

        auto startArr = high_resolution_clock::now();
        jumpSearchArray(workingArr, totalSize, searchBy, minVal, maxVal, "");
        auto stopArr = high_resolution_clock::now();

        auto startLL = high_resolution_clock::now();
        jumpSearchLinkedList(llHead, searchBy, minVal, maxVal, "");
        auto stopLL = high_resolution_clock::now();

        cout << left << setw(20) << "Jump Search"
             << setw(20) << "Array"
             << setw(25) << duration_cast<microseconds>(stopArr - startArr).count()
             << setw(25) << arrayMemory
             << setw(15) << "O(sqrt(n))" << endl;

        cout << left << setw(20) << "Jump Search"
             << setw(20) << "Linked List"
             << setw(25) << duration_cast<microseconds>(stopLL - startLL).count()
             << setw(25) << linkedListMemory
             << setw(15) << "O(sqrt(n))" << endl;

        destroyList(llHead);
    }

    // Memory Summary
    cout << "\n==================== MEMORY SUMMARY =======================\n";
    cout << left << setw(25) << "Data Structure"
         << setw(25) << "Total Records"
         << setw(25) << "Memory Per Record"
         << setw(25) << "Total Memory (Bytes)" << endl;
    cout << "-------------------------------------------------------------------------------------\n";
    cout << left << setw(25) << "Array"
         << setw(25) << totalSize
         << setw(25) << sizeof(Resident)
         << setw(25) << arrayMemory << endl;
    cout << left << setw(25) << "Linked List"
         << setw(25) << totalSize
         << setw(25) << sizeof(Node)
         << setw(25) << linkedListMemory << endl;

    cout << "\n  *Linked List overhead includes the 'next' pointer per node ("
         << sizeof(Node *) << " bytes extra per record)\n";

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

    size_t linkedListMemory = 0;
    linkedListMemory = sizeof(Node) * totalSize;    // struct data
    linkedListMemory += sizeof(void *) * totalSize; // next pointers already included in sizeof(Node)
    linkedListMemory += 16 * totalSize;

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
        {
            handleSortingBenchmark(workingArr, combinedArr, cityA, cityB, cityC, totalSize, arrayMemory, linkedListMemory);
        }
        else if (algoChoice == 2)
        {
            handleSearchingBenchmark(combinedArr, cityA, cityB, cityC, totalSize, arrayMemory, linkedListMemory);
        }
        else if (algoChoice == 3)
        {
            executeFullPerformanceAnalysis(combinedArr, cityA, cityB, cityC, totalSize, arrayMemory, linkedListMemory);
        }
        else if (algoChoice == 4)
        {
            break;
        }
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

// main menu
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
        {
            executeOperationSelection(dsChoice, cityA, cityB, cityC);
        }
        else if (dsChoice == 3)
        {
            executePerformanceComparison(cityA, cityB, cityC);
        }
        else if (dsChoice == 4)
        {
            executeInsightsReport(cityA, cityB, cityC);
        }
        else if (dsChoice == 5)
        {
            break;
        }
        else
        {
            cout << "\nPlease select in range\n";
        }
    }

    destroyList(cityA);
    destroyList(cityB);
    destroyList(cityC);
    return 0;
}
