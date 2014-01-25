
/**
 * @brief Creates a new LoadRunner parameter array from a list of strings.
 *
 * @param paramarr_name The name of the new parameter array.
 * @param paramarr_elements The elements of the new parameter array (the parameter array must have
 *        at least one element). Note that the last element must be "LAST", just like the other
 *        LoadRunner functions that accept a variable number of arguments.
 * @return Returns the number of elements in the new parameter array.
 *
 * @example:
 *
 * Action()
 * {
 *     int i;
 *     lrlib_paramarr_create("MyParamArray", 'a', 'b', 'c', 'd', 'z');
 *     for(i=0; i<lr_paramarr_len("MyParamArr"); i++) {
 *         lr_output_message("element %d", i, lr_paramarr_idx("MyParamArr", i));
 *     }
 *
 *     return 0;
 * }
 *
 */
int lrlib_paramarr_create(const char* paramarr_name, ...) {

    int i;

    // Snippet of stdarg.h included here so that varargs will work. Could just replace the #defines with the macro-expanded code, as this only has to work for a single data type.
    //typedef void* va_list; // Type to hold information about variable arguments
    //#define va_start(ap,v)  (ap = (va_list)&v + sizeof(v))
    //#define va_arg(ap,t)    (*(t *)((ap += sizeof(t)) - sizeof(t)))
    //#define va_end(ap)      (ap = (va_list)0)

    //va_start // Initialize a variable argument list
    //va_arg // Retrieve next argument
    //va_end // End using variable argument list (must be called at the end of the function using varargs)
    //va_copy // Copy variable argument list (macro )

    // Loop through arguments until arg == "LAST"
    //         Save each argument as an element of the parameter array.
    //         Array elements are called "paramarr_num, where num is the position of the element (starting from 1).
    //         lr_save_string(paramarr_name, va_arg(ap, char*);
    // Create a parameter_name_count parameter, with the total number of array elements in it.

    return i; // total number of elements in the parameter array.
}

/**
 * @brief Deletes a LoadRunner parameter array
 *
 * @note The lr_free_parameter function does not work with parameter arrays. You can use it to
 *       delete array elements, but not the whole array at once.
 *
 * @param paramarr_name The name of the parameter array to delete.
 * @return Returns the number of array elements that were deleted (including the _count array
 *         element).
 *
 * @example:
 *
 * Action()
 * {
 *     // Simulate the creation of a parameter array.
 *     // Note: Parameter array are usually created with with web_reg_save_param using ORD=All".
 *     lr_save_string("one", "MyParamArray_1");
 *     lr_save_string("two", "MyParamArray_2");
 *     lr_save_string("three", "MyParamArray_3");
 *     lr_save_string("3", "MyParamArray_count");
 *
 *     // Delete all elements of the parameter array
 *     lrlib_paramarr_delete("MyParamArray");
 *
 *     return 0;
 * }
 *
 */
int lrlib_paramarr_delete(char* paramarr_name) {
    int i;
    int num_elements;
    char* element_name = (char*)malloc(strlen(paramarr_name) + strlen("_count") + sizeof(NULL));

    // TODO: Check that the parameter array exists

    num_elements = lr_paramarr_len(paramarr_name);
    for(i=1; i<=num_elements; i++) {
        sprintf(element_name, "%s_%d", paramarr_name, i);
        lr_free_parameter(element_name);
    }
    sprintf(element_name, "%s_count", paramarr_name);
    lr_free_parameter(element_name);

    free(element_name);
    return i; // total number of elements in the parameter array.
}

/**
 * @brief Checks whether a LoadRunner parameter array contains a particular string element.
 *
 * @param paramarr_name The name of the parameter array to search.
 * @param element_to_find The string to find in the parameter array.
 * @return Returns TRUE (1) if the element was found in the parameter arry, otherwise returns
 *         FALSE (0).
 *
 * @example:
 *
 * Action()
 * {
 *     // Simulate the creation of a parameter array.
 *     // Note: Parameter array are usually created with with web_reg_save_param using ORD=All".
 *     lr_save_string("one", "MyParamArray_1");
 *     lr_save_string("two", "MyParamArray_2");
 *     lr_save_string("three", "MyParamArray_3");
 *     lr_save_string("3", "MyParamArray_count");
 *
 *     // Check to see if "two" is in the parameter array
 *     if(lrlib_paramarr_contains("MyParamArray", "two") ==  TRUE) {
 *         lr_output_message("Found element in parameter array.");
 *     } else {
 *         lr_output_message("Could not find element.");
 *     }
 *
 *     return 0;
 * }
 *
 */
int lrlib_paramarr_contains(char* paramarr_name, char* element_to_find) {
    int i;
    int num_elements;
    int element_found = FALSE;

    // TODO: Check that the parameter array exists

    num_elements = lr_paramarr_len(paramarr_name);
    for(i=1; i<=num_elements; i++) {
        if (strcmp(lr_paramarr_idx(paramarr_name, i), element_to_find) == 0) {
            element_found = TRUE;
            break;
        }
    }

    return element_found;
}

/**
 * @brief Finds the position of a string element in a a LoadRunner parameter array.
 *
 * @param paramarr_name The name of the parameter array to search.
 * @param element_to_find The string to find in the parameter array.
 * @return Returns the position of the element in the array (first element is 1). If the element
 *         is not found, the function returns 0.
 *
 * @example:
 *
 * Action()
 * {
 *     int position;
 *
 *     // Simulate the creation of a parameter array.
 *     // Note: Parameter array are usually created with with web_reg_save_param using ORD=All".
 *     lr_save_string("one", "MyParamArray_1");
 *     lr_save_string("two", "MyParamArray_2");
 *     lr_save_string("three", "MyParamArray_3");
 *     lr_save_string("3", "MyParamArray_count");
 *
 *     // At what position is "two" in the parameter array?
 *     position = lrlib_paramarr_search("MyParamArray", "two");
 *     if(position > 0) {
 *         lr_output_message("Found element %s in parameter array at position %d.",
 *             lr_paramarr_idx("MyParamArray", position), position);
 *     } else {
 *         lr_output_message("Could not find element.");
 *     }
 *
 *     return 0;
 * }
 *
 */
int lrlib_paramarr_search(char* paramarr_name, char* element_to_find) {
    int i;
    int num_elements;
    int element_found = FALSE;

    // TODO: Check that the parameter array exists

    num_elements = lr_paramarr_len(paramarr_name);
    for(i=1; i<=num_elements; i++) {
        if (strcmp(lr_paramarr_idx(paramarr_name, i), element_to_find) == 0) {
            element_found = TRUE;
            break;
        }
    }

    return element_found;
}



// Note existing LoadRunner functions:
// * lr_paramarr_idx
// * lr_paramarr_len
// * lr_paramarr_random
// * lr_param_increment
// * lr_free_parameter

// Note that there are already some functions in the strings.h library.

// lrlib_paramarr_unique(param_arr_name, new_param_arr_name). Takes a parameter array, and creates a new parameter array containing only unique values. Note: can implement this the inefficient way, as parameter arrays are generally not large.
// lrlib_paramarr_join(paramarr_name, delimiter)
// lrlib_paramarr_next() -- can use for loop iterator. static variable keeps track of place in loop. returns true while there is still elements.
// lrlib_paramarr_shuffle() - could use shuffle then next, to make sure you don't get repeats (which you would get with random)
// lrlib_paramarr_diff
// lrlib_paramarr_intersect


//TODO: how to save a binary blob (containing nulls) to a paramater. lr_save_var?
// TODO: What is max amount of heap memory that can be used when allocating large parameters?
