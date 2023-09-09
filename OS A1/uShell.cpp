/*!************************************************************************
 \file uShell.cpp
 \author Seetoh Wei Tung
 \par Email: seetoh.w@digipen.edu
 \par Course: CSD2182
 \par Ass #1
 \date 9-9-2023
 \brief A basic shell program called uShell. A shell program behaves 
 typically in a similar fashion to a command prompt.
**************************************************************************/

#include "uShell.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Constructor
uShell::uShell(bool bFlag) 
: m_prompt("uShell>"), 
m_verbose(bFlag), 
m_exit(false), 
m_exitCode(0)
{
    // Initialize the internal command list
    m_internalCmdList["echo"] = &uShell::echo;
    m_internalCmdList["setvar"] = &uShell::setVar;
    m_vars["PATH"] = ""; // Initialize PATH variable or set to a default value
}

bool uShell::getInput(std::string& _input) const
{
    // Try to get a line of input from the user
    if (!std::getline(std::cin, _input))
        return false;
    // Check for and remove the carriage return character (if present)
    if (_input.back() == '\r')
        _input.pop_back();
    // Return true to indicate successful input retrieval
    return true;
}

void uShell::printVerbose(std::string const& _input)
{
    // Find the first non-space character in the input string
    auto start = _input.find_first_not_of(' ');
    // Find the last non-space character in the input string
    auto end = _input.find_last_not_of(' ');
    // Check if both start and end positions were found
    if (start != std::string::npos && end != std::string::npos)
        // Print the trimmed portion of the input string
        std::cout << _input.substr(start, end - start + 1) << std::endl;
}

void uShell::tokenize(std::string const& _input, TokenList& _tokenList) const
{
    // Create a string stream to break the input into tokens
    std::istringstream iss(_input);
    std::string token;
    // Iterate through the input stream and extract tokens
    while (iss >> token)
        // Add the extracted token to the token list
        _tokenList.push_back(token);
}

bool uShell::replaceVars(TokenList& _tokenList) const
{
    // Loop through each token in the token list
    for (auto& token : _tokenList)
    {
        // Check for comment sign and remove all following tokens
        if (token == "#")
        {
            _tokenList.erase(_tokenList.begin() + (&token - &_tokenList[0]), _tokenList.end());
            break;
        }
        // Find the starting position of a variable expression "${"
        size_t startPos = token.find("${");
        // Find the ending position of the variable expression "}"
        size_t endPos = token.find("}", startPos);
        // Process variable expressions within the token
        while (startPos != std::string::npos && endPos != std::string::npos)
        {
            // Extract the variable name from the expression
            std::string varName = token.substr(startPos + 2, endPos - startPos - 2);

            // Check for valid variable name
            if (isValidVarname(varName.c_str(), varName.c_str() + varName.size()))
            {
                try 
                {
                    // Replace the variable expression with its value
                    token.replace(startPos, endPos - startPos + 1, m_vars.at(varName));
                } 
                catch (const std::out_of_range&) 
                {
                    // Handle the case where the variable is not defined
                    std::cerr << "Error: " << varName << " is not a defined variable." << std::endl;
                    return false;
                }
            }
            // Find the next variable expression within the token
            startPos = token.find("${", startPos + 2);
            endPos = token.find("}", startPos);
        }
    }
    // Return true to indicate successful variable replacement
    return true;
}

bool uShell::isValidVarname(char const* _start, char const* _end) const
{
    // Check if the first character of the variable name is an alphabet character
    if (!std::isalpha(*_start))
    {
        return false; // Return false if the variable name doesn't start with a letter
    }
    // Move to the next character in the variable name
    _start++;
    // Loop through the remaining characters in the variable name
    while (_start != _end)
    {
        // Check if the character is not an alphanumeric character
        if (!std::isalnum(*_start))
        {
            // Return false if a non-alphanumeric character is found
            return false;
        }
        // Move to the next character
        _start++;
    }
    // Return true to indicate that the variable name is valid
    return true;
}

std::string uShell::mergeTokens(TokenList const& _tokenList, unsigned _startPos) const
{
    // Create a string stream to build the merged string
    std::ostringstream oss;
    // Iterate through the tokens starting from the given position
    for (unsigned i = _startPos; i < _tokenList.size(); ++i)
    {
        // Append the current token to the merged string
        oss << _tokenList[i];
         // Check if it's not the last token to add a space separator
        if (i != _tokenList.size() - 1)  // Check if it's not the last token
        {
            oss << " ";
        }
    }
    // Return the merged string
    return oss.str();
}

void uShell::echo(TokenList const& _tokenList) 
{
    // Iterate through the tokens, starting from the second token
    for (auto token{_tokenList.cbegin() + 1}; token != _tokenList.cend(); ++token)
    {
        // Print the current token
        std::cout << *token;
        // Add a space separator if it's not the last token
        if (token == _tokenList.cend() - 1)
            std::cout << "\n";
        else 
            std::cout << " ";
    }
}

void uShell::setVar(TokenList const& _tokenList)
{
    // Check if there are at least two tokens (command and variable name)
    if (_tokenList.size() < 2)
    {
        std::cerr << "Error: Invalid setVar command." << std::endl;
        return;
    }
     // Extract the variable name from the second token
    std::string varName = _tokenList[1];
    // Extract the value from the remaining tokens, if available
    std::string value = (_tokenList.size() > 2) ? mergeTokens(_tokenList, 2) : "";
    // Check if the variable name is valid
    if (isValidVarname(varName.c_str(), varName.c_str() + varName.size()))
    {
        // Set the variable with the specified name to the provided value
        m_vars[varName] = value;
    }
    else
    {
        // Handle the case where the variable name is invalid
        std::cerr << "Error: Invalid variable name." << std::endl;
    }
}

int uShell::run()
{
    // Main loop: continue until the exit flag is set
    while (!m_exit)
    {
        // Print the shell prompt
        std::cout << m_prompt;
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
            // Check if the first token corresponds to an internal command
            if (m_internalCmdList.find(tokens[0]) != m_internalCmdList.end())
                (this->*m_internalCmdList[tokens[0]])(tokens);

            // Check if the first token is the "exit" command
            if (tokens[0] == "exit")
            {
                if (tokens.size() > 2)
                {
                    std::cerr << "Error: Too many arguments for exit command." << std::endl;
                    continue;
                }

                if (tokens.size() == 2)
                {
                    try
                    {
                        // Parse and set the exit code if provided
                        m_exitCode = std::stoi(tokens[1]);
                    }
                    catch (const std::exception&)
                    {
                        std::cerr << "Error: Invalid exit code." << std::endl;
                        continue;
                    }
                }
                else
                {
                    m_exitCode = 0;  // Default exit value
                }

                m_exit = true;  // Set the flag to terminate the loop
                break;  // Exit the loop immediately
            }
        }
        else
            break; // Break out of the loop if input retrieval fails
    }
    // Return the exit code when the shell terminates
    return m_exitCode;
}