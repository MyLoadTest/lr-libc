
// These functions depend on C:\Program Files\HP\LoadRunner\bin\pcre-8.10.dll.
// Note that this file is not present in older versions of VuGen (and may be updated in
// future versions), so it might be a good idea to include some kind of version check.

// Note: regular expression support was added for the web_reg_save_param_ex function, but LoadRunner still does
// not have support for regular expressions that work with normal strings.

// PCRE Functions
// --------------
// lrlib_preg_filter: Perform a regular expression search and replace
// lrlib_preg_grep: Return array entries that match the pattern
// lrlib_preg_last_error: Returns the error code of the last PCRE regex execution
// lrlib_preg_match_all: Perform a global regular expression match
// lrlib_preg_match: Perform a regular expression match
// lrlib_preg_quote: Quote regular expression characters
// lrlib_preg_replace_callback: Perform a regular expression search and replace using a callback
// lrlib_preg_replace: Perform a regular expression search and replace
// lrlib_preg_split: Split string by a regular expression (see also: lrlib_str_split)

