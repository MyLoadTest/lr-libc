/**
 * @file
 * @author  Stuart Moncrieff <stuart@myloadtest.com>
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * The lr-libc library is a collection of C functions for use in LoadRunner scripts.
 *
 * @section LICENSE
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef DWORD
#define DWORD unsigned long
#endif

#ifndef LPDWORD
#define LPDWORD DWORD far *
#endif

#ifndef PROCESS_TERMINATE
#define PROCESS_TERMINATE 0x0001
#endif

#ifndef PROCESS_VM_READ
#define PROCESS_VM_READ 0x0010
#endif

#ifndef PROCESS_QUERY_INFORMATION
#define PROCESS_QUERY_INFORMATION 0x0400
#endif

#ifndef PROCESS_QUERY_LIMITED_INFORMATION
#define PROCESS_QUERY_LIMITED_INFORMATION 0x1000
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifndef INFINITE
#define INFINITE 0xFFFFFFFF
#endif

#ifndef WAIT_ABANDONED
#define WAIT_ABANDONED 0x00000080L
#endif

#ifndef WAIT_OBJECT_0
#define WAIT_OBJECT_0 0x00000000L
#endif

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0L
#endif

#ifndef PDH_MORE_DATA
#define PDH_MORE_DATA 0x800007D2L
#endif

#ifndef PDH_NO_MORE_DATA
#define PDH_NO_MORE_DATA 0xC0000BCCL
#endif

#ifndef PERF_DETAIL_WIZARD
#define PERF_DETAIL_WIZARD 400
#endif

#ifndef PDH_STATUS
#define PDH_STATUS long
#endif

#ifndef PDH_HQUERY
#define PDH_HQUERY void*
#endif

#ifndef PDH_HCOUNTER
#define PDH_HCOUNTER void*
#endif

#define PDH_FMT_RAW          ((DWORD)0x00000010)
#define PDH_FMT_ANSI         ((DWORD)0x00000020)
#define PDH_FMT_UNICODE      ((DWORD)0x00000040)
#define PDH_FMT_LONG         ((DWORD)0x00000100)
#define PDH_FMT_DOUBLE       ((DWORD)0x00000200)
//#define PDH_FMT_LARGE        ((DWORD)0x00000400)
#define PDH_FMT_NOSCALE      ((DWORD)0x00001000)
#define PDH_FMT_1000         ((DWORD)0x00002000)
#define PDH_FMT_NODATA       ((DWORD)0x00004000)
#define PDH_FMT_NOCAP100     ((DWORD)0x00008000)

typedef struct _PDH_FMT_COUNTERVALUE {
    DWORD CStatus;
    long _padding;
    union {
        long longValue;
        double doubleValue;
        //long long largeValue;  // This gives 32 bits field in VuGen though it's supposed to be at least 64 bits
        unsigned char largeValue[8];
        const char* AnsiStringValue;
        const void* WideStringValue;
    } u;
} PDH_FMT_COUNTERVALUE, *PPDH_FMT_COUNTERVALUE;

/* LRLIB defines */

#define LRLIB_MAX_PARAM_NAME_LENGTH 200
#define LRLIB_MAX_SUFFIX_LENGTH 20
#define LRLIB_PARAM_NAME_BUFFER_LENGTH (LRLIB_MAX_PARAM_NAME_LENGTH + LRLIB_MAX_SUFFIX_LENGTH)

void lrlib_safe_free(void* ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}

void lrlib_safe_free_and_null(void** ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    
    if (*ptr != NULL)
    {
        free(*ptr);
        *ptr = NULL;
    }
}

void lrlib_load_dll(const char* dllPath)
{
    if (dllPath == NULL)
    {
        lr_error_message("DLL path cannot be NULL.");
        lr_abort();
    }

    {
        const int loadResult = lr_load_dll(dllPath);
        if (loadResult != 0)
        {
            lr_error_message("Error loading '%s' (error code %d).", dllPath, loadResult);
            lr_abort();
        }
    }
}

/**
 * @brief Creates a new UUID and saves its string representation to a parameter with the specified name.
 *
 * @param output_param_name The name of the parameter to save a created UUID to.
 * @return Returns TRUE (1) if the function succeeded; otherwise, returns FALSE (0).
 */
int lrlib_create_uuid(const char* output_param_name)
{
    const char* LIB_NAME = "Rpcrt4.dll";
    const int GUID_SIZE = 16;
    int rpcStatus;

    char* guid;
    unsigned char* str;

    const int loadResult = lr_load_dll(LIB_NAME);
    if (loadResult != 0)
    {
        lr_error_message("Error loading '%s' (error code %d).", LIB_NAME, loadResult);
        return FALSE;
    }

    guid = (char*)calloc(GUID_SIZE, 1);
    if (!guid)
    {
        lr_error_message("Error allocating memory.");
        return FALSE;
    }

    rpcStatus = UuidCreate(guid);
    if (rpcStatus != 0)
    {
        lr_error_message("Error creating UUID (%d).", rpcStatus);
        free(guid);
        return FALSE;
    }

    rpcStatus = UuidToStringA(guid, &str);
    if (rpcStatus != 0)
    {
        lr_error_message("Error converting UUID to string (%d).", rpcStatus);
        free(guid);
        return FALSE;
    }

    lr_save_string((char*)str, output_param_name);

    RpcStringFreeA(&str);
    free(guid);

    return TRUE;
}

/**
 * Pauses the execution of the vuser for the specified number of seconds. This
 * think time cannot be ignored by the script's runtime settings.
 *
 * Example code:
 *     // This is usually useful when you have a polling loop, and you don't
 *     // want to poll too quickly
 *
 * TODO: should write to the output log when this is called (what message does lr_think_time() write?
 *
 * @param[in] The time (in seconds) to wait.
 * @return    This function does not return a value.
 *
 * Note: This function only works on Windows.
 * Note: This function ignores the runtime settings related to think time.
 */
void lrlib_think_time(double time) {
    int rc;
    rc = lr_load_dll("Kernel32.dll"); // needed for sleep() <- use a static value, so lr_load_dll is only called once.
    if (rc != 0) {
        lr_error_message("Error loading DLL");
    }

    lr_start_transaction("sleep time");

    sleep(1000); // sleep time in milliseconds

    // Check whether VuGen regards sleep time as "wasted time" <- put this in function reference.
    lr_output_message("wasted time: %d", lr_get_transaction_wasted_time ("sleep time"));

    lr_end_transaction("sleep time", LR_AUTO);
}

/**
 * Gets the process ID of the mmdrv.exe process that is running the VuGen script that called
 * this function.
 *
 * @return    This function returns the process ID of the calling process.
 *
 * Example code:
 *     // Print the vuser's process ID
 *     int vuser_pid; vuser_pid = lrlib_get_vuser_pid();
 *     lr_output_message("vuser_pid: %d", vuser_pid);
 *
 * Note: This function only works on Windows.
 */
int lrlib_get_vuser_pid() {
    int rc; // return code
    int pid; // the process id
    static int dll_loaded = FALSE; // A static variable inside a function keeps its value between
                                   // invocations. The FALSE value is assigned only on the first
                                   // invocation.
    char* dll_name = "MSVCRT.DLL"; // This DLL contains the _getpid() function. It is a standard
                                   // Windows DLL, usually found in C:\WINDOWS\system32.
                                   // Note: on Windows platforms, if you do not specify a path,
                                   // lr_load_dll searches for the DLL using the standard sequence
                                   // used by the C++ function, LoadLibrary.

    // Only load the DLL the first time this function is called.
    if (dll_loaded == FALSE) {
        rc = lr_load_dll(dll_name);
        if (rc != 0) {
            lr_error_message("Error loading %s.", dll_name);
            lr_abort();
        }
        dll_loaded = TRUE;
    }

    pid = _getpid();

    return pid;
}

/**
 * Prints log options to the replay log.
 *
 * @param[in] The logging settings to print. This should be the value returned by
 *            lr_get_debug_message().
 * @return    This function does not return a value. The logging settings are printed to the
 *            replay log.
 *
 * Example code:
 *     // Print the current logging settings to the output log.
 *     unsigned int log_options;
 *     log_options = lr_get_debug_message();
 *     lrlib_print_log_options(log_options);
 *
 * Note: The standard lr_output_message function will write to the replay log even when logging
 * is disabled. The only case where it will not write to the replay log is when "send messages only
 * when an error occurs" is enabled.
 */
void lrlib_print_log_options(unsigned int log_options_to_print) {
    unsigned int current_log_settings; // the logging settings that were enabled when this
                                       // function was called.
    char bit_pattern[(sizeof(int) * 8) + 1]; // this string holds the pattern of bits from the
                                             // unsigned int containing the logging settings.

    // Save the logging settings that were enabled when this function was called.
    current_log_settings = lr_get_debug_message();

    // Get the bit pattern for the log options that were passed to this function.
    itoa(log_options_to_print, bit_pattern, 2); //

    // If "send messages only when an error occurs" is enabled, then turn it off, otherwise nothing
    // will be written to the output log.
    // Note use of the bitwise AND operator. "current_log_settings & LR_MSG_CLASS_JIT_LOG_ON_ERROR"
    // will evaluate to "LR_MSG_CLASS_JIT_LOG_ON_ERROR" if this setting is enabled.
    if (current_log_settings & LR_MSG_CLASS_JIT_LOG_ON_ERROR) {
        lr_set_debug_message(LR_MSG_CLASS_JIT_LOG_ON_ERROR, LR_SWITCH_OFF);
    }

    // Print the bit pattern.
    // Message formatting specifies a string that is 32 characters wide (padded with 0s to the
    // left of the printed value)
    lr_output_message("%032.32s", bit_pattern);
    lr_output_message("                      |    |||||");
    lr_output_message("                      |    ||||+-LR_MSG_CLASS_BRIEF_LOG (Standard log)");
    lr_output_message("                      |    |||+--LR_MSG_CLASS_RESULT_DATA (Data returned by server)");
    lr_output_message("                      |    ||+---LR_MSG_CLASS_PARAMETERS (Parameter substitution)");
    lr_output_message("                      |    |+----LR_MSG_CLASS_FULL_TRACE (Advanced trace)");
    lr_output_message("                      |    +-----LR_MSG_CLASS_EXTENDED_LOG (Extended log)");
    lr_output_message("                      +----------LR_MSG_CLASS_JIT_LOG_ON_ERROR (Send messages only when an error occurs)");

    // Restore the "send messages only when an error occurs" logging setting if it was
    // originally enabled.
    if (current_log_settings & LR_MSG_CLASS_JIT_LOG_ON_ERROR) {
        lr_set_debug_message(LR_MSG_CLASS_JIT_LOG_ON_ERROR, LR_SWITCH_ON);
    }

    return;
}

/**
 * Writes a message to the replay log, even if logging is disabled.
 *
 * @param[in] The message to write to the replay log.
 * @return    This function does not return a value.
 *
 * Example code:
 *     // Write the current {UserName} parameter to the replay log, even though "send messages
 *     // only when an error occurs" is enabled.
 *     lrlib_force_output_message(lr_eval_string("logged in user is {UserName}"));
 *
 * Note: The standard lr_output_message function will write to the replay log even when logging is
 * disabled. The only case where it will not write to the replay log is when "send messages only
 * when an error occurs" is enabled.
 * Note: The lr_output_message function supports sprintf-style message formatting. This function
 * only allows a single string argument. A good work-around for this is to include {Parameters}
 * in your message, and call the lr_eval_string function, as has been done in the example code.
 */
void lrlib_force_output_message(char* output_message) {
    unsigned int current_log_settings; // the logging settings that were enabled when this
                                       // function was called.

    // Check input variables
    if ( (output_message == NULL) || (strlen(output_message) == 0) ) {
        lr_error_message("output_message cannot be NULL or empty.");
        lr_abort();
    }

    // Save the logging settings that were enabled when this function was called.
    current_log_settings = lr_get_debug_message();

    // If "send messages only when an error occurs" is enabled, then turn it off, otherwise nothing
    // will be written to the output log.
    // Note use of the bitwise AND operator. "current_log_settings & LR_MSG_CLASS_JIT_LOG_ON_ERROR"
    // will evaluate to "LR_MSG_CLASS_JIT_LOG_ON_ERROR" if this setting is enabled.
    if (current_log_settings & LR_MSG_CLASS_JIT_LOG_ON_ERROR) {
        lr_set_debug_message(LR_MSG_CLASS_JIT_LOG_ON_ERROR, LR_SWITCH_OFF);
    }

    // Write the message to the replay log.
    lr_output_message(output_message);

    // Restore the "send messages only when an error occurs" logging setting if it was
    // originally enabled.
    if (current_log_settings & LR_MSG_CLASS_JIT_LOG_ON_ERROR) {
        lr_set_debug_message(LR_MSG_CLASS_JIT_LOG_ON_ERROR, LR_SWITCH_ON);
    }

    return;
}

/**
 * Sets new logging options to specify what information should be written to the replay log.
 *
 * @param[in] The new logging settings to use. Use the same C
 *       constants as lr_set_debug_message
 *       (LR_MSG_CLASS_BRIEF_LOG, LR_MSG_CLASS_EXTENDED_LOG
 *       etc.). Invalid combinations (that are not possible to
 *       create through the VuGen user interface) are not
 *       permitted.
 * @return    This function does not return a value.
 *
 * Example code:
 *     // Increase logging levels just for a short section of code (not the entire script).
 *     int original_options;
 *     // Save current logging options.
 *     original_options = lr_get_log lrlib_set_log_level(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA | LR_MSG_CLASS_FULL_TRACE);
 *
 *     // Put code that you want to get full logging for here (e.g. a step that is failing).
 *
 *     // Restore original logging options
 *     lrlib_set_log_level(original_options);
 *
 * Note: It is recommended that you use this function instead of the standard lr_set_debug_message
 * function. VuGen has an unexpected behaviour where if "send messages only when an error occurs"
 * is selected in the user interface (even if logging is disabled), then "send messages only when
 * an error occurs" will be enabled along with the new logging settings, even if it was not
 * specified in the function argument.
 */
void lrlib_set_log_level(unsigned int new_log_options) {
    int i;
    int is_valid = FALSE; // are the settings defined in new_log_options valid?
    int valid_log_settings[] = { // all the possible logging settings available through the GUI
        LR_MSG_CLASS_DISABLE_LOG,
        LR_MSG_CLASS_BRIEF_LOG,
        LR_MSG_CLASS_EXTENDED_LOG,
        LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS,
        LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_RESULT_DATA,
        LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA,
        LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_FULL_TRACE,
        LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_FULL_TRACE,
        LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA | LR_MSG_CLASS_FULL_TRACE,
        LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_BRIEF_LOG,
        LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG,
        LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS,
        LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_RESULT_DATA,
        LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA,
        LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_FULL_TRACE,
        LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_FULL_TRACE,
        LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA | LR_MSG_CLASS_FULL_TRACE
    };

    // Check wheter the new logging options are valid.
    for (i=0; i<17; i++) {
        if (new_log_options == valid_log_settings[i]) {
            is_valid = TRUE;
            break;
        }
    }

    if(is_valid == FALSE) {
        lr_error_message("Invalid logging setting. You may use one of the following:\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_DISABLE_LOG);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_BRIEF_LOG);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_EXTENDED_LOG);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_RESULT_DATA);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_FULL_TRACE);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_FULL_TRACE);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA | LR_MSG_CLASS_FULL_TRACE);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_BRIEF_LOG);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_RESULT_DATA);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_FULL_TRACE);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_FULL_TRACE);\n"
            "    lrlib_set_log_level(LR_MSG_CLASS_JIT_LOG_ON_ERROR | LR_MSG_CLASS_EXTENDED_LOG | LR_MSG_CLASS_PARAMETERS | LR_MSG_CLASS_RESULT_DATA | LR_MSG_CLASS_FULL_TRACE);"
        );
        lr_abort();
    }

    // Set the new logging options.
    lr_set_debug_message(LR_MSG_CLASS_DISABLE_LOG, LR_SWITCH_ON); // reset everything to 0
    lr_set_debug_message(new_log_options, LR_SWITCH_ON); // set the new option

    // If LR_MSG_CLASS_JIT_LOG_ON_ERROR has become set, and it was not specified in
    // new_log_options, then disable it.
    if ( (lr_get_debug_message() & LR_MSG_CLASS_JIT_LOG_ON_ERROR) &&
         !(new_log_options & LR_MSG_CLASS_JIT_LOG_ON_ERROR) ) {
        lr_set_debug_message(LR_MSG_CLASS_JIT_LOG_ON_ERROR, LR_SWITCH_OFF);
    }

    return;
}

int lrlib_get_process_file_path(const int processId, char* const filePath, const int maxLength)
{
    int result;

    lrlib_load_dll("kernel32.dll");
    lrlib_load_dll("psapi.dll");

    {
        const unsigned int hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (hProcess == NULL)
        {
            *filePath = '\0';
            return 0;
        }

        result = GetModuleFileNameExA(hProcess, NULL, filePath, maxLength);
        if (result == 0)
        {
            *filePath = '\0';
        }

        CloseHandle(hProcess);
    }

    return result;
}

int lrlib_kill_all_mmdrv()
{
    #define MAX_PROCESS_ID_COUNT 1024

    unsigned long currentProcessId;
    unsigned long processIds[MAX_PROCESS_ID_COUNT];
    const long ELEMENT_SIZE = sizeof(processIds[0]);
    unsigned long bytesReturned; 
    long enumResult;
    long processIdCount;
    long i;
    char processFilePath[MAX_PATH];
    char currentProcessFilePath[MAX_PATH];
    int res;
    int killCount = 0;

    lrlib_load_dll("kernel32.dll");
    lrlib_load_dll("psapi.dll");

    currentProcessId = GetCurrentProcessId();
    res = lrlib_get_process_file_path(currentProcessId, currentProcessFilePath, MAX_PATH);
    if (res <= 0)
    {
        lr_error_message("Error querying the current process.");
        return 0;
    }

    enumResult = EnumProcesses(processIds, MAX_PROCESS_ID_COUNT * ELEMENT_SIZE, &bytesReturned);
    if (!enumResult)
    {
        lr_error_message("Error enumerating processes.");
        return 0;
    }

    processIdCount = bytesReturned / ELEMENT_SIZE;
    for (i = 0; i < processIdCount; i++)
    {
        const unsigned long processId = processIds[i];
        if (processId == currentProcessId)
        {
            continue;
        }

        res = lrlib_get_process_file_path(processId, processFilePath, MAX_PATH);
        if (res <= 0)
        {
            continue;
        }

        if (stricmp(processFilePath, currentProcessFilePath) != 0)
        {
            continue;
        }

        {
            const unsigned long hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_TERMINATE, FALSE, processId);
            if (hProcess == NULL)
            {
                continue;
            }

            lr_output_message("Killing process %d", processId);
            if (TerminateProcess(hProcess, 0))
            {
                killCount++;
            }

            CloseHandle(hProcess);
        }
    }

    return killCount;
}

int lrlib_get_perfmon_counter_list(const char* outputParamArr)
{
    if (outputParamArr == NULL)
    {
        lr_error_message("Output parameter name cannot be NULL.");
        return -1;
    }

    if (strlen(outputParamArr) > LRLIB_MAX_PARAM_NAME_LENGTH)
    {
        lr_error_message("Output parameter name is too long.");
        return -1;
    }
    
    lrlib_load_dll("pdh.dll");
    
    {
        unsigned long size = 0;
        const PDH_STATUS initialStatus = PdhEnumObjectsA(NULL, NULL, NULL, &size, PERF_DETAIL_WIZARD, TRUE);
        if (initialStatus != PDH_MORE_DATA)
        {
            lr_error_message("Unexpected PDH code %d.", initialStatus);
            return -1;
        }
    
        size++;  // A reserve may be needed on some systems
    
        {
            unsigned int count;
            
            char* const buffer = (char*)malloc(size * sizeof(char));
            if (buffer == NULL)
            {
                lr_error_message("Error allocating memory (%u bytes).", size);
                return -1;
            }
    
            {
                const char* current;
                char parameterName[LRLIB_PARAM_NAME_BUFFER_LENGTH];

                const PDH_STATUS status = PdhEnumObjectsA(NULL, NULL, buffer, &size, PERF_DETAIL_WIZARD, FALSE);
                if (status != ERROR_SUCCESS)
                {
                    free(buffer);
                    lr_error_message("Error calling PDH function (%d).", status);
                    return -1;
                }
            
                current = buffer;
                count = 0;
                while (*current != '\0')
                {
                    const unsigned int length = strlen(current);
                    count++;
                    
                    sprintf(parameterName, "%s_%u", outputParamArr, count);
                    //lr_output_message("%s", current);
                    lr_save_string(current, parameterName);

                    current += length + 1;
                }
                
                sprintf(parameterName, "%s_count", outputParamArr);
                lr_save_int(count, parameterName);
            }
        
            free(buffer);
            return count;
        }
    }
}

int lrlib_get_perfmon_counter_item_list(const char* objectName, const char* itemOutputParamArr, const char* instanceOutputParamArr)
{
    if (itemOutputParamArr == NULL || instanceOutputParamArr == NULL)
    {
        lr_error_message("Output parameter name cannot be NULL.");
        return FALSE;
    }

    if (strlen(itemOutputParamArr) > LRLIB_MAX_PARAM_NAME_LENGTH || strlen(instanceOutputParamArr) > LRLIB_MAX_PARAM_NAME_LENGTH)
    {
        lr_error_message("Output parameter name is too long.");
        return FALSE;
    }
    
    lrlib_load_dll("pdh.dll");
    
    {
        unsigned long counterListLength = 0;
        unsigned long instanceListLength = 0;
    
        const PDH_STATUS initialStatus = PdhEnumObjectItemsA(
            NULL,
            NULL,
            objectName,
            NULL,
            &counterListLength,
            NULL,
            &instanceListLength,
            PERF_DETAIL_WIZARD,
            0);
        if (initialStatus != PDH_MORE_DATA)
        {
            lr_error_message("Unexpected PDH code %d.", initialStatus);
            return FALSE;
        }

        {
            char* counterList = (char*)malloc(counterListLength * sizeof(char));
            char* instanceList = (char*)malloc(instanceListLength * sizeof(char));
        
            if (counterList == NULL || instanceList == NULL)
            {
                lrlib_safe_free_and_null((void**)&counterList);
                lrlib_safe_free_and_null((void**)&instanceList);
        
                lr_error_message("Error allocating memory.");
                return FALSE;
            }
        
            {
                const PDH_STATUS status = PdhEnumObjectItemsA(
                    NULL,
                    NULL,
                    objectName,
                    counterList,
                    &counterListLength,
                    instanceList,
                    &instanceListLength,
                    PERF_DETAIL_WIZARD,
                    0);
            
                if (status != ERROR_SUCCESS)
                {
                    lrlib_safe_free_and_null((void**)&counterList);
                    lrlib_safe_free_and_null((void**)&instanceList);
            
                    lr_error_message("PDH error: %d.", status);
                    return FALSE;
                }
            
                if (counterListLength > 0)
                {
                    char parameterName[LRLIB_PARAM_NAME_BUFFER_LENGTH];
                    int count = 0;
                    const char* current;
                    for (current = counterList; *current != '\0'; current += strlen(current) + 1)
                    {
                        count++;
            
                        sprintf(parameterName, "%s_%u", itemOutputParamArr, count);
                        //lr_output_message("%s", current);
                        lr_save_string(current, parameterName);
                    }
                    
                    sprintf(parameterName, "%s_count", itemOutputParamArr);
                    lr_save_int(count, parameterName);
                }
            
                if (instanceListLength > 0)
                {
                    char parameterName[LRLIB_PARAM_NAME_BUFFER_LENGTH];
                    const char* current = instanceList;
                    int count = 0;
                    while (*current != '\0')
                    {
                        const unsigned int length = strlen(current);
                        count++;
            
                        sprintf(parameterName, "%s_%u", instanceOutputParamArr, count);
                        //lr_output_message("%s", current);
                        lr_save_string(current, parameterName);
                        
                        current += length + 1;
                    }
                    
                    sprintf(parameterName, "%s_count", instanceOutputParamArr);
                    lr_save_int(count, parameterName);
                }
            
                lrlib_safe_free_and_null((void**)&counterList);
                lrlib_safe_free_and_null((void**)&instanceList);
            }
        }
    }
    
    return TRUE;
}

int lrlib_get_perfmon_counter_value(
    const char* fullCounterPath,
    const DWORD maxSampleCount,
    const DWORD intervalInMsec,
    const DWORD counterFormat,
    const char* outputParamArr)
{
    if (fullCounterPath == NULL)
    {
        lr_error_message("Full counter path cannot be NULL.");
        return FALSE;
    }
    
    if (outputParamArr == NULL)
    {
        lr_error_message("Output parameter name cannot be NULL.");
        return FALSE;
    }
    
    if (strlen(outputParamArr) > LRLIB_MAX_PARAM_NAME_LENGTH)
    {
        lr_error_message("Output parameter name is too long.");
        return FALSE;
    }
    
    if (sizeof(PDH_FMT_COUNTERVALUE) != 16)
    {
        lr_error_message("INTERNAL ERROR: Windows API structure size mismatch.");
        return FALSE;
    }

    {
        int result = TRUE;
        PDH_HQUERY queryHandle = 0;
        PDH_HCOUNTER counterHandle = 0;
    
        lrlib_load_dll("kernel32.dll");
        lrlib_load_dll("pdh.dll");
    
        {
            PDH_STATUS openQueryStatus;
            PDH_STATUS addCounterStatus;
            PDH_STATUS initialCollectStatus;
            unsigned long index;
            int count = 0;
            char parameterName[LRLIB_PARAM_NAME_BUFFER_LENGTH];
            
            openQueryStatus = PdhOpenQueryA(NULL, 0, &queryHandle);
            if (openQueryStatus != ERROR_SUCCESS)
            {
                lr_error_message("Cannot open PDH query (error 0x%08X).", openQueryStatus);
                result = FALSE;
                goto CleanUp;
            }
        
            addCounterStatus = PdhAddCounterA(queryHandle, fullCounterPath, 0, &counterHandle);
            if (addCounterStatus != ERROR_SUCCESS)
            {
                lr_error_message("Cannot add PDH counter (error 0x%08X).", addCounterStatus);
                result = FALSE;
                goto CleanUp;
            }
        
            initialCollectStatus = PdhCollectQueryData(queryHandle);
            if (initialCollectStatus != ERROR_SUCCESS && initialCollectStatus != PDH_NO_MORE_DATA)
            {
                lr_error_message("Error collecting data (error 0x%08X).", initialCollectStatus);
                result = FALSE;
                goto CleanUp;
            }
        
            for (index = 0; index < maxSampleCount; index++)
            {
                Sleep(intervalInMsec);
        
                {
                    PDH_FMT_COUNTERVALUE itemBuffer = { 0 };
                    PDH_STATUS getValueStatus;
                    char current[64];
                    
                    const PDH_STATUS collectStatus = PdhCollectQueryData(queryHandle);
                    if (collectStatus == PDH_NO_MORE_DATA)
                    {
                        break;
                    }
            
                    if (collectStatus != ERROR_SUCCESS)
                    {
                        lr_error_message("Error collecting data (error 0x%08X).", collectStatus);
                        result = FALSE;
                        goto CleanUp;
                    }
            
                    getValueStatus = PdhGetFormattedCounterValue(
                        counterHandle,
                        counterFormat,
                        (LPDWORD)NULL,
                        &itemBuffer);
                    if (getValueStatus != ERROR_SUCCESS)
                    {
                        lr_error_message("Error formatting counter value (error 0x%08X).", getValueStatus);
                        result = FALSE;
                        goto CleanUp;
                    }
                
                    count++;
                    sprintf(parameterName, "%s_%u", outputParamArr, count);
                
                    if ((counterFormat & PDH_FMT_DOUBLE) == PDH_FMT_DOUBLE)
                    {
                        sprintf(current, "%.20g", itemBuffer.u.doubleValue);
                    }
                    else if ((counterFormat & PDH_FMT_LONG) == PDH_FMT_LONG)
                    {
                        sprintf(current, "%ld", itemBuffer.u.longValue);
                    }
                    //else if ((counterFormat & PDH_FMT_LARGE) == PDH_FMT_LARGE)
                    //{
                    //    sprintf(current, "%lld", itemBuffer.u.largeValue);
                    //}
                    else
                    {
                        lr_error_message("Unexpected counter format (0x%08X).", counterFormat);
                        result = FALSE;
                        goto CleanUp;
                    }
                    
                    lr_save_string(current, parameterName);
                }
            }
            
            sprintf(parameterName, "%s_count", outputParamArr);
            lr_save_int(count, parameterName);
        }
    
    CleanUp:
        if (queryHandle)
        {
            PdhCloseQuery(queryHandle);
        }
    
        return result;
    }
}


// TODO list of functions
// ======================
// * popen wrapper function
// * check PDF function
// * SHA256 function
// * check if a port is open
// * calendar/date functions
// * lrlib_kill_all_mmdrv
//   see: http://msdn.microsoft.com/en-us/library/windows/desktop/ms684847(v=vs.85).aspx (EnumProcesses, GetProcessId, TerminateProcess)
// * Add debug trace logging to functions with lr_debug_message(LR_MSG_CLASS_FULL_TRACE, "message");
