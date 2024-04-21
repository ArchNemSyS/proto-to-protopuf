#include "parser.h"

#include <iostream>
#include <fstream>
#include <string>








int main(int argc, char* argv[])
{
    std::cout << "loading " << argv[1] << '\n';

    //std::ifstream inf{ argv[1] };
    std::ifstream inf{ "test.proto" };

    if (inf)
    {
        inf.seekg(0, std::ios::end);
        size_t size = inf.tellg();
        std::string source(size, ' ');
        inf.seekg(0);
        inf.read(&source[0], size);



        Parser parser;
        parser.parse(source);

    }
    else
    {
        std::cerr << argv[1] << " could not be opened for reading!\n";
        return 1;
    }



    return 0;
}
