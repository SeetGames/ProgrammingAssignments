#include <string>
#include <vector>
#include <map>
#include "uShell.h"

/************************************************************************/
/*!
\brief
uShell2 class. Acts as a command prompt that takes in input and performs
commands based on the input.
*/
/************************************************************************/
class uShell2 : public uShell
{
protected:
    typedef void (uShell2::*fInternalCmd2)(TokenList const &);
    /* ! Store the list of strings to function pointers of internal command
    in uShell2, i.e. for changeprompt and exit */
    std::map<std::string, fInternalCmd2> m_internalCmdList2;

    /************************************************************************/
    /*!
    \brief
    Reads the exit code from the token list
    \param tokenList
    Stores the exit code that will be extracted
    */
    /************************************************************************/
    void exit(TokenList const & tokenList);

    /************************************************************************/
    /*!
    \brief
    Changes the prompt that starts at every line
    \param tokenList
    The list of tokens where shell can retrieve the string value
    */
    /************************************************************************/
    void changePrompt(TokenList const & tokenList);

    /************************************************************************/
    /*!
    \brief
    Sets up the argument lists required for calling the execvp function, and
    creates the child process
    \param tokenList
    The list of tokens to get the data value from
    \param startParam
    The starting token to parse data from the list
    \param endParam
    1 pass the last token to parse data from the list
    */
    /************************************************************************/
    void execute(TokenList const & tokenList,
                 unsigned startParam, unsigned endParam);

    /************************************************************************/
    /*!
    \brief
    Calls an external command using the passed in parameters.
    \param tokenList
    The list of tokens to get the data value from
    */
    /************************************************************************/
    void doExternalCmd(TokenList const & tokenList);

public:
    /************************************************************************/
    /*!
    \brief
    Creates the class object of uShell2
    \param bFlag
    boolean value to decide whether to echo input
    */
    /************************************************************************/
    uShell2(bool bFlag);

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
