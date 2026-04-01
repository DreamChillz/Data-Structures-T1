#ifndef ARRAY_H
#define ARRAY_H

#include <string>
using namespace std;

const int MAX = 5000; // Maximum residents per dataset

struct Resident {
    string id;
    int age;
    string transport;
    double distance;
    double factor;
    int days;
};


int loadCSV(string filename, Resident arr[]);
double calculateEmission(Resident r);

void executeArrayAnalysis(Resident* cities[], int counts[], int numCities, string cityName);
void queryArrayEmissionByAge(Resident* cities[], int counts[], int numCities, bool activeAges[5]);
void queryArrayEmissionByTransport(Resident* cities[], int counts[], int numCities, bool activeTransports[6]);

#endif