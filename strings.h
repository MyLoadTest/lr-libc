/**
 * Splits a delimited string and saves the elements to a parameter array
 *
 * @param[in] The delmimited string that is to be split
 * @param[in] The delimiter. Note that this can be a multi-character string, not just a single
 *            character.
 * @param[in] The name of the parameter array to save the elements to.
 * @return    Returns a count of the number of elements the string was broken into. If the
 *            delimiter was not found, then this will be 1. The count is also saved to the _count
 *            member of the parameter array.
 *
 * Example code:
 *     // Getting elements from a row of CSV data.
 *     lr_save_string("comma,separated,values", "Param_Csv")
 *     lrlib_str_split(lr_eval_string("{Param_Csv}"), ",", "ParamArr_Csv");
 *
 * Note: If you are getting a "Error -- memory violation : Exception ACCESS_VIOLATION received."
 * message for the strtok line in this function, then this will be because you are passing in a
 * pointer to a string literal. String literals are read-only, and the strtok function modifies the
 * string that is passed to it.
 *    char* my_string = "comma,separated,values";
 *    lrlib_str_split(my_string, ",", "My_ParamArr"); // Memory violation error.
 */
int lrlib_str_split(char* string_to_split, char* delimiter, const char* output_paramarr_name) {
    char* token; // a pointer to the current position in the tokenized string.
    int num_pieces; // number of pieces that the string has been split into. This is always at
                    // least 1, as long as the string is not null or zero characters long.
    char* param_name; // holds the parameter names for each element of the parameter array.

    // Check input variables
    if ( (string_to_split == NULL) || (strlen(string_to_split) == 0) ) {
        lr_error_message("string_to_split cannot be NULL or empty.");
        lr_abort();
    } else if ( (delimiter == NULL) || (strlen(delimiter) == 0) ) {
        lr_error_message("delimiter cannot be NULL or empty.");
        lr_abort();
    } else if ( (output_paramarr_name == NULL) || (strlen(output_paramarr_name) == 0) ) {
        lr_error_message("output_param_name cannot be NULL or empty.");
        lr_abort();
    }

    // Allocate memory for the buffer. It must be be large enough to contain the
    // {ParameterName_count} parameter name.
    param_name = (char*)malloc(strlen(output_paramarr_name) + strlen("_count"));
    if (param_name == NULL) {
        lr_error_message("Unable to allocate memory for param_name.");
        lr_abort();
    }

    // Note regarding implicit declarations: we do not need to explicitly declare strtok  or strcat
    // (even though they don't return an int), as we are typecasting its return value, and
    // sizeof(char*) is the same as sizeof(int).
    token = (char*)strtok(string_to_split, delimiter); // Get the first token
    if (token == NULL) {
        // The delimiter was not found in string_to_split, so the entire string is saved to the
        // output parameter. The output parameter should be called {ParameterName_1}
        strcpy(param_name, output_paramarr_name);
        lr_save_string(string_to_split, (char*)strcat(param_name, "_1"));
    }

    while (token != NULL) { // While valid tokens are returned.
        num_pieces++;
        // Create the {ParameterName_x} parameter for each element of the parameter array.
        sprintf(param_name, "%s_%d", output_paramarr_name, num_pieces);
        lr_save_string(token, param_name);
        token = (char*)strtok(NULL, delimiter); // Get the next token.
    }

    // Create a {ParameterName_count} parameter, so that the lr_paramarr_* functions may be used.
    strcpy(param_name, output_paramarr_name);
    lr_save_int(num_pieces, (char*)strcat(param_name, "_count"));

    // Free the small amount of memory allocated for the parameter name.
    free(param_name);

    // Return the numer of pieces that the string was split into. If the delimiter was not found,
    // then this will be 1.
    return num_pieces;
}

// TODO list of functions
// ======================
// * replace all occurrances of substring with new string (str_replace). Dont use this instead of web_convert_param to convert to/from URLEncoded or HTML entities.
// * a "generate GUID" function (using the Windows GUIDFromString function). Alternatively, use lr_param_unique()
// * trim - remove leading/trailing whitespace from a string.
// * get a substring between a LB/RB, save to parameter (no ord=All option). match is lazy (not greedy)
// * sapeventqueue_encode/sapeventqueue_decode
// * reverse a string
