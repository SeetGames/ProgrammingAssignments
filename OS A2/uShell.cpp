/*!************************************************************************
 \file uShell.cpp
 \author Seetoh Wei Tung
 \par Email: seetoh.w@digipen.edu
 \par Course: CSD2182
 \par Assignment #2
 \date 17-9-2023
 \brief Continuation of uShell assignment. This file contains the
    implementation of the uShell2 class. It inherits from uShell and
    adds additional functionality to the shell. This includes the
    changeprompt and exit commands.
**************************************************************************/

#include "uShell2.h"
#include <iostream>
#include <unistd.h> // for execvp
#include <sys/types.h>
#include <sys/wait.h> // for waitpid
#include <sstream>
#include <cstring>
#include <sys/stat.h> // for struct stat and stat()

/**
 * @brief Constructor for uShell2 class that initializes internal command list.
 * 
 * @param bFlag Boolean flag to enable/disable debug mode.
 */
uShell2::uShell2(bool bFlag) : uShell(bFlag)
{
    m_internalCmdList2["changeprompt"] = &uShell2::changePrompt;
    m_internalCmdList2["exit"] = &uShell2::exit;
}

/**
 * @brief Changes the prompt of the uShell.
 * 
 * @param tokenList A list of tokens containing the new prompt.
 * If the list is empty or contains only one token, the prompt is set to an empty string.
 * If the list contains a '#' token, the prompt is set to all the tokens before the '#' token (excluding the '#' token).
 * If the list does not contain a '#' token, the prompt is set to all the tokens in the list.
 */
void uShell2::changePrompt(TokenList const &tokenList)
{
    if (tokenList.size() < 2)
    {
        m_prompt = "";
        return;
    }

    // Start from the second token and concatenate all tokens until a '#' is found or the end is reached
    std::string newPrompt;
    for (size_t i = 1; i < tokenList.size(); ++i)
    {
        if (tokenList[i] == "#")
            break;
        newPrompt += tokenList[i];
        if (i != tokenList.size() - 1) // If not the last token, add a space
            newPrompt += " ";
    }
    m_prompt = newPrompt;
}

/**
 * Exits the uShell2 program with an optional exit code.
 * 
 * @param tokenList A list of tokens containing the exit code (optional).
 *                  If the list contains more than 2 tokens, the function returns without doing anything.
 *                  If the list contains 2 tokens, the second token is parsed as an integer and used as the exit code.
 *                  If the second token is not a valid integer, the function returns without doing anything.
 *                  If the list contains only 1 token, the default exit code of 0 is used.
 */
void uShell2::exit(TokenList const &tokenList)
{
    if (tokenList.size() > 2)
    {
        return;
    }

    if (tokenList.size() == 2)
    {
        try
        {
            m_exitCode = std::stoi(tokenList[1]);
        }
        catch (const std::exception &)
        {
            return;
        }
    }
    else
    {
        m_exitCode = 0; // Default exit value
    }

    m_exit = true;
}

/**
 * Executes the command specified in the tokenList with the given parameters.
 * 
 * @param tokenList The list of tokens representing the command and its arguments.
 * @param startParam The index of the first argument in the tokenList.
 * @param endParam The index of the last argument in the tokenList.
 */
void uShell2::execute(TokenList const &tokenList, unsigned startParam, unsigned endParam)
{
    std::string commandToExecute = tokenList[0];
    char **args = new char *[endParam - startParam + 2]; // +1 for the command itself and +1 for the NULL terminator
    args[0] = const_cast<char *>(tokenList[0].c_str()); // Use the original command name
    for (unsigned i = startParam; i <= endParam; ++i)
    {
        args[i - startParam] = const_cast<char *>(tokenList[i].c_str());
    }
    args[endParam - startParam + 1] = NULL; // NULL terminate the argument list

    if (tokenList[0].find('/') == 0 || tokenList[0].find("./") == 0)
    {
        // Absolute path
        execvp(tokenList[startParam].c_str(), args);
    }
    else
    {
        // Search in PATH variable
        std::string pathVar = m_vars["PATH"];
        std::istringstream ss(pathVar);
        std::string path;
        while (std::getline(ss, path, ':'))
        {
            std::string fullPath = path + "/" + tokenList[0];
            commandToExecute = fullPath; // Use the full path for execution
            execvp(fullPath.c_str(), args);
        }
    }
    std::cerr << "Error: " << tokenList[0] << " cannot be found" << std::endl;
    delete[] args; // Clean up the dynamically allocated memory
}

/**
 * Executes an external command by forking a child process and waiting for it to finish.
 * @param tokenList A list of tokens representing the command and its arguments.
 */
void uShell2::doExternalCmd(TokenList const &tokenList)
{
    std::cout << std::flush; // Flush the output before forking

    pid_t pid = fork(); // Create a child process

    if (pid == 0)
    {
        // Child process
        execute(tokenList, 0, tokenList.size() - 1);
        _exit(0); // Exit with an error code
    }
    else if (pid < 0)
    {
        // Fork failed
        std::cerr << "Error: Failed to fork child process." << std::endl;
    }
    else
    {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to finish
    }
}

/**
 * @brief Executes the uShell2 program.
 * 
 * @return int Exit code of the program.
 */
int uShell2::run()
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
            // Check if the first token corresponds to an internal command of uShell2
            if (m_internalCmdList2.find(tokens[0]) != m_internalCmdList2.end())
                (this->*m_internalCmdList2[tokens[0]])(tokens);
            else if (m_internalCmdList.find(tokens[0]) != m_internalCmdList.end()) // If not an internal command of uShell2, check if it's an internal command of uShell
                (this->*m_internalCmdList[tokens[0]])(tokens);
            else // If it's neither an internal command of uShell2 nor uShell, treat it as an external command
                doExternalCmd(tokens);

            // Check if the exit flag is set after executing a command
            if (m_exit)
                break;
        }
        else
        {
            break; // Break out of the loop if input retrieval fails
        }
    }
    // Return the exit code when the shell terminates
    return m_exitCode;
}