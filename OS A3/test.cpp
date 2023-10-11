#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sstream>
#include "uShell3.h"

uShell3::ProcessInfo::ProcessInfo()
{
    PID = 0;
    bActive = false;
}

uShell3::ProcessInfo::ProcessInfo(int _id, bool _state)
{
    PID = _id;
    bActive = _state;
}

bool uShell3::exist(const TokenList &_tokenList, unsigned _start, unsigned _end)
{
    // Ensure the bounds are valid
    if (_start >= _tokenList.size() || _end >= _tokenList.size())
        return false;

    std::string command = _tokenList[_start];

    // If it has a '/', it's treated as a path (either relative or absolute)
    if (command.find('/') != std::string::npos)
    {
        struct stat buffer;
        return (stat(command.c_str(), &buffer) == 0);
    }
    else
    {
        const char *path = getenv("PATH");
        if (!path)
            return false;

        // Split the PATH variable based on ':' delimiter
        std::string pathStr(path);
        std::istringstream ss(pathStr);
        std::string token;

        while (std::getline(ss, token, ':'))
        {
            std::string fullPath = token + "/" + command;

            struct stat buffer;
            if (stat(fullPath.c_str(), &buffer) == 0)
                return true; // Command exists in this directory
        }
    }
    return false;
}

void uShell3::doExternalCmd(const TokenList &_tokenList)
{
    // Signal handling using a lambda for background processes
    auto handle_finished_processes = [&]()
    {
        int status;
        pid_t done;
        while (true)
        {
            done = waitpid(-1, &status, WNOHANG);
            if (done == 0)
            {
                break; // no child has exited yet, but they're still running
            }
            if (done == -1)
            {
                if (errno == ECHILD)
                    break; // no more child processes
            }
            else
            {
                for (auto &process : m_bgProcessList)
                {
                    if (process.PID == done)
                    {
                        process.bActive = false;
                        // Output process status and other details...
                        if (WIFEXITED(status))
                        {
                            int exitStatus = WEXITSTATUS(status);
                            std::cout << "background process " << process.PID << " exited with exit status " << exitStatus << "." << std::endl;
                        }
                        else if (WIFSIGNALED(status))
                        {
                            int signalNumber = WTERMSIG(status);
                            std::cout << "background process " << process.PID << " was killed by signal " << signalNumber << "." << std::endl;
                        }
                    }
                }
            }
        }
    };

    TokenList mutableTokens = _tokenList; // Create a copy

    // 1. Split the commands
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> currentCommand;
    for (const auto &token : mutableTokens)
    {
        if (token == "|")
        {
            commands.push_back(currentCommand);
            currentCommand.clear();
        }
        else
        {
            currentCommand.push_back(token);
        }
    }
    commands.push_back(currentCommand); // push the last command

    // Check for consecutive pipes or pipe at the start or end
    if (mutableTokens[0] == "|" || mutableTokens[mutableTokens.size()-1] == "|") {
        std::cerr << "Error: syntax error" << std::endl;
        return;
    }

    for (size_t i = 1; i < mutableTokens.size(); ++i) {
        if (mutableTokens[i] == "|" && mutableTokens[i-1] == "|") {
            std::cerr << "Error: syntax error" << std::endl;
            return;
        }
    }

    // 2. Create a pipe for each pair of commands
    std::vector<PipeInfo> pipes(commands.size() - 1);
    for (PipeInfo &pipeInfo : pipes)
    {
        if (pipe(pipeInfo.descriptor) == -1)
        {
            perror("pipe");
            return;
        }
    }

    // Add this flag before you start processing commands
    bool commandFailed = false;

    // 3. Execute commands
    std::vector<pid_t> children;
    for (unsigned i = 0; i < commands.size(); ++i)
    {
        if (commandFailed) // Skip the creation of child processes if a command already failed.
            break;

        pid_t pid = fork();
        if (pid == 0)
        { // Child
            // Redirect input if not the first command
            if (i != 0)
            {
                dup2(pipes[i - 1].descriptor[PipeInfo::IN_DESCRIPTOR], STDIN_FILENO);
                close(pipes[i - 1].descriptor[PipeInfo::OUT_DESCRIPTOR]);
            }
            // Redirect output if not the last command
            if (i != commands.size() - 1)
            {
                dup2(pipes[i].descriptor[PipeInfo::OUT_DESCRIPTOR], STDOUT_FILENO);
                close(pipes[i].descriptor[PipeInfo::IN_DESCRIPTOR]);
            }
            // Close all pipes
            for (PipeInfo &pipeInfo : pipes)
            {
                close(pipeInfo.descriptor[PipeInfo::IN_DESCRIPTOR]);
                close(pipeInfo.descriptor[PipeInfo::OUT_DESCRIPTOR]);
            }
            // Execute command
            std::vector<char *> args;
            for (const auto &arg : commands[i])
            {
                args.push_back(const_cast<char *>(arg.c_str()));
            }
            args.push_back(nullptr); // NULL-terminate the arguments

            execvp(args[0], args.data());
            // If execvp fails
            std::cerr << "Error: " << args[0] << " cannot be found" << std::endl;
            _exit(EXIT_FAILURE);
        }
        else if (pid < 0)
        {
            perror("fork");
            return;
        }
        else
        {
            children.push_back(pid);
            // Close write end for the parent
            if (i != commands.size() - 1)
            {
                close(pipes[i].descriptor[PipeInfo::OUT_DESCRIPTOR]);
            }
        }
    }

    // Parent: Wait for all children to finish
    for (pid_t child : children)
    {
        int status;
        if (waitpid(child, &status, 0) == -1) 
        {
            perror("waitpid");
            return;
        }

        // If a command failed, set the commandFailed flag to true
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            commandFailed = true;
        }
    }

    // Handle background processes (if any)
    handle_finished_processes();
}

void uShell3::finish(const TokenList &_tokenList)
{

    // 1. Check if the finish command has parameters
    if (_tokenList.size() < 2)
    {
        std::cerr << "Error: Missing process index for finish command." << std::endl;
        return;
    }

    int processIndex;
    try
    {
        // 2. Convert the argument to a process index
        processIndex = std::stoi(_tokenList[1]);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: no such process index." << std::endl;
        return;
    }

    // 3. Validate process index
    if (processIndex < 0 || processIndex >= static_cast<int>(m_bgProcessList.size()))
    {
        std::cerr << "Error: no such process index." << std::endl;
        return;
    }

    ProcessInfo &pInfo = m_bgProcessList[processIndex];

    // 4. Check if the process is still active
    if (!pInfo.bActive)
    {
        std::cout << "Process Index " << processIndex << " process " << pInfo.PID
                  << " is no longer a child process." << std::endl;
        return;
    }

    // 5. Wait for the process if it's still active
    int status;
    pid_t waitedPID = waitpid(pInfo.PID, &status, 0);
    if (waitedPID == -1)
    {
        perror("waitpid");
        return;
    }

    // 6. Print the exit status and other details
    if (WIFEXITED(status))
    {
        int exitStatus = WEXITSTATUS(status);
        std::cout << "process " << pInfo.PID << " exited with exit status " << exitStatus << "." << std::endl;
    }
    else if (WIFSIGNALED(status))
    {
        int signalNumber = WTERMSIG(status);
        std::cout << "process " << pInfo.PID << " was killed by signal " << signalNumber << "." << std::endl;
    }

    // 7. Mark the process as inactive and remove it from the list
    pInfo.bActive = false;
}

uShell3::uShell3(bool _bFlag) : uShell2(_bFlag)
{
    // Set the prompt
    m_prompt = "uShell";
    // Set the internal command list
    m_internalCmdList3["finish"] = &uShell3::finish;
}

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



#if 0
void uShell3::doExternalCmd(const TokenList &_tokenList)
{
    //Return early if first token is "|"
    if (_tokenList.front() == "|" || _tokenList.empty())
        return;

    // Signal handling using a lambda for background processes
    auto handle_finished_processes = [&]()
    {
        int status;
        pid_t done;
        while (true)
        {
            done = waitpid(-1, &status, WNOHANG);
            if (done == 0)
            {
                break; // no child has exited yet, but they're still running
            }
            if (done == -1)
            {
                if (errno == ECHILD)
                    break; // no more child processes
            }
            else
            {
                for (auto &process : m_bgProcessList)
                {
                    if (process.PID == done)
                    {
                        process.bActive = false;
                        // Output process status and other details...
                        if (WIFEXITED(status))
                        {
                            int exitStatus = WEXITSTATUS(status);
                            std::cout << "background process " << process.PID << " exited with exit status " << exitStatus << "." << std::endl;
                        }
                        else if (WIFSIGNALED(status))
                        {
                            int signalNumber = WTERMSIG(status);
                            std::cout << "background process " << process.PID << " was killed by signal " << signalNumber << "." << std::endl;
                        }
                    }
                }
            }
        }
    };

    TokenList mutableTokens = _tokenList; // Create a copy
    bool runInBackground = false;
    if (!mutableTokens.empty() && mutableTokens.back() == "&")
    {
        runInBackground = true;
        mutableTokens.pop_back();
    }

    // 1. Split the commands
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> currentCommand;
    unsigned syntaxErrors = 0;
    for (const auto &token : mutableTokens)
    {
        if (token == "|")
        {
            if (currentCommand.empty())
                syntaxErrors++;
            commands.push_back(currentCommand);
            currentCommand.clear();
        }
        else
        {
            currentCommand.push_back(token);
        }
    }
    if (!currentCommand.empty())
        commands.push_back(currentCommand); // push the last command

    // 2. Create a pipe for each pair of commands
    std::vector<PipeInfo> pipes(commands.size() - 1);
    for (PipeInfo &pipeInfo : pipes)
    {
        if (pipe(pipeInfo.descriptor) == -1)
        {
            perror("pipe");
            return;
        }
    }

    for (const auto &command : commands)
{
    for (const auto &token : command)
        std::cerr << "'" << token << "' ";
    std::cerr << std::endl;
}
    // Add this flag before you start processing commands
    bool commandFailed = false;

    // 3. Execute commands
    std::vector<pid_t> children;
    for (unsigned i = 0; i < commands.size(); ++i)
    {
        if (commandFailed)
            break;

        // Check for missing command between pipes, at the start, or end
        if (commands[i].empty())
        {
            std::cerr << "Error: syntax error" << std::endl;
            return; // Exit the function early
        }

        if (commands[i+1].size() == 1 && commands[i+1][0] == "&")
        {
            std::cerr << "Error: & cannot be found" << std::endl;
            return;
        }

        // Check if command exists
        if (!exist(commands[i], 0, commands[i].size() - 1))
        {
            std::cerr << "Error: " << commands[i][0] << " cannot be found" << std::endl;
            commandFailed = true;
        }

        // Check for missing command between pipes or at the start
        if (commands[i].empty() || (i == 0 && commands[i][0] == "|") || (i == commands.size() - 1 && commands[i][0] == "|"))
        {
            std::cerr << "Error: syntax error" << std::endl;
            continue; // Move to the next iteration
        }

        if (syntaxErrors > i)
        {
            std::cerr << "Error: syntax error" << std::endl;
            continue; // Move to the next iteration
        }
        
        if (commandFailed)
            break;
        
        pid_t pid = fork();
        if (pid == 0)
        { // Child
            // Redirect input if not the first command
            if (i != 0)
            {
                dup2(pipes[i - 1].descriptor[PipeInfo::IN_DESCRIPTOR], STDIN_FILENO);
                close(pipes[i - 1].descriptor[PipeInfo::OUT_DESCRIPTOR]);
            }
            // Redirect output if not the last command
            if (i != commands.size() - 1)
            {
                dup2(pipes[i].descriptor[PipeInfo::OUT_DESCRIPTOR], STDOUT_FILENO);
                close(pipes[i].descriptor[PipeInfo::IN_DESCRIPTOR]);
            }
            // Close all pipes
            for (PipeInfo &pipeInfo : pipes)
            {
                close(pipeInfo.descriptor[PipeInfo::IN_DESCRIPTOR]);
                close(pipeInfo.descriptor[PipeInfo::OUT_DESCRIPTOR]);
            }
            // Execute command
            std::vector<char *> args;
            for (const auto &arg : commands[i])
            {
                args.push_back(const_cast<char *>(arg.c_str()));
            }
            args.push_back(nullptr); // NULL-terminate the arguments

            execvp(args[0], args.data());
            // If execvp fails
            std::cerr << "Error: " << args[0] << " cannot be found" << std::endl;
            _exit(127);
        }
        else if (pid < 0)
        {
            perror("fork");
            return;
        }
        else
        {
            children.push_back(pid);
            // Close write end for the parent
            if (i != commands.size() - 1)
            {
                close(pipes[i].descriptor[PipeInfo::OUT_DESCRIPTOR]);
            }
            if (runInBackground)
            {
                m_bgProcessList.push_back(ProcessInfo(pid, true));
                std::cout << "[" << m_bgProcessList.size() - 1 << "] process " << pid << std::endl;
                return;
            }
        }
        commandFailed = false;
    }

    // // Parent: Wait for all children to finish
    if (!runInBackground)
    {
        for (pid_t child : children)
        {
            int status;
            if (waitpid(child, &status, 0) == -1) 
            {
                perror("waitpid");
                return;
            }

            // If a command failed, set the commandFailed flag to true
            if (WIFEXITED(status) && WEXITSTATUS(status) == 127)
            {
                commandFailed = true;
                for (pid_t killChild : children)
                {
                    if (killChild != child)  // Don't kill the child that's already terminated
                        kill(killChild, SIGTERM);  // Send the termination signal
                }
                break;
            }
        }
    }

    // // Handle background processes (if any)
    handle_finished_processes();
}
#endif