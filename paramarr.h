
/**
 * @brief Create a new LoadRunner parameter array from a list of strings.
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



// Note existing LoadRunner functions:
// * lr_paramarr_idx
// * lr_paramarr_len
// * lr_paramarr_random
// * lr_param_increment

// Note that there are already some functions in the strings.h library.

// lrlib_paramarr_unique(param_arr_name, new_param_arr_name). Takes a parameter array, and creates a new parameter array containing only unique values. Note: can implement this the inefficient way, as parameter arrays are generally not large.
// lrlib_paramarr_contains() - returns true of false
// lrlib_paramarr_search() - returns the key number, kind of the opposite of lr_paramarr_idx
// lrlib_paramarr_join(paramarr_name, delimiter)
// lrlib_paramarr_next() -- can use for loop iterator. static variable keeps track of place in loop. returns true while there is still elements.
// lrlib_paramarr_shuffle() - could use shuffle then next, to make sure you don't get repeats (which you would get with random)
// lrlib_paramarr_diff
// lrlib_paramarr_intersect
// lrlib_paramarr_delete() - lr_free_parameter does not work with parameter arrays. You can use it to delete array elements, but not the whole array at once.
// lrlib_paramarr_create()  -convert an array of char*s to a LoadRunner parameter array.

//TODO: how to save a binary blob (containing nulls) to a paramater. lr_save_var?
// TODO: What is max amount of heap memory that can be used when allocating large parameters?


