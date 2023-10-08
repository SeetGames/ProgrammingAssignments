#include "uShell3.h"
#include <iostream>

int main(int argc, char* argv[])
{
    bool verboseMode = false;

    // Parse command-line arguments
    for (int i = 1; i < argc; i++) 
    {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") 
        {
            verboseMode = true;
        }
    }

    uShell3 shell(verboseMode);
    int exitCode = shell.run();

    return exitCode;
}
