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

// This function replaces unreserved characters in a string with their encoded values.
// Encoding is in the style of SAP Web Dynpro. E.g. "abd*def" becomes "abc~002Adef".
// Reserved/unreserved characters are according to RFC3986 (http://tools.ietf.org/html/rfc3986)
// This function returns a pointer to the start of the encoded string (buf).
// Note that buf must be big enough to hold original string plus all converted entities.
int lrlib_sapeventqueue_encode(char* plain_string, char* buf) {
    int len = strlen(plain_string);
    int i,j;
    char hex_value[3];

    if (plain_string == NULL) {
        lr_error_message("Input string is empty.");
        return NULL;
    }

    for (i=0, j=0; i<len; i++) {
        // Check if character is in list of allowed characters.
        // A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
        // a b c d e f g h i j k l m n o p q r s t u v w x y z
        // 0 1 2 3 4 5 6 7 8 9 - _ . ~
        if ( (plain_string[i] >= 'A' && plain_string[i] <= 'Z') ||
                 (plain_string[i] >= 'a' && plain_string[i] <= 'z') ||
                 (plain_string[i] >= '0' && plain_string[i] <= '9') ||
                 (plain_string[i] == '-') ||
                 (plain_string[i] == '_') ||
                 (plain_string[i] == '.') ||
                 (plain_string[i] == '~') ) {

            buf[j++] = plain_string[i];
        } else if ( (plain_string[i] < 32 ) || (plain_string[i] > 126) ) {
            lr_error_message("Input string contains non-printable or non-ASCII character %c at position: %d", plain_string[i], i);
            return NULL;
        } else {
            // The unicode value for use in url encoding is the same as the hex value for the ASCII character
            itoa(plain_string[i], hex_value, 16);
            buf[j++] = '~';
            buf[j++] = '0';
            buf[j++] = '0';
            buf[j++] = toupper(hex_value[0]);
            buf[j++] = toupper(hex_value[1]);
        }
    }

    buf[j] = NULL; // terminate the string
    return buf;
}

// This function replaces encoded characters from with their non-encoded value.
// Decoding is in the style of SAP Web Dynpro. E.g. "abc~002Adef" becomes "abd*def".
// Reserved characters are according to RFC3986 (http://tools.ietf.org/html/rfc3986)
// This function returns a pointer to the start of the decoded string (buf).
// Note that buf must be big enough to hold the decoded string (always equal to or shorter than the encoded string).
char* lrlib_sapeventqueue_decode(char* enc_string, char* buf) {
    int len = strlen(enc_string);
    int i, j;
    char code[3]; // holds url encoded value e.g. "2F" (/)
    int hex; // decimal value of hex code e.g. 47 (0x2F)
    int rc; // return code

    if (enc_string == NULL) {
        lr_error_message("Input string is empty.");
        return NULL;
    }

    for (i=0, j=0; i<len; i++, j++) {
        // Only convert entities that do not start with "~E". Do not run off the end of the string.
        if ( (enc_string[i] == '~') &&
                 (enc_string[i+1] != 'E') &&
                 ((i+4) < len) &&
                 (enc_string[i+1] == '0') &&
                 (enc_string[i+2] == '0') &&
                 (isalpha(enc_string[i+3]) || isdigit(enc_string[i+3])) &&
                 (isalpha(enc_string[i+4]) || isdigit(enc_string[i+4])) ) {
            // Get the hex value from the input string
            code[0] = enc_string[i+3];
            code[1] = enc_string[i+4];
            code[3] = NULL;
            // Convert the hex value to the appropriate character, and add it to buf
            rc = sscanf(code, "%2x", &hex);
            if (rc != 1) {
                lr_error_message("Invalid hex value: %s", code);
            }
            buf[j] = hex;
            i+=4; // skip the rest of this encoded value in the input string
        } else {
            buf[j] = enc_string[i];
        }
    }

    return buf;
}

// TODO list of functions
// ======================
// * replace all occurrances of substring with new string (str_replace). Dont use this instead of web_convert_param to convert to/from URLEncoded or HTML entities.
// * a "generate GUID" function (using the Windows GUIDFromString function). Alternatively, use lr_param_unique()
// * trim - remove leading/trailing whitespace from a string.
// * get a substring between a LB/RB, save to parameter (no ord=All option). match is lazy (not greedy)
// * reverse a string
