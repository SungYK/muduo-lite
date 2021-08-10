#include "src/net/Buffer.h"
#include <string>
#include <iostream>

using namespace std;

int main()
{
    Buffer buf;
    buf.append("Hello world");
    cout << buf.peek() << endl;

    cout << buf.readableBytes() << endl;
    cout << buf.retrieveAsString(5) << endl;
    cout << buf.readableBytes() << endl;
    cout << buf.retrieveAllAsString() << endl;

    return 0;
}