#ifndef va_list
typedef unsigned char* va_list; // Type to hold information about variable arguments
#endif

#ifndef va_start
#define va_start(ap,v)  (ap = (va_list)&v + sizeof(v))
#endif

#ifndef va_arg
#define va_arg(ap,t)    (*(t*)((ap += sizeof(t)) - sizeof(t)))
#endif

#ifndef va_end
#define va_end(ap)      (ap = (va_list)0)
#endif

/**
  @brief Creates a new LoadRunner parameter array from a list of strings.
 
  @param paramarr_name The name of the new parameter array.
  @param paramarr_elements The elements of the new parameter array (the parameter array must have
         at least one element). Note that the last element must be "LAST", just like the other
         LoadRunner functions that accept a variable number of arguments.
  @return Returns the number of elements in the new parameter array.
 
  @example:
 
  Action()
  {
      int i;
      lrlib_paramarr_create("MyParamArray", "a", "b", "c", "d", "z", LAST);
      for (i = 1; i <= lr_paramarr_len("MyParamArr"); i++)
      {
          lr_output_message("element %d: %s", i, lr_paramarr_idx("MyParamArr", i));
      }
 
      return 0;
  }
 
 */
int lrlib_paramarr_create(const char* paramarrName, ...)
{
    if (paramarrName == NULL)
    {
        lr_error_message("ParamArray name cannot be NULL.");
        return -1;
    }

    {
        int count = 0;
        const char* param;
        char* parameterName = (char*)malloc(strlen(paramarrName) + 32);
        
        va_list args;
        va_start(args, paramarrName);
        for (param = va_arg(args, char*); param != LAST; param = va_arg(args, char*))
        {
            count++;
            sprintf(parameterName, "%s_%d", paramarrName, count);
            lr_save_string(param, parameterName);
        }
    
        va_end(args);
        
        sprintf(parameterName, "%s_count", paramarrName);
        lr_save_int(count, parameterName);
    
        free(parameterName);
    
        return count;
    }
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
 * @brief Finds the position of a string element in a LoadRunner parameter array.
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

/**
 * @brief Adds an element to the end of a LoadRunner parameter array.
 *
 * @param paramarr_name The name of the parameter array to add the element to.
 * @param element_to_add The string to add to the end of the parameter array.
 * @return Returns the position of the element that was just added to the parameter array.
 *         Note: this is also the new array length.
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
 *     lr_output_message("There are %d elements in the array.", lr_paramarr_len("MyParamArray"));
 *
 *     // Add an element to the end of the parameter array.
 *     lrlib_paramarr_push("MyParamArray", "four");
 *     lr_output_message("There are %d elements in the array.", lr_paramarr_len("MyParamArray"));
 *
 *     return 0;
 * }
 *
 */
int lrlib_paramarr_push(char* paramarr_name, char* element_to_add) {
    int num_elements;
    char* element_name = (char*)malloc(strlen(paramarr_name) + strlen("_count") + sizeof(NULL));

    // TODO: Check that the parameter array exists

    num_elements = lr_paramarr_len(paramarr_name);

    // Add the new element to the end of the array.
    sprintf(element_name, "%s_%d", paramarr_name, num_elements + 1);
    lr_save_string(element_to_add, element_name);

    // Increase the parameter element count by 1.
    sprintf(element_name, "%s_count", paramarr_name);
    lr_save_int(num_elements + 1, element_name);

    free(element_name);

    return num_elements + 1;
}

/**
 * @brief Removes a parameter array element from the end of the array.
 *
 * @param paramarr_name The name of the parameter array to remove the element from.
 * @param output_param_name The name of a parameter to which the element that has just been removed
 *        from the array should be saved.
 * @return Returns the position of the element that was just removed from the parameter array.
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
 *     lr_output_message("There are %d elements in the array.", lr_paramarr_len("MyParamArray"));
 *
 *     // Remove an element from the end of the parameter array.
 *     lrlib_paramarr_pop("MyParamArray", "RemovedElement");
 *     lr_output_message("Removed %s. There are %d elements in the array.",
 *         lr_eval_string("{RemovedElement}"), lr_paramarr_len("MyParamArray"));
 *
 *     return 0;
 * }
 *
 */
int lrlib_paramarr_pop(char* paramarr_name, char* output_param_name) {
    int num_elements;
    char* element_name = (char*)malloc(strlen(paramarr_name) + strlen("_count") + sizeof(NULL));

    // TODO: Check that the parameter array exists
    // TODO: what happens when the array is empty?

    num_elements = lr_paramarr_len(paramarr_name);

    // Get the last element of the parameter array, save it to the new parameter,
    // then delete the element.
    lr_save_string(lr_paramarr_idx(paramarr_name, num_elements), output_param_name);
    sprintf(element_name, "%s_%d", paramarr_name, num_elements);
    lr_free_parameter(element_name);

    // Decrease the parameter element count by 1.
    sprintf(element_name, "%s_count", paramarr_name);
    lr_save_int(num_elements - 1, element_name);

    free(element_name);

    return num_elements; // TODO: not sure what the best return value for this function is.
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
