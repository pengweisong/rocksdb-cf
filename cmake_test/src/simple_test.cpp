

#include <iostream>
#include <vector>

#include <assert.h>
#include <cstring>
using namespace std;

int main()
{
    vector<char> vec;
    vec.reserve(5);
    ::memcpy(static_cast<void*>(&*vec.begin()), "heheh1111133333", 20);

    std::cout << vec.size() << " : " << vec.data() << "\n";

    vec.clear();

    uint64_t num = 9999999;
    ::memcpy(static_cast<void*>(&*vec.begin()),
             reinterpret_cast<const void*>(&num), sizeof(uint64_t));

    std::cout << vec.size() << " : " << vec.data() << "\n";

    uint64_t c = 0;

    ::memcpy(static_cast<void*>(&c),
             reinterpret_cast<const void*>(&*vec.begin()), sizeof(uint64_t));

    std::cout << c << "\n";
    assert(c == num);
}
