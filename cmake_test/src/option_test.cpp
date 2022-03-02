
#include "gflags/gflags.h"
#include "vertex_options.h"


#include <iostream>

using namespace manager;
using namespace std;

manager::Options options;

int main(int argc, char **argv)
{
    google::ParseCommandLineFlags(&argc, &argv, true);

    std::cout<<FLAGS_kDefaultPartitionNum<<"\n";
    std::cout<<FLAGS_kDefaultNodeTypeLen<<"\n";
    std::cout<<FLAGS_kDefaultNodeType<<"\n";
    std::cout<<FLAGS_kDefaultAsSrcVertexMin<<"\n";
    std::cout<<FLAGS_kDefaultAsSrcVertexMax<<"\n";
    std::cout<<FLAGS_kDefaultAsDestVertexMin<<"\n";
    std::cout<<FLAGS_kDefaultAsDestVertexMax<<"\n";
}
