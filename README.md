Additional C Functions for LoadRunner
=====================================
This is a collection of C functions that I have written for use in LoadRunner scripts.

Documentation
-------------
All lr-libc functions have been documented with Doxygen annotations. This allows HTML documentation to be automatically generated from the source code. I will put the documentation online using GitHub Pages as soon as I have figured out how to generate non-horrible looking HTML with Doxygen.

Software License
----------------
This library is licensed under the GNU Lesser General Public License ([LGPL](http://en.wikipedia.org/wiki/GNU_Lesser_General_Public_License)) Version 3. You may freely include it in your own LoadRunner scripts with no other requirements or obligations.

How to contribute to lr-libc
----------------------------
If you have a function you think that others would find useful, your code would be appreciated.

1. Sign up for Github
2. Fork the [lr-libc](https://github.com/MyLoadTest/lr-libc) Repository
3. Make your change and submit a pull request

Unit Tests
----------
I have created a unit test harness to help contributors to lr-libc ensure that their changes don't break anything. Running the test harness requires that both PowerShell and VuGen are installed on your development PC.

Design Philosophy
-----------------
LoadRunner scripts aren't a typical type of program; they are short pieces of "throw-away code" typically written by a single person in less than a day. They are very tightly coupled with the application that they have been written to test or monitor. The majority of code in a script is automatically generated from a network capture of the application under test. When the application changes, a user will probably re-record the business process, then cut and paste code snippets from the old script into the new one. Some coding behaviours that would be "best practice" for other types of software development can be an anti-pattern for LoadRunner script development.

Most LoadRunner users don't have more than a basic understanding of C, and forcing them write code that does any kind of memory allocation frequently produces C interpreter run time errors like "Error -- memory violation : Exception ACCESS_VIOLATION received". Library functions should not require a user to allocate a buffer to pass to a function, or require the user to free() an area of memory after the function has returned. **If string values are to be returned from a function, they should be saved to a {parameter}, with the parameter name supplied as a function argument.**

Because LoadRunner users don't write C code every day, they might need a little hand-holding, especially if they need to modify an existing function. **All lr-libc functions should include some example code as well as clear documentation describing the purpose of the function and any caveats**. With comments in the function body, it is important to capture not just the _intent_ of the code, but also the steps - even if they would be obvious to someone who is a professional C programmer. **If there is a short and clever way of doing something and a long and obvious way of doing something, then the library should use the obvious solution**. Functions should never use the ternary operator.

Don't ever expect users to check the return code of a library function...or even to notice that there is a Warning in the Replay Log. **If a function is given invalid input, then it should fail noisily** with a descriptive lr_error_message() and then by stopping the execution of the script with lr_abort().

LoadRunner scripts are not distributed as compiled code. A script is a folder containing *.c files, and other files needed for replay. When the script is run, it is transferred to a load generator or BPM host and _then_ it is compiled. Rather than copying *.h library files to each load generator, it is better to embed them into the script, so that the script is a complete, self-contained item. This is especially important if the script is to be stored in a repository such as Performance Center or BSM.

External dependencies should be minimised. It is best if library functions only use the C functions that are included with LoadRunner (including the C Standard Library), or standard Windows functions. It is rare for LoadRunner scripts to be run on non-Windows operating systems (although this is possible for some vuser types). **Platform-specific functions should be clearly labelled with the operating system they will work with.** 

It is better for library function source code to be visible (capable of being modified and debugged) in the script, that it is for it to be hidden inside a DLL. Users of lr-libc may choose to cut and paste a single function into their script, rather than include the entire library. **Each lr-libc function should be independent, and should not refer to other functions in the library.**

Finally, **indentation should follow the K&R Style** and **use 4 spaces instead of tabs**. All **function names should be prefixed with "lrlib_"** (C does not have namespaces, so this avoids collisions of function names), **function names and variable names should use underscores** (e.g. lrlib_save_file()).

Alternatives to lr-libc
-----------------------
LoadRunner already includes a large number of userful functions, including most of the standard C library, functions specific to the various vuser types, and other protocol-agnostic functions (starting with "lr_"). Before you spend time writing your own functions, it is worth double-checking to see if HP has already solved your problem.

Remember that LoadRunner scripts are standard C, so you will be able to find useful code snippets all across the Internet. You can also use functions from third-party DLLs by calling lr_load_dll() from your script.

LoadRunner-specific functions can be found on the HP Software Support website, on various blogs and forums, and in the [Ylib](https://github.com/randakar/y-lib) library by Floris Kraak.