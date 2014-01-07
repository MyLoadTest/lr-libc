/**
 * Creates a new UUID and saves its string representation to a parameter with the specified name.
 *
 * @param[in] The name of the parameter to save a created UUID to.
 * @return    Returns TRUE (1) if the function succeeded; otherwise, returns FALSE (0).
 */
int lrlib_create_uuid(const char* output_param_name)
{
    const char* LIB_NAME = "Rpcrt4.dll";
    const int GUID_SIZE = 16;
    int rpcStatus;
    
    char* guid;
    unsigned char* str;
    
    const int loadResult = lr_load_dll(LIB_NAME);
    if (loadResult != 0)
    {
        lr_error_message("Error loading '%s' (error code %d).", LIB_NAME, loadResult);
        return FALSE;
    }

    guid = (char*)calloc(GUID_SIZE, 1);
    if (!guid)
    {
        lr_error_message("Error allocating memory.");
        return FALSE;
    }
    
    rpcStatus = UuidCreate(guid);
    if (rpcStatus != 0)
    {
        lr_error_message("Error creating UUID (%d).", rpcStatus);
        free(guid);
        return FALSE;
    }

    rpcStatus = UuidToStringA(guid, &str);
    if (rpcStatus != 0)
    {
        lr_error_message("Error converting UUID to string (%d).", rpcStatus);
        free(guid);
        return FALSE;
    }
    
    lr_save_string((char*)str, output_param_name);

    RpcStringFreeA(&str);
    free(guid);
    
    return TRUE;
}