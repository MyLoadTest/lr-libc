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

#ifdef _WIN32
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
#endif

// TODO list of functions
// ======================
// * functions for VuGen logging levels
// * popen wrapper function
// * check PDF function
// * SHA256 function
// * check if a port is open
// * calendar/date functions
