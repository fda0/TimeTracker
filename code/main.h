#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



//~ NOTE: Types

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef s32 b32;

typedef time_t date64;
typedef s32 time32;

typedef float f32;
typedef double f64;

#define internal static
#define global_variable static
#define local_persist static


#define S64_MAX LLONG_MAX



struct Thread_Memory
{
    char input_buffer[256];
    char cursor[64];
    b32 new_data;
};



//~ NOTE: Platforms and compilers


#if Def_Windows
#include "win32_platform.cpp"
#else
#error "system not defined"
#endif



//~ NOTE: Macros

#define Macro_Wrap(Macro) \
    do                    \
    {                     \
        Macro             \
    } while (0)

#if BUILD_INTERNAL
#define Assert(Expression)          \
    Macro_Wrap(if (!(Expression)) { \
        __debugbreak();             \
        *(int *)0 = 1;              \
    })
#else
#define Assert(Expression)
#endif

#define Invalid_Code_Path Assert(0)



#define Array_Count(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Minimum(A, B) (A > B ? B : A)

#define Kilobytes(Value) ((Value)*1024ULL)
#define Megabytes(Value) (Kilobytes(Value) * 1024ULL)
#define Gigabytes(Value) (Megabytes(Value) * (u64)1024ULL)
#define Terabytes(Value) (Gigabytes(Value) * (u64)1024ULL)

#define Days(Value) (Hours(Value) * 24)
#define Hours(Value) (Minutes(Value) * 60)
#define Minutes(Value) (Value * 60)


//~ NOTE: Data types

struct File_Path2
{
    char file_name[MAX_PATH];
    char directory[MAX_PATH];
};



enum Missing_Ending
{
    MissingEnding_None,
    MissingEnding_Assumed,
    MissingEnding_Critical
};


struct Description
{
    char *content;
    s32 length;
};

struct Defered_Description
{
    Description description;
    s32 value;
};

enum Record_Type : s32
{
    Record_Empty,
    Record_TimeStart,
    Record_TimeStop,
    Record_TimeDelta,
    Record_CountDelta
};

struct Record
{
    date64 date;

    Record_Type type;
    s32 value;

    Description description;
};



#include "memory.h"



// TODO: Pull out char[MAX_PATH] to StrMaxPath?

struct Program_State
{
    Dynamic_Array<Defered_Description> defered_descs;
    Memory_Arena mixed_arena;
    Dynamic_Array<Record> records;

    char archive_directory[MAX_PATH];
    File_Path2 executable_path2;
    char input_file_name[MAX_PATH];
    char input_file_full_path[MAX_PATH]; // TODO: Collapse to File_Path2

    File_Time input_file_mod_time;
    date64 timezone_offset;

    s32 parse_error_count;
    s32 change_count;

    b32 reading_from_file;
};


enum Condition
{
    Con_NoMatchigTokens,
    Con_HasErrors,
    Con_IsValid,
};

inline b32
is_condition_valid(Condition condition)
{
    b32 result = (condition == Con_IsValid);
    return result;
}

struct Parse_Date_Result
{
    date64 date;
    b32 is_valid;
};

struct Parse_Complex_Date_Result
{
    date64 date;
    Condition condition;
};

struct Parse_Time_Result
{
    s32 time;
    b32 is_valid;
};

struct Parse_Number_Result
{
    s32 number;
    b32 is_valid;
};



struct Date_Range_Result
{
    union {
        date64 date_ranges[2];
        struct
        {
            date64 begin;
            date64 end; // NOTE: Inclusive end
        };
    };
    Condition condition;
};
