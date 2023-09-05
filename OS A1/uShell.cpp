#include "uShell.h"
#include <iostream>
#include <sstream>

// Implementation of constructor
uShell::uShell(bool bFlag)
    : m_prompt("$ "), m_verbose(bFlag), m_exitCode(0)
{
    // Initialize internal command map here if needed
}

bool uShell::getInput(std::string &input) const {
    std::getline(std::cin, input);
    return !std::cin.eof();
}

void uShell::printVerbose(std::string const &input) {
    if (m_verbose) {
        std::cout << input << std::endl;
    }
}

void uShell::tokenize(std::string const &input, TokenList &tokenList) const {
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        tokenList.push_back(token);
    }
}

bool uShell::replaceVars(TokenList &tokenList) const {
    // Implement variable replacement logic here
    return true;
}

bool uShell::isValidVarname(char const *start, char const *end) const {
    // Implement variable name validation logic here
    return true;
}

std::string uShell::mergeTokens(TokenList const &tokenList, unsigned startPos) const {
    std::ostringstream oss;
    for (unsigned i = startPos; i < tokenList.size(); ++i) {
        if (i > startPos) {
            oss << ' ';
        }
        oss << tokenList[i];
    }
    return oss.str();
}

void uShell::echo(TokenList const &tokenList) {
    std::string output = mergeTokens(tokenList, 1);
    std::cout << output << std::endl;
}

void uShell::setVar(TokenList const &tokenList) {
    if (tokenList.size() >= 3) {
        std::string key = tokenList[1];
        std::string value = mergeTokens(tokenList, 2);
        m_vars[key] = value;
    }
}

void removeComments(std::string &input) 
{
    auto pos = input.find("#");
    if (pos != std::string::npos) {
        input = input.substr(0, pos);
    }
}

int uShell::run() {
    std::string input;
    while (true) {
        std::cout << m_prompt;
        if (!getInput(input)) break;

        removeComments(input);  // Call the member function
        printVerbose(input);

        TokenList tokenList;
        tokenize(input, tokenList);
        if (!tokenList.empty()) {
            const auto &cmd = tokenList[0];
            if (m_internalCmdList.find(cmd) != m_internalCmdList.end()) {
                auto func = m_internalCmdList[cmd];
                (this->*func)(tokenList);
            } else {
                // Handle external commands here
            }
        }
    }
    return m_exitCode;
}
