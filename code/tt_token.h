struct Tokenizer
{
    char *at;
};


enum Token_Type
{
    Token_Unknown,

    Token_Semicolon,

    Token_String,
    Token_Identifier,
    Token_Date,
    Token_Time,

    Token_End_Of_Stream
};

struct Token
{
    Token_Type type;

    size_t text_length;
    char *text;
};