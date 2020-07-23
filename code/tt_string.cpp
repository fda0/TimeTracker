
#define MAX_TIMESTAMP_STRING_SIZE sizeof("2020-12-31 13:45")
internal void get_timestamp_string(char *output, u32 output_size, time_t time)
{
    tm *s = gmtime(&time);

    Assert(MAX_TIMESTAMP_STRING_SIZE <= output_size);
    snprintf(output, output_size, "%04d-%02d-%02d %02d:%02d", 
             s->tm_year + 1900, s->tm_mon + 1, s->tm_mday,
             s->tm_hour, s->tm_min);
}

internal void get_timestamp_string_for_file(char *output, u32 output_size, time_t time)
{
    tm *s = gmtime(&time);

    snprintf(output, output_size, "%04d-%02d-%02d_%02d-%02d", 
             s->tm_year + 1900, s->tm_mon + 1, s->tm_mday,
             s->tm_hour, s->tm_min);
}

#define MAX_TIME_STRING_SIZE sizeof("00:00")
internal void get_time_string(char *output, u32 output_size, time_t time)
{
    Assert(output_size >= MAX_TIME_STRING_SIZE);
    time = abs(time);

    s32 hours_in_day = 24;
    s32 hours = (s32)((time / Hours(1)) % hours_in_day);

    s32 minutes_in_hour = 60;
    s32 minutes = (s32)((time / Minutes(1)) % minutes_in_hour);
    snprintf(output, output_size, "%02d:%02d", hours, minutes);
}

internal void get_date_string(char *output, u32 output_size, time_t timestamp)
{
    tm *s = gmtime(&timestamp);

    snprintf(output, output_size, "%04d-%02d-%02d", 
             s->tm_year + 1900, s->tm_mon + 1, s->tm_mday);
}

internal void print_offset(s32 offset_sum)
{
    const u32 size = MAX_TIME_STRING_SIZE + 2;
    char offset_time_str[size];

    if (offset_sum > Minutes(1))        offset_time_str[0] = '+';
    else if (offset_sum < -Minutes(1))  offset_time_str[0] = '-';
    else                                offset_time_str[0] = 0;

    if (offset_time_str[0])
    {
        get_time_string(offset_time_str + 1, size - 2, offset_sum);
        offset_time_str[size - 2] = '\t';
        offset_time_str[size - 1] = 0;
    }

    printf("%s", offset_time_str);
}

internal void print_work_time_row(Time_Entry *start, Time_Entry *stop, s32 offset_sum,  
                                  char *replace_stop = NULL)
{
    char start_time_str[MAX_TIME_STRING_SIZE];
    get_time_string(start_time_str, sizeof(start_time_str), start->time);

    char stop_time_str[MAX_TIME_STRING_SIZE];
    if (replace_stop == NULL)
    {
        Assert(stop);
        get_time_string(stop_time_str, sizeof(stop_time_str), stop->time);
        replace_stop = stop_time_str;
    }

    printf("%s -> %s\t", start_time_str, replace_stop);

    print_offset(offset_sum);

    if (start->description)         printf("\"%s\" ", start->description);
    if (stop && stop->description)  printf("\"%s\" ", stop->description);
    
    printf("\n");
}


#define MAX_PROGRESS_BAR_SIZE sizeof("[++++ ++++ ++++ ++++ ++++ +++-> stop is missing!")
internal void get_progress_bar_string(char *output, s32 output_size, time_t time, Missing_Type type)
{
    // NOTE(mateusz): Max output:
    Assert(output_size >= MAX_PROGRESS_BAR_SIZE);
    if (time > Days(1)) time = Days(1);

    s32 hours = (s32)(time / Hours(1) % (Days(1) / Hours(1)));
    s32 minutes = (s32)(time / Minutes(1)) % (Hours(1) / Minutes(1));

    s32 i = 0;
    s32 symbol_counter = 0;
    output[i++] = '[';
    while (hours + 1 > symbol_counter)
    {
        if (symbol_counter > 0 && 
            ((symbol_counter) % 4 == 0))
        {
            output[i++] = ' ';
        }

        if (hours == symbol_counter)
        {
            if (minutes > 0) output[i++] = '-';
            else break;
        }
        else
        {
            output[i++] = '+';
        }

        ++symbol_counter;
    }

    for (int space_index = 0;
         space_index < (4 - (symbol_counter % 4)) % 4;
         ++space_index)
    {
        output[i++] = ' ';
    }

    if (type == Missing_None)          output[i++] = ']';
    if (type == Missing_Assumed)       output[i++] = ')';
    else if (type == Missing_Critical) 
    { 
        i += sprintf(&output[i], "> stop is missing!");
    }

    output[i] = 0;
    Assert(i < output_size);
}



internal void add_ending_slash_to_path(char *path)
{
    u32 length = (u32)strlen(path);
    if (path[length - 1] != '/' ||
        path[length - 1] != '\\')
    {
        path[length] = '/';
        path[length + 1] = 0;
    }
}