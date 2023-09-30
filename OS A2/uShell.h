#include <string>
#include <vector>
#include <map>

/*! list of strings */
using TokenList = std::vector<std::string>;

/************************************************************************/
/*!
 \brief
 uShell class. Acts as a command prompt that takes in input and performs
 commands based on the input
*/
/************************************************************************/
class uShell
{
protected:
    //Shell function pointers
    typedef void (uShell::*fInternalCmd)(TokenList const &);
    //Store the list of strings to function pointers of internal commands
    std::map<std::string, fInternalCmd> m_internalCmdList;
    //Map of variables to string values
    std::map<std::string, std::string> m_vars;
    //String prompt that is displayed at the beginning
    std::string m_prompt;
    //decide whether to echo user input
    bool m_verbose;
    //determine whether to exit
    bool m_exit;
    //exit code determined during runtime
    int m_exitCode;
    /************************************************************************/
    /*!
     \brief
    Get one line of input from std::cin. Removes the trailing \r if any
    \param input
    The input string to take the user input by reference
    \return
    return false if encountering the EOF
    */
    /************************************************************************/
    bool getInput(std::string &input) const;

    /************************************************************************/
    /*!
    \brief
    Print the user input without printing the starting and the trailing spaces.
    \param input
    The input string
    */
    /************************************************************************/
    void printVerbose(std::string const &input);

    /************************************************************************/
    /*!
    \brief
    Tokenize the input string into a list of strings which are separated by spaces.
    \param input
    The input string given by the user
    \param tokenList
    Token list to fill up, passed by reference to add strings to it
    */
    /************************************************************************/
    void tokenize(std::string const &input, TokenList &tokenList) const;

    /************************************************************************/
    /*!
    \brief
    Search and replace from the map all variables in the token list with the value
    \param tokenList
    Token list to replace the variables with
    \return
    Boolean value to decide whether there is an invalid variable found in the string.
    Returning true means all valid. Otherwise,
    Returning false means there is an invalid variable (abort the search).
    */
    /************************************************************************/
    bool replaceVars(TokenList &tokenList) const;
    /************************************************************************/
    /*!
    \brief
    Check whether each character is a number or digit from the given start and
    end of a character array.
    \param start
    Pointer to the start of string to search from
    \param end
    Pointer to the end of string where search stops
    \return
    Boolean value to represent if the string is valid
    Returning true means each character in the given string is valid.
    Otherwise, it returns false.
    */
    /************************************************************************/
    bool isValidVarname(char const *start, char const *end) const;

    /************************************************************************/
    /*!
    \brief
    Merge all the tokens from a given token list, separated by a space, into one
    single string. We expect return value optimization and move semantics help.
    \param tokenList
    The token list to take from
    \param startPos
    The starting position / index to start merging from
    \return
    The merged string
    */
    /************************************************************************/
    std::string mergeTokens(TokenList const &tokenList, unsigned startPos) const;

    /************************************************************************/
    /*!
    \brief
    Echo the user input
    \param tokenList
    The list of data to read in
    */
    /************************************************************************/
    void echo(TokenList const &tokenList);

    /************************************************************************/
    /*!
    \brief
    Set the value of a variable
    fix the spacing issue
    */
    /************************************************************************/
    void setVar(TokenList const &tokenList);

public:
    /************************************************************************/
    /*!
    \brief
    Creates the class object of uShell
    \param bFlag
    Boolean value to decide whether to echo input
    */
    /************************************************************************/
    uShell(bool bFlag);

    /************************************************************************/
    /*!
    \brief
    Public function for external call. Execute in loops and waits for input.
    \return
    Exit code, of the exit command
    */
    /************************************************************************/
    int run();
};