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

    fclose(fp);

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

    fclose(fp);

    return bytes;
}

/**
 * Writes a string to the end of a file.
 *
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

// TODO list of functions
// ======================
// * append/write to file with locking
// * read the contents of a file from the filesystem lrlib_read_text_file
//   (e.g. and then sscanf a number from it)