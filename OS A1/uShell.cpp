#include "uShell.h"
#include <iostream>
#include <sstream>
#include <algorithm>

// Constructor
uShell::uShell(bool bFlag) 
: m_prompt("uShell>"), 
m_verbose(bFlag), 
m_exitFlag(false), 
m_exitCode(0)
{
    m_internalCmdList["echo"] = &uShell::echo;
    m_internalCmdList["setVar"] = &uShell::setVar;
    m_vars["PATH"] = ""; // Initialize PATH variable or set to a default value
}

bool uShell::getInput(std::string& _input) const
{
    std::getline(std::cin, _input);
    if (_input.back() == '\r')
        _input.pop_back();
    return !std::cin.eof();
}

void uShell::printVerbose(std::string const& _input)
{
    auto start = _input.find_first_not_of(' ');
    auto end = _input.find_last_not_of(' ');
    if (start != std::string::npos && end != std::string::npos)
        std::cout << _input.substr(start, end - start + 1) << std::endl;
}

void uShell::tokenize(std::string const& _input, TokenList& _tokenList) const
{
    std::istringstream iss(_input);
    std::string token;
    while (iss >> token)
        _tokenList.push_back(token);
}

bool uShell::replaceVars(TokenList& _tokenList) const
{
    for (auto& token : _tokenList)
    {
        // Check for comment sign and remove all following tokens
        if (token.find("#") != std::string::npos)
        {
            _tokenList.erase(std::remove_if(_tokenList.begin(), _tokenList.end(),
                [&](const std::string& t) { return t.find("#") != std::string::npos; }),
                _tokenList.end());
            break;
        }

        size_t startPos = token.find("${");
        size_t endPos = token.find("}", startPos);
        while (startPos != std::string::npos && endPos != std::string::npos)
        {
            std::string varName = token.substr(startPos + 2, endPos - startPos - 2);
            
            // Check for valid variable name
            if (!isValidVarname(varName.c_str(), varName.c_str() + varName.size()))
            {
                std::cerr << "Error: Invalid variable name " << varName << std::endl;
                return false;
            }

            try 
            {
                token.replace(startPos, endPos - startPos + 1, m_vars.at(varName));
            } 
            catch (const std::out_of_range&) 
            {
                std::cerr << "Error: Variable " << varName << " not found." << std::endl;
                return false;
            }
            startPos = token.find("${", endPos);
            endPos = token.find("}", startPos);
        }
    }
    return true;
}

bool uShell::isValidVarname(char const* _start, char const* _end) const
{
    if (!std::isalpha(*_start))
    {
        return false;
    }
    _start++;
    while (_start != _end)
    {
        if (!std::isalnum(*_start))
        {
            return false;
        }
        _start++;
    }
    return true;
}

std::string uShell::mergeTokens(TokenList const& _tokenList, unsigned _startPos) const
{
    std::ostringstream oss;
    for (unsigned i = _startPos; i < _tokenList.size(); ++i)
    {
        oss << _tokenList[i];
        if (i != _tokenList.size() - 1)  // Check if it's not the last token
        {
            oss << " ";
        }
    }
    std::string merged = mergeTokens(_tokenList, 0);
    std::cout << "Merged tokens: [" << merged << "]" << std::endl;
    return oss.str();
}

void uShell::echo(TokenList const& _tokenList)
{
    for (unsigned i = 1; i < _tokenList.size(); ++i)
    {
        std::cout << _tokenList[i];
        if (i != _tokenList.size() - 1)  // Check if it's not the last token
        {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void uShell::setVar(TokenList const& _tokenList)
{
    if (_tokenList.size() < 2)
    {
        std::cerr << "Error: Invalid setVar command." << std::endl;
        return;
    }
    std::string varName = _tokenList[1];
    std::string value = (_tokenList.size() > 2) ? mergeTokens(_tokenList, 2) : "";
    if (isValidVarname(varName.c_str(), varName.c_str() + varName.size()))
    {
        m_vars[varName] = value;
    }
    else
    {
        std::cerr << "Error: Invalid variable name." << std::endl;
    }
}

int uShell::run()
{
    while (!m_exitFlag)
    {
        std::cout << m_prompt << " ";  // Use the member variable for the prompt
        std::string input;
        if (getInput(input))
        {
            TokenList tokens;
            tokenize(input, tokens);
            if (m_verbose)
                printVerbose(input);
            if (!replaceVars(tokens))
                continue;
            if (tokens.empty())
                continue;
            if (m_internalCmdList.find(tokens[0]) != m_internalCmdList.end())
                (this->*m_internalCmdList[tokens[0]])(tokens);
            else
            {
                // External command handling can be added here
            }

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

                m_exitFlag = true;  // Set the flag to terminate the loop
                break;  // Exit the loop immediately
            }
        }
        else
            break;
    }
    return m_exitCode;
}
