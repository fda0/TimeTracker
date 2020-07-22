#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

//
// NOTE(mateusz): Types
//

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef s32 b32;
typedef s32 b32x;

typedef float f32;
typedef double f64;

#define internal static 
#define global_variable static
#define local_persist static





//
// NOTE(mautesz): Platforms and compilers
//

#define COMPILER_MSVC 1


#if COMPILER_MSVC
#include "win32_platform.cpp"
#endif










//
// NOTE(mateusz): Macros
//

#define Array_Count(Array) (sizeof(Array) / sizeof((Array)[0]))

#define Minimum(A, B) (A > B ? B : A)

#define Kilobytes(Value) ((Value) * 1024)
#define Megabytes(Value) (Kilobytes(Value) * 1024)
#define Gigabytes(Value) (Megabytes(Value) * (u64)1024)
#define Terabytes(Value) (Gigabytes(Value) * (u64)1024)

#define Days(Value) (Hours(Value) * 24)
#define Hours(Value) (Minutes(Value) * 60)
#define Minutes(Value) (Value * 60)


#define Assert(Expression) \
    if(!(Expression)) {*(int*)0 = 1;}

#define Assert_Message(Expression, Message) \
    if(!(Expression)) {puts(Message); __debugbreak(); *(int*)0 = 1;}

#define Invalid_Code_Path Assert(0)






//
//
//

enum Entry_Type
{
    Entry_None,

    Entry_Start,
    Entry_End,
    Entry_Subtract,
    Entry_Add,
};

struct Time_Entry
{
    Entry_Type type;
    time_t date_stamp;
    time_t time;
    char *description;
    Time_Entry *next_in_day;
};

enum Missing_Type
{
    Missing_None,
    Missing_Assumed,
    Missing_Critical
};

struct Day
{
    time_t date_stamp;
    s32 sum;
    Missing_Type missing;
    Time_Entry first_time_entry;
};



#include "tt_memory.h"


struct Program_State
{
    char input_filename[MAX_PATH];
    char archive_directory[MAX_PATH];

    Day days[365]; // TODO(mateusz): Add memory arena support for days.
    u32 day_count;

    Memory_Arena description_arena;
    Memory_Arena struct_arena;

    // TODO(mateusz): Allocate at high virtual address and 
    // allocate more pages from windows if needed.

    // TODO(mateusz): Research if two differenet memory blocks are needed...
    // Are there problems with accessing not byte aligned structs?
    u8 byte_memory_block[Megabytes(8)]; 
    u8 aligned_memory_block[Megabytes(8)];
};

enum Instruction_Type
{
    Ins_None,

    Ins_Time_Entry,
    Ins_Show,
    Ins_Save,
    Ins_Archive,

    Ins_Exit
};

enum Instruction_Flag : u32
{
    Flag_Clear = 0,
    Flag_No_Save = (1 << 0),
};