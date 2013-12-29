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
 * Checks if a file already exists on the filesystem.
 *
 * @param[in] The name of the file to check. Note: Include the full path in the
 *            file name.
 * @return    Returns TRUE (1) if file exists and user has read access to the
 *            file, otherwise function returns FALSE (0).
 */
int lrlib_file_exists(char* file_name) {
    int fp; // file pointer

    fp = fopen(file_name, "r+"); // open file in read mode. File must already exist.
    if (fp == NULL) {
        return FALSE;
    } else {
        fclose(fp);
        return TRUE;
    }
}

#define SEEK_END 9999
/**
 * Gets the size of a file in bytes
 *
 * Example code:
 *
 * @param[in] The name of the file to check. Note: Include the full path in the
 *            file name.
 * @return    Returns the size of the file in bytes.
 */
int lrlib_get_file_size(char* file_name) {
    // TODO: test for null input
    int fp; // filestream pointer
    int sz; // file size
    fp = fopen(file_name, "r+b"); // open in binary mode (read-only). File must exist.
    fseek(fp, 0, SEEK_END); // sets the position indicator associated with the stream to the end of the file.
    sz = ftell(fp); // standard C function to return the current value of the position indicator of the stream. For binary streams, this is the number of bytes from the beginning of the file. This function is undocumented in the VuGen help file.
    lr_output_message("size: %d", sz); // size in bytes
    return sz;
}

/**
 * Gets the size of a file in bytes
 *
 * Example code:
 *
 * @param[in] The name of the file to check. Note: Include the full path in the
 *            file name.
 * @return    Returns the size of the file in bytes.
 */
// Saves a file to the hard disk.
// Arguments:
// - file_name: Include the full path in the file name. Note that file must not exist before function is called.
// - file_content: The data to save to the file. Can be binary or string data.
// - file_size: The size/length of the data to save to the file. If it is string data, you can find this using strlen(). If you are saving binary data from a web page, use web_get_int_property(HTTP_INFO_DOWNLOAD_SIZE).
// Returns 0 on success. On failure, function will raise lr_error_message and return -1.
int lrlib_save_file(char* file_name, void* file_content, unsigned int file_size) {
    int rc; // function return code
    int fp; // file pointer

    // Check input values
    if (file_name == NULL) {
        lr_error_message("File name is NULL");
        return -1;
    } else if (file_content == NULL) {
        lr_error_message("File content is NULL");
        return -1;
    } else if (file_size < 1) {
        lr_error_message("Invalid file size: %d", file_size);
        return -1;
    }

    // Does the file already exist?
    if (lrlib_file_exists(file_name) == TRUE) {
        lr_error_message("File %s already exists", file_name);
        return -1;
    }

    fp = fopen(file_name, "wb"); // open file in "write, binary" mode.
    if (fp == NULL) {
        lr_error_message("Error opening file: %s", file_name);
        return -1;
    }

    rc = fwrite(file_content, file_size, 1, fp);
    if (rc != 1) {
        lr_error_message("Error writing to file. Items written: %d", rc);
        return -1;
    }

    rc = fclose(fp);
    if (rc != 0) {
        lr_error_message("Error closing file: %s", file_name);
        return -1;
    }

    return 0;
}

/**
 * Writes a string to the end of a file.
 *
 * TODO: write to file with locking
 * Note: Include the full path in the file name, and escape any slashes. E.g. "C:\\TEMP\\output.txt". Note that file does not have to exist beforehand, but directory does.
 * If attempting to write a single line, include a newline character at the end of the string.
 *
 * @param[in] The name of the file to check. Note: Include the full path in the
 *            file name.
 * @param[in] The string to append to the end of the file.
 * @return    Returns 0 on success. On failure, function will raise lr_error_message and return -1.
 */
int lrlib_append_to_file(char* file_name, char* string) {
    int fp; // file pointer
    int rc; // return code
    int length = strlen(string);

    // Check that file_name is not NULL.
    if (file_name == NULL) {
        lr_error_message("Error. File name is NULL");
        return -1;
    }

    fp = fopen(file_name, "a"); // open file in "append" mode.
    if (fp == NULL) {
        lr_error_message("Error opening file: %s", file_name);
        return -1;
    }

    rc = fprintf(fp, "%s", string);
    if (rc != length) {
       lr_error_message("Error writing to file: %s", file_name);
       return -1;
    }

    rc = fclose(fp);
        if (rc != 0) {
        lr_error_message("Error closing file: %s", file_name);
        return -1;
    }

    return 0;
}


#ifdef _WIN32
/**
 * Pause the execution of the vuser for the specified number of seconds.
 *
 * Note: This function only works on Windows
 * Note that this ignores the runtime settings related to think time.
 * TODO: should write to the output log when this is called (what message does lr_think_time() write?
 *
 * @param[in] The time (in seconds) to wait.
 * @return    This function does not return a value.
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

// TODO: add a "generate GUID" function (using the Windows GUIDFromString function)