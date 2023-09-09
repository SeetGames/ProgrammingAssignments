#include "uShell.h"
#include <iostream>

int main(int argc, char* argv[])
{
    bool verboseMode = false;

    // Check command line arguments for verbose mode
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose")
        {
            verboseMode = true;
        }
    }

    uShell shell(verboseMode);
    int exitCode = shell.run();

    //std::cout << "Exiting with code: " << exitCode << std::endl;
    return exitCode;
}
