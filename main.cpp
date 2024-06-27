#include "parser.h"

#include <string>
#include <filesystem>
#include <iostream>


int main(int argc, char* argv[])
{
    for (const auto & entry : std::filesystem::directory_iterator(".")) //argv[1]
    {
        if (entry.path().string().ends_with(".proto")) {

            try
            {
            Parser::parseFile( entry.path().string() );
            }
            catch (const std::exception& exception)
            {
                std::cerr << "exception: " << exception.what() << '\n';
            }
            catch(...)
            {
                std::cerr << "Abnormal termination\n";
            }
        }
    }

    return 0;
}
