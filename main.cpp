#include <iostream>
#include <string>
#include <fstream>
#include "Simulate.h"
#include "Tester.h"



int main() {

    srand(time(nullptr));

    printf("-----------------Cache Simulator-----------------\n");
    printf("Do you want to:\n");
    printf("1. Run automated tests\n");
    printf("2. Run custom setup\n");

    int choice;
    std::cin >> choice;

    if (choice == 1) {
        runTests();
        return 0;
    }

    customSetup();

    return 0;
}

