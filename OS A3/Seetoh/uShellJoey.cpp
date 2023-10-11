/*
\file		uShell.cpp
\author 	Joey Chua
\par    	email: joeyjunyu.c\@digipen.edu
\date   	Aug 7, 2023
\brief      This file defines function that mimics the behaviour
of a powershell.

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
 */
#include "uShell3.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>     // For strerror
#include <cstdlib>     // For getenv
#include <unistd.h>    // For execvp
#include <sys/types.h> // For pid_t
#include <sys/wait.h>  // For waitpid
#include <sys/stat.h>  // For struct stat

uShell3::uShell3(bool bFlag) : uShell2(bFlag)
{
    // Initialize function pointers for internal commands
    m_internalCmdList3["finish"] = &uShell3::finish;
}

// 3.1 uShell3::ProcessInfo::ProcessInfo()
uShell3::ProcessInfo::ProcessInfo() : PID(0), bActive(false)
{
    // Default constructor sets PID to 0 and bActive to false
}

// 3.2 uShell3::ProcessInfo::ProcessInfo(int id, bool state)
uShell3::ProcessInfo::ProcessInfo(int id, bool state) : PID(id), bActive(state)
{
    // Constructor sets PID to id and bActive to state
}
/*!
 * \brief
 * Finish command: wait for a background process to finish.
 * \param tokenList
 * The list of data to read in, the process ID to wait for.
 */
void uShell3::finish(const TokenList &tokenList)
{
    // Ensure there are exactly 2 parameters
    if (!(tokenList.size() - 2 == 0))
    {
        std::cerr << "Error: no such process index." << std::endl;
        return;
    }

    try
    {
        // Parse the process index from the token list and validate it
        int processIndex = std::stoi(tokenList[1]);
        if (processIndex < 0 || static_cast<size_t>(processIndex) >= m_bgProcessList.size())
        {
            throw std::invalid_argument("");
        }

        ProcessInfo &processInfo = m_bgProcessList[processIndex];

        // Check if the process is still active
        if (!processInfo.bActive)
        {
            std::cerr << "Process Index " << processIndex << " process " << processInfo.PID
                      << " is no longer a child process." << std::endl;
            return;
        }

        // Wait for the specified process and retrieve its exit status
        int status;
        if (waitpid(processInfo.PID, &status, 0) != -1)
        {
            // Print the process status and mark it as inactive
            std::cout << "process " << processInfo.PID
                      << " exited with exit status " << WEXITSTATUS(status) << std::endl;
            processInfo.bActive = !processInfo.bActive;
        }
        else
        {
            // Print an error message
            std::cerr << "Error: Failed to wait for process " << processIndex << std::endl;
        }
    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Error: no such process index." << std::endl;
    }
}

/*!
\brief
Determine whether the command exists or not.
\param tokenList
The list of tokens to get the command and arguments from.
\param startParam
The starting token to parse data from the list.
\param endParam
The last token to parse data from the list.
*/
bool uShell3::exist(TokenList const &tokenList, unsigned startParam, unsigned endParam)
{
    (void)endParam;
    // Check if the tokenList is empty or the startParam is greater than or equal to the size
    if (tokenList.empty() || startParam >= tokenList.size())
    {
        return false;
    }

    // Get the executable file name from the tokenList
    std::string executable = tokenList[startParam];

    // Check if the executable carries an absolute path
    if (executable[0] == '/' || executable[0] == '.')
    {
        // Check if the file exists using the stat system call
        struct stat buffer;
        if (stat(executable.c_str(), &buffer) == 0)
        {
            return true; // File exists
        }
    }
    else
    {
        // Search for the file in the directories specified by PATH environment variable
        char *path = getenv("PATH");
        if (path != nullptr)
        {
            std::string pathString(path);
            std::istringstream pathStream(pathString);
            std::string dir;
            while (std::getline(pathStream, dir, ':'))
            {
                std::string filePath = dir + "/" + executable;
                struct stat buffer;
                if (stat(filePath.c_str(), &buffer) == 0)
                {
                    return true; // File exists in one of the directories
                }
            }
        }
    }

    return false; // File does not exist
}

// Convert the TokenList (assumed to be a vector of strings) to an array of char pointers
char **convertToArgv(const TokenList &tokens)
{
    char **argv = new char *[tokens.size() + 1]; // +1 for the NULL terminator

    for (size_t i = 0; i < tokens.size(); ++i)
    {
        argv[i] = const_cast<char *>(tokens[i].c_str());
    }

    argv[tokens.size()] = nullptr; // Null-terminate the argv array
    return argv;
}

void uShell3::doExternalCmd(const TokenList &tokenList)
{
    if (tokenList.empty())
    {
        return; // Return silently if the token list is empty.
    }

    if (tokenList[0] == "|")
    {
        return; // Starting with a pipe is a syntax error.
    }

    // Check for background token.
    bool isBackground = (tokenList.back() == "&");

    // Create a list for pipe information.
    std::vector<PipeInfo> pipesInfo;

    // Count pipes and store their positions.
    for (size_t i = 0; i < tokenList.size(); ++i)
    {
        if (tokenList[i] == "|")
        {
            if (i == 0 || i == tokenList.size() - 1 || tokenList[i + 1] == "|")
            {
                std::cout << "Error: syntax error" << std::endl;
                return; // Syntax error for empty strings between pipes or pipe at the end.
            }

            PipeInfo pInfo;
            if (pipe(pInfo.descriptor) == -1)
            {
                perror("pipe");
                return; // Error while creating a pipe.
            }
            pInfo.posInToken = i;
            pipesInfo.push_back(pInfo);
        }
    }

    size_t startPos = 0;
    bool commandFailed = false; // To keep track if any command in the pipeline fails

    for (size_t p = 0; p <= pipesInfo.size() && !commandFailed; ++p)
    {
        size_t endPos = (p == pipesInfo.size()) ? tokenList.size() : pipesInfo[p].posInToken;

        // Extract command and its arguments from tokenList.
        TokenList cmdTokens(tokenList.begin() + startPos, tokenList.begin() + endPos);

        // If the last command ends with background "&", remove it.
        if (isBackground && p == pipesInfo.size())
        {
            cmdTokens.pop_back();
        }

        // Check if the command exists.
        if (!exist(cmdTokens, 0, cmdTokens.size()))
        {
            std::cout << "Error: " << cmdTokens[0] << " cannot be found" << std::endl;
            commandFailed = true;
            break; // Exit the loop, no need to process further commands in the pipeline.
        }

        pid_t pid = fork();

        if (pid == -1)
        {
            std::cout << "Error: Failed to create child process" << std::endl;
            return; // Error during fork.
        }
        else if (pid == 0)
        { // Child process
            if (p != 0)
            { // Not the first command
                dup2(pipesInfo[p - 1].descriptor[PipeInfo::IN_DESCRIPTOR], STDIN_FILENO);
            }

            if (p != pipesInfo.size())
            { // Not the last command
                dup2(pipesInfo[p].descriptor[PipeInfo::OUT_DESCRIPTOR], STDOUT_FILENO);
            }

            // Close all pipe descriptors.
            for (const auto &pipeInfo : pipesInfo)
            {
                close(pipeInfo.descriptor[PipeInfo::IN_DESCRIPTOR]);
                close(pipeInfo.descriptor[PipeInfo::OUT_DESCRIPTOR]);
            }

            char **argv = convertToArgv(cmdTokens);
            execvp(argv[0], argv); // If execvp fails, the following line will be executed.
            _exit(EXIT_FAILURE);   // Just exit if execvp fails.
        }

        // Parent code
        // Close the write end of the current pipe; it's no longer needed after the fork.
        if (p != pipesInfo.size())
        {
            close(pipesInfo[p].descriptor[PipeInfo::OUT_DESCRIPTOR]);
        }

        if (isBackground)
        {
            m_bgProcessList.push_back(ProcessInfo(pid, true));
            std::cout << "[" << m_bgProcessList.size() - 1 << "] process " << pid << std::endl;
        }
        else
        {
            // Only if no background processing is specified
            waitpid(pid, NULL, 0);
        }

        startPos = endPos + 1; // Update the start position for the next command.
    }

    // Parent should close all read-end pipe descriptors.
    for (const auto &pipeInfo : pipesInfo)
    {
        close(pipeInfo.descriptor[PipeInfo::IN_DESCRIPTOR]);
    }
}

int uShell3::run()
{
    while (!m_exit)
    {
        std::cout << m_prompt << ">";

        // Read user input
        std::string input;
        if (!getInput(input))
            break;

        if (input.empty())
            continue;
        if (!input.empty() && input[0] == '#')
        {
            std::cerr << "Error: " << input << " cannot be found" << std::endl;
            continue;
        }

        if (input == "exit")
            break;
        std::string originalInput = input; // Keep the original input for verbose output

        if (m_verbose)
        {
            if (originalInput.empty())
                continue;

            printVerbose(originalInput); // Display the original input in verbose mode
        }

        TokenList tokenList;
        tokenize(input, tokenList);

        if (!replaceVars(tokenList))
        {
            continue;
        }

        if (!tokenList.empty())
        {
            const auto &cmd = tokenList[0];

            // Check if it's an internal command in m_internalCmdList
            if (m_internalCmdList.find(cmd) != m_internalCmdList.end())
            {
                auto func = m_internalCmdList[cmd];
                (this->*func)(tokenList);
            }
            // Check if it's an internal command in m_internalCmdList2
            else if (m_internalCmdList2.find(cmd) != m_internalCmdList2.end())
            {
                auto func = m_internalCmdList2[cmd];
                (this->*func)(tokenList);
            }
            // Check if it's an internal command in m_internalCmdList3
            else if (m_internalCmdList3.find(cmd) != m_internalCmdList3.end())
            {
                auto func = m_internalCmdList3[cmd];
                (this->*func)(tokenList);
            }
            else
            {
                doExternalCmd(tokenList);
            }
        }
        else
        {
            continue;
        }
    }
    return m_exitCode;
}

// g++ -std=c++17 -Werror -Wall -Wextra -pedantic -c uShell3.cpp -o uShell3.o
// g++ -std=c++17 -Werror -Wall -Wextra -pedantic uShell3.o uShell1.obj shellmain3.obj -o uShell3
//  ./uShell3
//  ./uShell_ref -v <test_cases_for_A1.3.txt> REFverbose_result.txt
//  ./uShell3 -v <test_cases_for_A1.3.txt> verbose_result.txt