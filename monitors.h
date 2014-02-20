// TODO list of functions
// ======================
// * perfmon counters (with perfmon.dll?) (e.g. get CPU utilisation then write
//   it to an lr_user_data_point along with the hostname)
//   see: http://msdn.microsoft.com/en-us/library/windows/desktop/aa373083(v=vs.85).aspx
//   see: http://en.literateprograms.org/CPU_usage_using_performance_counters_(C,_Windows_2000)
// * AWS counters (using the web API)


int lrlib_get_perfmon_counter_list(const char* outputParamArr)
{
    if (outputParamArr == NULL)
    {
        lr_error_message("Output parameter name cannot be NULL.");
        return -1;
    }

    if (strlen(outputParamArr) > LRLIB_MAX_PARAM_NAME_LENGTH)
    {
        lr_error_message("Output parameter name is too long.");
        return -1;
    }

    lrlib_load_dll("pdh.dll");

    {
        unsigned long size = 0;
        const PDH_STATUS initialStatus = PdhEnumObjectsA(NULL, NULL, NULL, &size, PERF_DETAIL_WIZARD, TRUE);
        if (initialStatus != PDH_MORE_DATA)
        {
            lr_error_message("Unexpected PDH code %d.", initialStatus);
            return -1;
        }

        size++;  // A reserve may be needed on some systems

        {
            unsigned int count;

            char* const buffer = (char*)malloc(size * sizeof(char));
            if (buffer == NULL)
            {
                lr_error_message("Error allocating memory (%u bytes).", size);
                return -1;
            }

            {
                const char* current;
                char parameterName[LRLIB_PARAM_NAME_BUFFER_LENGTH];

                const PDH_STATUS status = PdhEnumObjectsA(NULL, NULL, buffer, &size, PERF_DETAIL_WIZARD, FALSE);
                if (status != ERROR_SUCCESS)
                {
                    free(buffer);
                    lr_error_message("Error calling PDH function (%d).", status);
                    return -1;
                }

                current = buffer;
                count = 0;
                while (*current != '\0')
                {
                    const unsigned int length = strlen(current);
                    count++;

                    sprintf(parameterName, "%s_%u", outputParamArr, count);
                    //lr_output_message("%s", current);
                    lr_save_string(current, parameterName);

                    current += length + 1;
                }

                sprintf(parameterName, "%s_count", outputParamArr);
                lr_save_int(count, parameterName);
            }

            free(buffer);
            return count;
        }
    }
}

int lrlib_get_perfmon_counter_item_list(const char* objectName, const char* itemOutputParamArr, const char* instanceOutputParamArr)
{
    if (itemOutputParamArr == NULL || instanceOutputParamArr == NULL)
    {
        lr_error_message("Output parameter name cannot be NULL.");
        return FALSE;
    }

    if (strlen(itemOutputParamArr) > LRLIB_MAX_PARAM_NAME_LENGTH || strlen(instanceOutputParamArr) > LRLIB_MAX_PARAM_NAME_LENGTH)
    {
        lr_error_message("Output parameter name is too long.");
        return FALSE;
    }

    lrlib_load_dll("pdh.dll");

    {
        unsigned long counterListLength = 0;
        unsigned long instanceListLength = 0;

        const PDH_STATUS initialStatus = PdhEnumObjectItemsA(
            NULL,
            NULL,
            objectName,
            NULL,
            &counterListLength,
            NULL,
            &instanceListLength,
            PERF_DETAIL_WIZARD,
            0);
        if (initialStatus != PDH_MORE_DATA)
        {
            lr_error_message("Unexpected PDH code %d.", initialStatus);
            return FALSE;
        }

        {
            char* counterList = (char*)malloc(counterListLength * sizeof(char));
            char* instanceList = (char*)malloc(instanceListLength * sizeof(char));

            if (counterList == NULL || instanceList == NULL)
            {
                lrlib_safe_free_and_null((void**)&counterList);
                lrlib_safe_free_and_null((void**)&instanceList);

                lr_error_message("Error allocating memory.");
                return FALSE;
            }

            {
                const PDH_STATUS status = PdhEnumObjectItemsA(
                    NULL,
                    NULL,
                    objectName,
                    counterList,
                    &counterListLength,
                    instanceList,
                    &instanceListLength,
                    PERF_DETAIL_WIZARD,
                    0);

                if (status != ERROR_SUCCESS)
                {
                    lrlib_safe_free_and_null((void**)&counterList);
                    lrlib_safe_free_and_null((void**)&instanceList);

                    lr_error_message("PDH error: %d.", status);
                    return FALSE;
                }

                if (counterListLength > 0)
                {
                    char parameterName[LRLIB_PARAM_NAME_BUFFER_LENGTH];
                    int count = 0;
                    const char* current;
                    for (current = counterList; *current != '\0'; current += strlen(current) + 1)
                    {
                        count++;

                        sprintf(parameterName, "%s_%u", itemOutputParamArr, count);
                        //lr_output_message("%s", current);
                        lr_save_string(current, parameterName);
                    }

                    sprintf(parameterName, "%s_count", itemOutputParamArr);
                    lr_save_int(count, parameterName);
                }

                if (instanceListLength > 0)
                {
                    char parameterName[LRLIB_PARAM_NAME_BUFFER_LENGTH];
                    const char* current = instanceList;
                    int count = 0;
                    while (*current != '\0')
                    {
                        const unsigned int length = strlen(current);
                        count++;

                        sprintf(parameterName, "%s_%u", instanceOutputParamArr, count);
                        //lr_output_message("%s", current);
                        lr_save_string(current, parameterName);

                        current += length + 1;
                    }

                    sprintf(parameterName, "%s_count", instanceOutputParamArr);
                    lr_save_int(count, parameterName);
                }

                lrlib_safe_free_and_null((void**)&counterList);
                lrlib_safe_free_and_null((void**)&instanceList);
            }
        }
    }

    return TRUE;
}

int lrlib_get_perfmon_counter_value(
    const char* fullCounterPath,
    const DWORD maxSampleCount,
    const DWORD intervalInMsec,
    const DWORD counterFormat,
    const char* outputParamArr)
{
    if (fullCounterPath == NULL)
    {
        lr_error_message("Full counter path cannot be NULL.");
        return FALSE;
    }

    if (outputParamArr == NULL)
    {
        lr_error_message("Output parameter name cannot be NULL.");
        return FALSE;
    }

    if (strlen(outputParamArr) > LRLIB_MAX_PARAM_NAME_LENGTH)
    {
        lr_error_message("Output parameter name is too long.");
        return FALSE;
    }

    if (sizeof(PDH_FMT_COUNTERVALUE) != 16)
    {
        lr_error_message("INTERNAL ERROR: Windows API structure size mismatch.");
        return FALSE;
    }

    {
        int result = TRUE;
        PDH_HQUERY queryHandle = 0;
        PDH_HCOUNTER counterHandle = 0;

        lrlib_load_dll("kernel32.dll");
        lrlib_load_dll("pdh.dll");

        {
            PDH_STATUS openQueryStatus;
            PDH_STATUS addCounterStatus;
            PDH_STATUS initialCollectStatus;
            unsigned long index;
            int count = 0;
            char parameterName[LRLIB_PARAM_NAME_BUFFER_LENGTH];

            openQueryStatus = PdhOpenQueryA(NULL, 0, &queryHandle);
            if (openQueryStatus != ERROR_SUCCESS)
            {
                lr_error_message("Cannot open PDH query (error 0x%08X).", openQueryStatus);
                result = FALSE;
                goto CleanUp;
            }

            addCounterStatus = PdhAddCounterA(queryHandle, fullCounterPath, 0, &counterHandle);
            if (addCounterStatus != ERROR_SUCCESS)
            {
                lr_error_message("Cannot add PDH counter (error 0x%08X).", addCounterStatus);
                result = FALSE;
                goto CleanUp;
            }

            initialCollectStatus = PdhCollectQueryData(queryHandle);
            if (initialCollectStatus != ERROR_SUCCESS && initialCollectStatus != PDH_NO_MORE_DATA)
            {
                lr_error_message("Error collecting data (error 0x%08X).", initialCollectStatus);
                result = FALSE;
                goto CleanUp;
            }

            for (index = 0; index < maxSampleCount; index++)
            {
                Sleep(intervalInMsec);

                {
                    PDH_FMT_COUNTERVALUE itemBuffer = { 0 };
                    PDH_STATUS getValueStatus;
                    char current[64];

                    const PDH_STATUS collectStatus = PdhCollectQueryData(queryHandle);
                    if (collectStatus == PDH_NO_MORE_DATA)
                    {
                        break;
                    }

                    if (collectStatus != ERROR_SUCCESS)
                    {
                        lr_error_message("Error collecting data (error 0x%08X).", collectStatus);
                        result = FALSE;
                        goto CleanUp;
                    }

                    getValueStatus = PdhGetFormattedCounterValue(
                        counterHandle,
                        counterFormat,
                        (LPDWORD)NULL,
                        &itemBuffer);
                    if (getValueStatus != ERROR_SUCCESS)
                    {
                        lr_error_message("Error formatting counter value (error 0x%08X).", getValueStatus);
                        result = FALSE;
                        goto CleanUp;
                    }

                    count++;
                    sprintf(parameterName, "%s_%u", outputParamArr, count);

                    if ((counterFormat & PDH_FMT_DOUBLE) == PDH_FMT_DOUBLE)
                    {
                        sprintf(current, "%.20g", itemBuffer.u.doubleValue);
                    }
                    else if ((counterFormat & PDH_FMT_LONG) == PDH_FMT_LONG)
                    {
                        sprintf(current, "%ld", itemBuffer.u.longValue);
                    }
                    //else if ((counterFormat & PDH_FMT_LARGE) == PDH_FMT_LARGE)
                    //{
                    //    sprintf(current, "%lld", itemBuffer.u.largeValue);
                    //}
                    else
                    {
                        lr_error_message("Unexpected counter format (0x%08X).", counterFormat);
                        result = FALSE;
                        goto CleanUp;
                    }

                    lr_save_string(current, parameterName);
                }
            }

            sprintf(parameterName, "%s_count", outputParamArr);
            lr_save_int(count, parameterName);
        }

    CleanUp:
        if (queryHandle)
        {
            PdhCloseQuery(queryHandle);
        }

        return result;
    }
}
