Action()
{
    const int res = lrlib_create_uuid("test_guid");
    lr_output_message("res = %d, param = '%s'.", res, lr_eval_string("{test_guid}"));

    return 0;
}