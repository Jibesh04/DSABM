#include <iostream>
#include <ctime>

void myFunction() {
    for (volatile int i = 0; i < 1000000; ++i);
}

int main() {
    clock_t start = clock();

    myFunction();

    clock_t end = clock();
    double elapsed = double(end - start) / CLOCKS_PER_SEC; // Convert to seconds
    std::cout << "Time taken: " << elapsed << " seconds" << std::endl;

    return 0;
}