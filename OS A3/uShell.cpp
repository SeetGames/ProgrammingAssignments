/*!************************************************************************
 \file uShell.cpp
 \author Seetoh Wei Tung
 \par Email: seetoh.w@digipen.edu
 \par Course: CSD2182
 \par Assignment #1.3
 \date 10-10-2023
 \brief Continuation of uShell2 assignment. This file contains the
    implementation of the uShell3 class. It inherits from uShell2 and
    adds additional functionality to the shell.
**************************************************************************/
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "uShell3.h"
#include <algorithm>
#include <cstring>

/*!*****************************************************************************
 * @brief Construct a new u Shell3::Process Info::Process Info object
 *******************************************************************************/
uShell3::ProcessInfo::ProcessInfo() : PID(0), bActive(false) {}

/*!*****************************************************************************
 * @brief Construct a new u Shell3::Process Info::Process Info object
 *
 * @param _id The process ID given by the OS
 * @param _state The state of the process to set
 *******************************************************************************/
uShell3::ProcessInfo::ProcessInfo(int _id, bool _state) : PID(_id), bActive(_state) {}

/*!*****************************************************************************
 * @brief Determine whether the command exists or not.
 * This function determines whether the command exists or not. It gets the executable file
 * name. If the file carries an absolute path, it checks whether the file exists. Otherwise, it sets
 * up the environment variables list and loop through the list to find whether the file exists.
 * @param _tokenList The list of tokens to get the command and arguments from
 * @param _start The starting token to parse data from the list
 * @param _end The last token to parse data from the list
 * @return true
 * @return false
 *******************************************************************************/
bool uShell3::exist(const TokenList &_tokenList, unsigned _start, unsigned _end)
{
    (void)_end; // Unused

    // Ensure the bounds are valid
    if (_start >= _tokenList.size() || _tokenList.empty())
        return false;

    std::string command = _tokenList[_start];

    // If it has a '/', it's treated as a path (either relative or absolute)
    if (command.front() == '/' || command.front() == '.')
    {
        struct stat buffer;
        return (stat(command.c_str(), &buffer) == 0);
    }
    else
    {
        const char *pathEnv = getenv("PATH");
        if (!pathEnv)
            return false;

        // Create a copy of the PATH environment variable to use with strtok()
        char *pathCopy = strdup(pathEnv);
        if (!pathCopy)
            return false;

        char *token = strtok(pathCopy, ":");

        while (token)
        {
            std::string fullPath = std::string(token) + "/" + command;

            struct stat buffer;
            if (stat(fullPath.c_str(), &buffer) == 0)
            {
                free(pathCopy); // Cleanup before returning
                return true;    // Command exists in this directory
            }

            token = strtok(NULL, ":");
        }

        free(pathCopy); // Cleanup after using strdup()
    }
    return false;
}

/*!*****************************************************************************
 * @brief Calls an external command using the passed in parameters.
 * @param _tokenList The list of tokens to get the data value from
 *******************************************************************************/
void uShell3::doExternalCmd(const TokenList &_tokenList)
{
    if (_tokenList.front() == "|" || _tokenList.empty())
        return;

    bool runInBackground = false;
    if (!_tokenList.empty() && _tokenList.back() == "&")
        runInBackground = true;

    std::vector<PipeInfo> pipes{};
    std::vector<int> inDescriptors, outDescriptors;
    // Find the position of the pipe token "|" within the token list
    for (size_t i = 0; i < _tokenList.size(); ++i)
    {
        // Skip if this token isn't a pipe
        if (_tokenList.at(i) != "|")
            continue;

        // don't allow pipe at the beginning or end of the command or double pipe
        if (i == 0 || i == _tokenList.size() - 1 || _tokenList[i + 1] == "|")
        {
            std::cerr << "Error: syntax error" << std::endl;
            return;
        }

        // Valid pipe, create a pipe
        PipeInfo pipeInfo;
        // pipe takes in an array of 2 integers, the first is the read descriptor, the second is the write descriptor
        if (pipe(pipeInfo.descriptor) == -1)
        { // pipe returns -1 if it fails to create a pipe
            perror("pipe");
            return;
        }
        // Set the position of the pipe token to i in the PipeInfo struct
        pipeInfo.posInToken = i;
        // Add this pipe to the vector of pipes
        pipes.push_back(pipeInfo);
        inDescriptors.push_back(pipeInfo.descriptor[PipeInfo::IN_DESCRIPTOR]);
        outDescriptors.push_back(pipeInfo.descriptor[PipeInfo::OUT_DESCRIPTOR]);
    }

    bool commandFailed = false;

    // Execute commands
    for (size_t i = 0; i <= pipes.size(); ++i)
    {
        // Check if any of the commands failed and break out of the loop
        if (commandFailed)
            break;

        size_t startPos = (i == 0) ? 0 : pipes[i - 1].posInToken + 1;
        size_t endPos{};
        if (i == pipes.size())
            endPos = _tokenList.size();
        else
            endPos = pipes[i].posInToken;

        // Get the commands(strings) from the token list
        TokenList commands(_tokenList.begin() + startPos, _tokenList.begin() + endPos);

        if (runInBackground && i == pipes.size())
            commands.pop_back();

        if (!exist(commands, 0, commands.size()))
        { // If the command doesn't exist, print an error message and break out of the loop and don't execute any further commands
            std::cerr << "Error: " << commands[0] << " cannot be found" << std::endl;
            // commandFailed = true;
            break; // Exit the loop, no need to process further commands in the pipeline.
        }

        // Fork a child process
        pid_t pid = fork();
        if (pid < 0)
        { // fork returns -1 if it fails to create a child process
            perror("fork");
            return;
        }
        else if (pid == 0)
        { // Fork is successful and this is the child process
            if (i != 0)
            {
                dup2(pipes[i - 1].descriptor[PipeInfo::IN_DESCRIPTOR], STDIN_FILENO);
            }
            if (i != pipes.size())
            {
                dup2(pipes[i].descriptor[PipeInfo::OUT_DESCRIPTOR], STDOUT_FILENO);
            }
            for (const auto &pipeInfo : pipes)
            {
                close(pipeInfo.descriptor[PipeInfo::IN_DESCRIPTOR]);
                close(pipeInfo.descriptor[PipeInfo::OUT_DESCRIPTOR]);
            }

            std::vector<char *> args;
            for (const auto &cmd : commands)
            {
                args.push_back(const_cast<char *>(cmd.c_str()));
            }
            args.push_back(nullptr);
            execvp(args[0], args.data());
            _exit(EXIT_FAILURE);
        }
        else
        { // Parent
            if (i != pipes.size())
                close(pipes[i].descriptor[PipeInfo::OUT_DESCRIPTOR]);

            if (runInBackground)
            {
                m_bgProcessList.push_back(ProcessInfo(pid, true));
                std::cout << "[" << m_bgProcessList.size() - 1 << "] process " << pid << std::endl;
            }
            else // Ensure the child process is done before continuing if it's not a background process
                waitpid(pid, NULL, 0);
        }
    }

    // Close all file descriptors
    for (size_t i = 0; i < pipes.size(); ++i)
        close(pipes[i].descriptor[PipeInfo::IN_DESCRIPTOR]);
}

/*!*****************************************************************************
 * @brief Finish command: wait for a background process to finish.
 * @param _tokenList The list of data to read in, the process ID to wait for.
 *******************************************************************************/
void uShell3::finish(const TokenList &_tokenList)
{
    if (_tokenList.size() != 2)
    {
        std::cerr << "Error: no such process index." << std::endl;
        return;
    }

    int processIndex;
    try
    {
        processIndex = std::stoi(_tokenList[1]);
        if (processIndex < 0 || static_cast<size_t>(processIndex) >= m_bgProcessList.size())
            throw std::invalid_argument("");
    }
    catch (...)
    {
        std::cerr << "Error: no such process index." << std::endl;
        return;
    }

    ProcessInfo &selectedProcess = m_bgProcessList[processIndex];
    if (!selectedProcess.bActive)
    {
        std::cerr << "Process Index " << processIndex << " process " << selectedProcess.PID
                  << " is no longer a child process." << std::endl;
        return;
    }

    int exitStatus;
    if (waitpid(selectedProcess.PID, &exitStatus, 0) != -1)
    {
        std::cout << "process " << selectedProcess.PID
                  << " exited with exit status " << WEXITSTATUS(exitStatus) << std::endl;
        selectedProcess.bActive = !selectedProcess.bActive;
    }
    else
    {
        std::cerr << "Error: Failed to wait for process " << processIndex << std::endl;
    }
}

/*!*****************************************************************************
 * @brief Creates the class object of uShe113
 * @param _bFlag boolean value to decide whether to echo input
 *******************************************************************************/
uShell3::uShell3(bool _bFlag) : uShell2(_bFlag)
{
    // Set the prompt
    m_prompt = "uShell";
    // Set the internal command list
    m_internalCmdList3["finish"] = &uShell3::finish;
}

/*!*****************************************************************************
 * @brief Public function for external call. Execute in loops and waits for input
 * @return int Exit code, of the exit command
 *******************************************************************************/
int uShell3::run()
{
    // Main loop: continue until the exit flag is set
    while (!m_exit)
    {
        // Print the shell prompt
        std::cout << m_prompt << ">";
        // Read user input
        std::string input;
        if (getInput(input))
        {
            // Tokenize the input into a list of tokens
            TokenList tokens;
            tokenize(input, tokens);
            // Print the verbose version of the input if in verbose mode
            if (m_verbose)
                printVerbose(input);
            // Replace variables in the token list with their values
            if (!replaceVars(tokens))
                continue;
            // Skip empty input lines
            if (tokens.empty())
                continue;

            // Check command priority: uShell3 > uShell2 > uShell
            // First, check for uShell3 internal commands
            if (m_internalCmdList3.find(tokens[0]) != m_internalCmdList3.end())
                (this->*m_internalCmdList3[tokens[0]])(tokens);
            // Next, check for uShell2 internal commands
            else if (m_internalCmdList2.find(tokens[0]) != m_internalCmdList2.end())
                (this->*m_internalCmdList2[tokens[0]])(tokens);
            // Finally, check for uShell internal commands
            else if (m_internalCmdList.find(tokens[0]) != m_internalCmdList.end())
                (this->*m_internalCmdList[tokens[0]])(tokens);
            // If none of the internal commands match, then execute as external command
            else
                doExternalCmd(tokens);

            // Check if the exit flag is set after executing a command
            if (m_exit)
                break;
        }
        else
            break; // Break out of the loop if input retrieval fails
    }
    // Return the exit code when the shell terminates
    return m_exitCode;
}