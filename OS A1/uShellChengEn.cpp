/* Start Header
*****************************************************************/
/*!
\file   uShell.cpp
\author Lau Cheng En, chengen.lau, 2200938
\par    chengen.lau@digipen.edu
\date   02/09/2023
\brief  This file contains the definitions of functions declared
        in uShell.h, which implements a shell interface "uShell".
        As of the current version, it supports setting of
        environment variables, ignoring of comments and running
        a few commands.
        
        The commands include:
        - echo
            Prints out the rest of the arguments
            "${<var_name}" can be used to refer to a set variable
        - setvar <var_name> <value>
            Sets a variable to a specific value
            Variables must begin with an alphabet and can contain
            alpha-numerical values only
        - exit <num>
            Quits the program with the exit code given
            (0 by default)

Copyright (C) 2023 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* End Header
*******************************************************************/
#include "uShell.h"
#include <iostream>
#include <algorithm>

bool uShell::getInput(std::string& input) const {
  if (!std::getline(std::cin, input)) { return false; }
  
  // if last char is '\r', remove it
  if (input.back() == '\r') { input.pop_back(); }
  return true;
}

void uShell::printVerbose(std::string const& input) {
  size_t start{ input.find_first_not_of(' ') }, end{ input.find_last_not_of(' ') };

  std::cout << input.substr(start, end - start + 1) << "\n";
}

void uShell::tokenize(std::string const& input, TokenList& tokenList) const {
  for (size_t i{input.find_first_not_of(' ')}, end{}; i != std::string::npos; i = input.find_first_not_of(' ', end)) {
    end = input.find_first_of(' ', i);
    
    tokenList.emplace_back(input.substr(i, end - i));
    if (end == std::string::npos) {
      break;
    }  // break if end of line
  }

  #ifdef DEBUG
  std::cout << "tokenizing \"" << input << "\"\nFound:\n";
  for (std::string const& str : tokenList) {
    std::cout << str << (str == tokenList.back() ? "\n\n" : " | ");
  }
  #endif
}

bool uShell::replaceVars(TokenList& tokenList) const {
  #ifdef DEBUG
  std::cout << "replaceVars...\nCurrent TokenList:\n";
  for (std::string const& str : tokenList) {
    std::cout << str << (str == tokenList.back() ? "\n" : " | ");
  }
  #endif
  
  size_t i{1};
  for (std::string& str : tokenList) {
    if (str == "#") {
      #ifdef DEBUG
      std::cout << "'#' detected at pos " << i - 1 << ", resizing tokenList...\n";
      #endif

      tokenList.resize(i - 1);
      break;
    }
    /* Leaving this here in case comments
       system is to be improved on in future  */
    // else {
    //   size_t pos{str.find_first_of(COMMENT_SYMBOL)};
    //   if (pos != std::string::npos) {
    //     str.resize(pos);
    //     tokenList.resize(i);
    //     break;
    //   }
    // }
      
    size_t pos{};
    while ((pos = str.find_first_of('$', pos)) != std::string::npos) {
      // ignore if not '{'
      if (str[pos + 1] != '{') {
        ++pos;
        continue;
      }
      
      #ifdef DEBUG
      std::cout << "Found \"${\" at pos " << pos << "\n";
      #endif

      size_t end{ str.find_first_of('}', pos + 1) };
      if (end == std::string::npos) { break; }
      
      // check sub-string for more occurences of "${}"
      std::string var_str{ str.substr(pos + 2, end - pos - 2) };

      #ifdef DEBUG
      std::cout << "Checking sub-str \"" << var_str << "\" for more variables...\n";
      #endif

      for (size_t sub_pos{pos}; (sub_pos = str.find_first_of('$', sub_pos + 1)) != std::string::npos && sub_pos < end;) {
        if (str[sub_pos + 1] != '{') { continue; }  // if '{' not directly after '$', continue

        #ifdef DEBUG
        std::cout << "Found \"${\" at pos " << sub_pos << "\n";
        #endif

        // create a new substr with the current "${"
        end = str.find_first_of('}', sub_pos + 2);
        var_str = str.substr(sub_pos + 2, end - sub_pos - 2);
        pos = sub_pos;
      }
      const char* var{ var_str.c_str() };

      #ifdef DEBUG
      std::cout << "Validating " << var << "...\n";
      #endif

      std::map<std::string, std::string>::const_iterator iter{ m_vars.find(var) };
      if (iter == m_vars.end()) {
        std::cerr << "Error: " << var << " is not a defined variable.\n";
        return false; 
      }
      // if valid variable name, replace accordingly in map
      if (isalpha(*var) && isValidVarname(var + 1, var + var_str.size())) {
        str.replace(pos, var_str.size() + 3, iter->second);
      }

      #ifdef DEBUG
      std::cout << "Found \"}\" at pos " << end << "\n";
      std::cout << "replacing " << var << " with " << iter->second << "\n\n";
      #endif
      
      break;
    }
    ++i;
  }

  return true;
}

bool uShell::isValidVarname(char const* start, char const* end) const {
  for (unsigned i{}; i < end-start; ++i) {
    if (!isalnum(*(start + i))) { 
      #ifdef DEBUG
      std::cout << "Invalid variable\n";
      #endif
      return false;
    }
  }

  #ifdef DEBUG
  std::cout << "Valid variable\n";
  #endif

  return true;
}

std::string uShell::mergeTokens(TokenList const& tokenList, unsigned startPos) const {
  #ifdef DEBUG
  std::cout << "mergeTokens: merged: ";
  for (std::string const& str : tokenList) {
    std::cout << str << (str == tokenList.back() ? "\n" : " | ");
  }
  #endif

  std::string merged_str{ tokenList[startPos] };
  for (std::vector<std::string>::const_iterator i{tokenList.cbegin() + startPos + 1};
       i != tokenList.cend(); ++i)
  {
    merged_str += " " + *i;
  }

  #ifdef DEBUG
  std::cout << "into " << merged_str << "\n\n";
  #endif

  return merged_str;
}

void uShell::echo(TokenList const& tokenList) {
  for (std::vector<std::string>::const_iterator 
       i{tokenList.cbegin() + 1}; i != tokenList.cend(); ++i)
  {
    std::cout << *i << (i == tokenList.cend() - 1 ? "\n" : " ");
  }
}

void uShell::setVar(TokenList const& tokenList) {
  #ifdef DEBUG
  std::cout << "setVar:\nCurrent TokenList:\n";
  for (std::string const& str : tokenList) {
    std::cout << str << (str == tokenList.back() ? "\n" : " | ");
  }
  #endif

  if (tokenList.empty()) { return; }

  
  std::string const var{ tokenList[1] };
  const char* c_str = var.c_str();

  #ifdef DEBUG
  std::cout << "Validating " << var << "...\n";
  #endif

  if (!isValidVarname(c_str, c_str + var.size())) { return; }
  
  

  if (tokenList.size() >= 3) { // combine strings
    m_vars[var] = mergeTokens(tokenList, 2);
  }
  else {  // set to empty string
    m_vars[var] = std::string(); 
  }

  #ifdef DEBUG
  std::cout << "Mapped " << var << " to \"" << m_vars[var] << "\"\n\n";
  #endif
}

uShell::uShell(bool bFlag) {
  m_prompt = "uShell";
  m_verbose = bFlag;
  m_exit = false;
  m_exitCode = 0;
  m_internalCmdList.emplace(std::string("echo"), &uShell::echo);
  m_internalCmdList.emplace(std::string("setvar"), &uShell::setVar);
}

int uShell::run() {
  TokenList tokenList{};
  while (!m_exit) {
    tokenList.clear();  // clear the token list every iteration
    std::cout << m_prompt << ">";

    std::string input{};
    // exit next iteration if eof reached or "exit" command given
    if (!getInput(input) || input == "exit") { break; }
    // continue if input empty
    if (input.find_first_not_of(' ') == std::string::npos) { continue; }

    tokenize(input, tokenList);
    if (m_verbose) {
      printVerbose(input);
    }
    // continue if token list is empty or invalid variable given
    if (!replaceVars(tokenList) || tokenList.empty()) { continue; }

    #ifdef DEBUG
    std::cout << "Output: ";
    #endif

    // run command based on the first token in the list
    std::map<std::string, fInternalCmd>::const_iterator iter{ m_internalCmdList.find(tokenList.front()) };
    if (iter != m_internalCmdList.end()) {
      (this->*(iter->second))(tokenList);
    }

    #ifdef DEBUG
    std::cout << "\n-------------------------------\n\n";
    #endif
  }

  return m_exitCode;
}