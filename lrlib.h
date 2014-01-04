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
 * Prints log options to the output log.
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
 * Note: The lr_output_message() function will write to the replay log even when logging is
 * disabled. The only case where it will not write to the replay log is when "send messages only
 * when an error occurs" is enabled
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


// TODO list of functions
// ======================
// * popen wrapper function
// * check PDF function
// * SHA256 function
// * check if a port is open
// * calendar/date functions
// * lrlibc_kill_all_mmdrv
//   see: http://msdn.microsoft.com/en-us/library/windows/desktop/ms684847(v=vs.85).aspx (EnumProcesses, GetProcessId, TerminateProcess)
// * Add debug trace logging to functions with lr_debug_message(LR_MSG_CLASS_FULL_TRACE, "message");
