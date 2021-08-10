#include <iostream>
#include "src/base/Timestamp.h"

using namespace std;

int main()
{
    Timestamp t1 = Timestamp::now();
    cout << t1.toString() << endl;
    Timestamp t2 = Timestamp::nowAfter(10.0);
    cout << t2.toString() << endl;
    cout << ((t1 < t2)?"yes":"no") << endl;
    return 0;
}