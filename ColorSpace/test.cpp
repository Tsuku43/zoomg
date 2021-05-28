#include <iostream>

#include "src/ColorSpace.h"
#include "src/Comparison.h"
using namespace std;
int main() {
    ColorSpace::Rgb a(132, 132, 32);
    ColorSpace::Rgb b(132, 131, 32);
    auto p = ColorSpace::Cie2000Comparison::Compare(&a, &b);
    cout << p << endl;
}