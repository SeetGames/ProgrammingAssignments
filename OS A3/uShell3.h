#include "uShell2.h"

/*!*****************************************************************************
 * @brief uShe113 class. Acts as a command prompt that takes in input and performs
          commands based on the input.
*******************************************************************************/
class uShell3 : public uShell2
{
protected:
    typedef void (uShell3::*fInternalCmd3)(const TokenList&);

    /*! Store the list of strings to function pointers of internal command
        in uShe113, i.e. finish.*/
    std::map<std::string, fInternalCmd3> m_internalCmdList3;

    /*!*****************************************************************************
     * @brief Process info for background processes.
     * 
    *******************************************************************************/
    struct ProcessInfo
    {
        /*! The process ID given by OS */
        int PID;

        /*! The state of the process, whether it is active */
        bool bActive;

        /*!*****************************************************************************
         * @brief Default constructor
         * It sets PID to 0 and bActive to false.
        *******************************************************************************/
        ProcessInfo();
        /*!*****************************************************************************
         * @brief Value constructor, set process id and current state.
         * It sets PID to id and bActive to state.
         * @param _id The process ID given by the OS
         * @param _state The state of the process to set
        *******************************************************************************/
        ProcessInfo(int _id, bool _state);
    };

    /*!*****************************************************************************
     * @brief Process info for piping commands.
    *******************************************************************************/
    struct PipeInfo
    {   
        /*! File descriptor array. Thc first is READ/IN descriptor, the
        second WRTIE/Otn descriptor */
        int descriptor[2];
        /*! The position of the pipe token "|" within the token list*/
        unsigned posInToken;
        /*! Const value for RFAD/IN descriptor */
        static const int IN_DESCRIPTOR = 0;
        /*! Const value for descriptor */
        static const int OUT_DESCRIPTOR = 1;
    };
    /*! Store the list of background processes */
    std::vector<ProcessInfo> m_bgProcessList;

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
    bool exist(const TokenList& _tokenList, unsigned _start, unsigned _end);

    /*!*****************************************************************************
     * @brief Calls an external command using the passed in parameters.
     * 
     * The function checks for rare case where first token is a pipe, where nothing will be done.
     * It flushes the buffer and then count the number of pipes in the token list. It adds pipe
     * information to a list typed of std :: vector < P ipeInf o > and creates the pipe (using
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
    virtual void doExternalCmd(const TokenList& _tokenList);

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
    void finish(const TokenList& _tokenList);

public:
   /*!*****************************************************************************
    * @brief Creates the class object of uShe113
    * It sets function pointers for finish in m internalCmdList3.
    * @param _bFlag boolean value to decide whether to echo input
   *******************************************************************************/
    uShell3(bool _bFlag);

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
    int run();
};

























