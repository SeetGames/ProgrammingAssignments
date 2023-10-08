#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sstream>
#include "uShell3.h"

/*!*****************************************************************************
 * @brief 1 Introduction to A 1.3
    In A 1.3, thc student is expected to extend the same uShe11 program in A 1.1 and A 1.2. In
    particular, the student is expected to implement the following:
    - An internal command called finish, to force the shell program to wait for background
    processes.
    - Run background processes by supporting a syntax when a word " is appended to
    the end of the command line.
    - Support multiple pipes between commands.
    In this specification, we shall concentrate on describing these additional features.
    Please consult A1.1 and A 1.2 for the other specifications.

    2 Background commands and synchronization
    The default behaviour of external commands is that they are executed before the next
    command is read. However, background jobs provide an opportunity for the user to launch
    jobs that the user does not have to wait for before running the next command. An example is
    grep, which is a command-line utility for searching plain-text data sets for lines that match
    a regular expression. Usually, one would need to finish searching before continuing, but
    background jobs give us a way to avoid that problem. The syntax for running a background
    job is by adding a “& ” to the end of the command. When a background job is launched,
    the shell program responds by printing the process index (internal to the shell) and the
    process id (via the function getpid()). For example:
    uShell> grep shell *.cpp & # grep will search cpp file for the string of shell
    [0] process 1496
    uShell> # grep is the first background job to launch, hence 0.
    uShell> # grep’s process id from getpid is 1496.
    uShell> grep shell *.cpp& # wrong use of the &
    grep: *.cpp&: No such file or directory

    2.1 finish - Internal Command
    The internal command that can be used in connection with this is the internal command
    finish. finish forces the shell program to wait for particular background processes to
    complete before continuing. The format for using finish is finish <process index>. The
    given process ID must be a valid one. For example:
    uShell>gedit &
    [0] process 10064
    uShell>finish 0
    process 10064 exited with exit status 0.
    uShell>gedit &
    [1] 12456
    uShell>finish 0
    Process Index 0 process 10064 is no longer a child process.
    uShell>finish 1
    process 12456 exited with exit status 0.

    2.2 Pipes
    Finally, an additional feature of uShell is the support of pipes. This is also a composite
    external command, where multiple external commands are executed concurrently. The
    syntax for pipes is the following:
    ext_cmd1 | ext_cmd2 | ext_cmd3 | ...
    where each ext cmd is a sequence of words that form any legitimate external command as
    described above. The only difference is that these external commands cannot be background
    jobs (but you are not expected to perform error handling of this). You do not have to support
    piping with internal commands, although that may be a useful extension to implement.
    Semantically, what the syntax mean is that the output of the preceding external commands
    is fed into the succeeding external commands. So the output of ext cmd1 is feeding
    into the input of ext cmd2 and so on and so forth. It is important to note that these
    commands are running concurrently, so there is no need to wait for ext cmd1 to complete
    before ext cmd2 runs. Finally, the last external command in the chain of pipes outputs to
    the stdout. Here are some examples of pipes:
    uShell>ls
    a config.obj my_setenv.c a.stackdump my_setenv.h
    my_setenv.o main.cpp main.o Makefile
    uShell>ls | grep setenv
    my_setenv.c
    my_setenv.h
    my_setenv.o
    uShell>ls | grep2 setenv | wc
    Error: grep2 cannot be found
    uShell>ls | grep setenv | wc

    3 uShell3.h
    You will finish the following definition of member functions of the class uShell3 in uShell.cpp,
    which is the only file required to submit. The classes uShell and uShell2, which are de-
    clared in Part 1 and 2, has been defined in uShell ref.cpp and uShell2 ref.cpp, respec-
    tively. The OBJ files are provided in VPL as uShell ref.obj and uShell2 ref.obj for
    linking. You are not allowed to modify the files uShell_ref.cpp and uShell2_ref.cpp.
    You are only allowed to modify uShell3.cpp. You are not allowed to modify uShell3.h.
 *
*******************************************************************************/

/*!*****************************************************************************
 * @brief Construct a new u Shell3::Process Info::Process Info object
 *******************************************************************************/
uShell3::ProcessInfo::ProcessInfo()
{
    PID = 0;
    bActive = false;
}

/*!*****************************************************************************
 * @brief Construct a new u Shell3::Process Info::Process Info object
 *
 * @param _id The process ID given by the OS
 * @param _state The state of the process to set
 *******************************************************************************/
uShell3::ProcessInfo::ProcessInfo(int _id, bool _state)
{
    PID = _id;
    bActive = _state;
}

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

/*!*****************************************************************************
 * @brief Calls an external command using the passed in parameters.
 *
 * The function checks for rare case where first token is a pipe, where nothing will be done.
 * It flushes the buffer and then count the number of pipes in the token list. It adds pipe
 * information to a list typed of std::vector<PipeInfo> and creates the pipe (using
 * pipe(). You may save the position of the pipe in the token list so that it can be used to
 * separate the arguments. Once it has obtained the number of processes required, it loops
 * over for further checking. It identifies where the beginning and ending position of the
 * parameters in the token list. Otherwise if there is only a single process, it checks whether
 * there is a background process token. It reports the syntax error if there is an empty string
 * between the pipes. The function checks whether the command in the given token list with
 * the specified starting and ending position of the parameters exists by calling exist() and
 * reports the error accordingly. It returns if there is an error.
 * Otherwise, it starts to create the child processes using fork(). For each fork(), if
 * background process is specified, the parent does not wait and adds the child to the list by
 * pushing into m bgProcessList. Please note you need to print out the status for the child
 * process. Otherwise if no background process is required, the parent just adds the child
 * PID to the list of the processes with piping. For the child process, it sets the arguments
 * to the input in the token list given to the process and call execute(). If the process is
 * not the first one to be created, it closes stdin and replaces with pipe in. If the process is
 * not the last one, it closes stdout and replaces with pipe out. It closes all pipes that are
 * not needed anymore when calling execute() given in uShell2.h. Then the child exits by
 * calling exit(EXIT FAILURE). The parent process should close all pipes before waiting for
 * all the child processes to be done.
 * @param _tokenList The list of tokens to get the data value from
 *******************************************************************************/
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

/*!*****************************************************************************
 * @brief Finish command: wait for a background process to finish.
 *
 * This function does finish command. It checks for finish command without parameters
 * and indicates the error. If the finish command is with the correct parameters, it convert
 * the argument to process id to wait for that process. It checks if the waited process ID is valid
 * in the m bgProcessList and also checks if the process to be waited for has already done
 * by checking bActive for the process. If everything is OK, it waits and gets the returned
 * status (using waitpid()). At the end it prints the status for the process and then set the
 * process active flag to false.
 * @param _tokenList The list of data to read in, the process ID to wait for.
 *******************************************************************************/
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

/*!*****************************************************************************
 * @brief Creates the class object of uShe113
 * It sets function pointers for finish in m internalCmdList3.
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
 * This function is enhanced based on the assignment A1.1. It has a loop and firstly check
 * whether exit command is called. If so, it stops. Otherwise it prints out the prompt, with
 * the right arrow. Then it gets user input. If there are no more lines from the input, it exits
 * from the program (assume it gets re-directed input from the files). Otherwise, it clears the
 * input buffer for next input. Obviously, it needs to skip if there is no input (e.g. empty line).
 * It starts to tokenize the input otherwise. After this, it prints the input if verbose mode is
 * set. It replaces all variables if possible. If the function call for replacement replaceVars()
 * returns false, an error has occurred, it continues to next line of input. Please note that
 * replacement also clears comments, so we have to check if the result is empty. Next, it
 * finds if it is an internal command in the first list m internalCmdList. If so, it activates
 * the internal command. Otherwise, it finds in the second list m internalCmdList2 and
 * activates the internal command if there is a match. Else, it continues to search the
 * internal command in the third list m internalCmdList3. If the search for the
 * internal command fails, it regards the command as external command and does
 * the external command accordingly.1 Please note that if the next char is EOF, we
 * should end the loop to exit. Outside the loop, it returns exit code m exitCode.
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
