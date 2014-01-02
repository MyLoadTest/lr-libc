/**
 * Checks if a file already exists on the filesystem.
 *
 * @param[in] The name of the file to check. Note: Include the full path in the
 *            file name.
 * @return    Returns TRUE (1) if file exists and user has read access to the
 *            file, otherwise function returns FALSE (0).
 */
int lrlib_file_exists(char* file_name) {
    int fp; // filestream pointer

    // Check input variables
    if ( (file_name == NULL) || (strlen(file_name) == 0) ) {
        lr_error_message("File name cannot be NULL or empty.");
        lr_abort();
    }

    // Open file in read mode. File must already exist.
    fp = fopen(file_name, "r+");
    if (fp == NULL) {
        return FALSE;
    } else {
        fclose(fp);
        return TRUE;
    }
}

/**
 * Gets the size of a file in bytes.
 *
 * Example code:
 *     // Get the size of the C:\TEMP\example.txt file.
 *     int size;
 *     size = lrlib_get_file_size("C:\\TEMP\\example.txt");
 *     lr_output_message("The size of the file is: %d", size);
 *
 * Test cases:
 *    * call with empty file name
 *    * call with null file name
 *    * call with file that does not exist
 *    * call with file of known size
 *
 * @param[in] The name of the file to check. Note: Include the full path in the
 *            file name.
 * @return    Returns the size of the file in bytes.
 */
int lrlib_get_file_size(char* file_name) {
    int fp; // filestream pointer
    int size; // file size

    // Check input variables
    if ( (file_name == NULL) || (strlen(file_name) == 0) ) {
        lr_error_message("File name cannot be NULL or empty.");
        lr_abort();
    }

    // Open the file in binary mode (read-only). File must exist.
    fp = fopen(file_name, "r+b");
    if (fp == NULL) {
        lr_error_message("File must exist to get file size.");
        lr_abort();
    }

    // Set the position indicator associated with the stream to the end of the
    // file. The end of the file is indicated by 2 (SEEK_END), with an offset
    // of 0 bytes.
    fseek(fp, 0, 2);

    // ftell is a standard C function that returns the current value of the
    // position indicator of the stream. For binary streams, this is the number
    // of bytes from the beginning of the file. This function is undocumented
    // in the VuGen 11.04 help file.
    size = ftell(fp);

    // Close the filestream
    fclose(fp);

    // Return the size of the file (in bytes)
    return size;
}

/**
 * Saves a file to the hard disk.
 *
 * Example code:
 *     // Downloading a PDF from a website, saving it to a parameter, then saving the parameter contents to a file.
 *     int pdf_size;
 *
 *     // Save entire HTTP response body (LB and RB are empty)
 *     web_reg_save_param_ex(
 *         "ParamName=PdfContents",
 *         "LB/BIN=",
 *         "RB/BIN=",
 *         SEARCH_FILTERS,
 *         "Scope=Body",
 *         LAST);
 *
 *     // Note that the PDF download must be in a step by itself in order to save
 *     // the contents. It cannot be an EXTRARES.
 *     web_url("Download PDF",
 *         "URL=http://www.myloadtest.com/resources/hp-discover-2012-tb2337-moncrieff.pdf",
 *         "Resource=0",
 *         LAST);
 *
 *     pdf_size = web_get_int_property(HTTP_INFO_DOWNLOAD_SIZE);
 *
 *     // And now here is the function that the example is supposed to be for.
 *     lrlib_save_file("C:\\TEMP\\test.pdf", lr_eval_string("{PdfContents}"), pdf_size);
 *
 * Test Cases:
 *     * file_size smaller than file_content
 *     * file_size greater than file_content
 *
 * @param[in] The name of the file to write to. Note: Include the full path in
 *            the file name. Note that if a file with the same name already
 *            exists, it will be silently overwritten.
 * @param[in] The data to save to the file. Can be binary or string data.
 * @param[in] The size/length of the data to save to the file. If it is string
 *            data, you can find this using strlen(). If you are saving binary
 *            data from a web page, use
 *            web_get_int_property(HTTP_INFO_DOWNLOAD_SIZE). Note that if the
 *            binary file is 1000 bytes, and you pass in 100, then only the
 *            first 100 bytes will be written to the file.
 * @return    Returns the number of bytes successfully written to the file,
 *            otherwise an error is raised and the script is aborted.
 *
 * Note: to delete a file or directory, see remove() and rmdir()
 */
int lrlib_save_file(char* file_name, void* file_content, unsigned int file_size) {
    int rc; // function return code
    int fp; // filestream pointer
    int bytes; // number of bytes written to the file

    // Check input variables
    if ( (file_name == NULL) || (strlen(file_name) == 0) ) {
        lr_error_message("File name cannot be NULL or empty.");
        lr_abort();
    } else if (file_content == NULL) {
        lr_error_message("File content cannot be NULL");
        lr_abort();
    } else if (file_size < 1) {
        lr_error_message("Invalid file size: %d", file_size);
        lr_abort();
    }


    // Open file in "write, binary" mode. File does not have to already exist.
    fp = fopen(file_name, "wb");
    if (fp == NULL) {
        lr_error_message("Error opening file: %s", file_name);
        lr_abort();
    }

    // Write to the file (file_size * 1 bytes).
    // Note that increasing the block size (above 1 byte) does not make writing
    // the file contents more efficient. The bytes are still written one at a
    // time to the stream, as if fputc was called for each byte.
    bytes = fwrite(file_content, 1, file_size, fp);
    if (bytes != file_size) {
        lr_error_message("Error writing to file. Bytes written: %d of %d", bytes, file_size);
        lr_abort();
    }

    // Close the filestream
    fclose(fp);

    // Return the number of blocks written by fwrite
    return bytes;
}

/**
 * Writes a string to the end of a file.
 *
 * Example code:
 *     // TODO
 *
 * @param[in] The name of the file to check. Note: Include the full path in the
 *            file name. and escape any slashes. E.g. "C:\\TEMP\\output.txt".
 *            Note that file does not have to exist beforehand, but the
 *            directory does.
 * @param[in] The string to append to the end of the file. Note: If attempting
 *            to write a single line, include a newline character at the end
 *            of the string.
 * @return    Returns the number of bytes successfully written to the file,
 *            otherwise an error is raised and the script is aborted.
 */
int lrlib_append_to_file(char* file_name, char* string) {
    int rc; // return code
    int fp; // filestream pointer
    int length = strlen(string);

    // Check input variables
    if ( (file_name == NULL) || (strlen(file_name) == 0) ) {
        lr_error_message("File name cannot be NULL or empty.");
        lr_abort();
    }

    // TODO: will this detect if the directory does not exist?
    fp = fopen(file_name, "a"); // open file in "append" mode.
    if (fp == NULL) {
        lr_error_message("Error opening file: %s", file_name);
        lr_abort();
    }

    // fprintf returns the total number of characters written to the stream.
    // If a writing error occurs, the error indicator (ferror) is set and a
    // negative number is returned.
    rc = fprintf(fp, "%s", string);
    if (rc != length) {
       lr_error_message("Error writing to file: %s", file_name);
       lr_abort();
    }

    // Close the filestream
    fclose(fp);

    // Return the number of characters written by fprintf
    return rc;
}

/**
 * Reads a text file and saves its contents to a parameter.
 *
 * @param[in] The name of the file to read Note: Include the full path in the file name. and escape
 *            any slashes. E.g. "C:\\TEMP\\file.txt".
 * @param[in] The name of the file to read. Note: Include the full path in the file name.
 * @return    Returns nothing. Reversed string is saved to a parameter.
 *
 * Example code:
 *         // This code controls pacing time, without using runtime settings. Pacing time can be changed
 *         // while the script is running by updating the number of seconds in the dynamic_pacing.txt file
 *         double pacing_time;
 *         int start_time, end_time, time_taken;
 *         extern double atof(const char *string); // Explicit declaration
 *
 *         start_time = time(NULL);
 *
 *         // Insert business process here
 *
 *         end_time = time(NULL);
 *         time_taken = end_time - start_time; // total time spent in business process
 *
 *         // Read the number from the text file, and store it in a variable.
 *         lrlib_read_text_file("C:\\TEMP\\dynamic_pacing.txt", "Param_PacingValue");
 *         pacing_time = (double)atof(lr_eval_string("{Param_PacingValue}"));
 *
 *         // Delay for the amount of time necessary to match the required pacing interval.
 *         lr_think_time(pacing_time - time_taken);
 *
 * Note: the sscanf() function is useful when reading formatted data from a string.
 * Note: if the file is large, then memory can be freed by calling lr_free_parameter().
 */
void lrlib_read_text_file(const char* file_name, const char* output_param_name) {
    int fp; // filestream pointer
    int file_size;
    char* file_contents; // a pointer to a buffer to store the contents of the file


    // Check input variables
    if ( (file_name == NULL) || (strlen(file_name) == 0) ) {
        lr_error_message("file_name cannot be NULL or empty.");
        lr_abort();
    } else if ( (output_param_name == NULL) || (strlen(output_param_name) == 0) ) {
        lr_error_message("output_param_name cannot be NULL or empty.");
        lr_abort();
    }

    // Open the file in binary mode (read-only). File must exist.
    // Opening in text mode is not advised, as newline characters may be converted.
    fp = fopen(file_name, "r+b");
    if (fp == NULL) {
        lr_error_message("File must already exist to read file contents.");
        lr_abort();
    }

    // Set the position indicator associated with the stream to the end of the
    // file. The end of the file is indicated by 2 (SEEK_END), with an offset
    // of 0 bytes.
    fseek(fp, 0, 2);

    // ftell is a standard C function that returns the current value of the
    // position indicator of the stream. For binary streams, this is the number
    // of bytes from the beginning of the file. This function is undocumented
    // in the VuGen 11.04 help file.
    file_size = ftell(fp);

    // Allocate memory to store the file contents (.
    file_contents = (char*)malloc(file_size + sizeof(NULL));
    if (file_contents == NULL) {
        lr_error_message("Unable to allocate memory for file_contents");
        lr_abort();
    }

    // Set the position indicator associated with the stream to the start of the
    // file. The start of the file is indicated by 0 (SEEK_SET), with an offset
    // of 0 bytes.
    fseek(fp, 0, 0);

    // Read the contents of the file
    fread(file_contents, file_size, file_size, fp);

    file_contents[file_size] = NULL; // Null-terminate

    // Save the file contents to a parameter
    lr_save_string(file_contents, output_param_name);

    // Close the filestream
    fclose(fp);

    // Free the memory allocated for the file contents.
    free(file_contents);

    return;
}




// TODO list of functions
// ======================
// * append/write to file with locking
// * include the ferror code for file IO error conditions
// * read a single line from a text file (up to newline character). Could use
//   it with split() function to read CSV data.