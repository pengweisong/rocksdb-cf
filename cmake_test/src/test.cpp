

#include <iostream>

#include "gflags/gflags.h"
#include "options.h"
using namespace std;
int main(int argc, char **argv)
{
    google::ParseCommandLineFlags(&argc, &argv, true);
    // google::SetCommandLineOption("flagfile","option.conf");
    cout << "argc=" << argc << endl;
    if (FLAGS_big_menu) {
        cout << "big menu is ture" << endl;
    }
    else {
        cout << "big menu is flase" << endl;
    }

    cout << "languages=" << FLAGS_languages << endl;
    return 0;
}
