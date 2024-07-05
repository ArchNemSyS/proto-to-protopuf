#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <string_view>
#include <vector>

class Parser;

struct ParsedFile
{
    std::string filename;
    Parser*     parser;
};


class Parser
{
static std::vector<ParsedFile> parsed_files;


private:
    std::string_view m_source;
    std::string m_output;
    std::vector<std::string_view> m_enums;
    std::vector<std::string_view> m_messages;
    std::vector<std::string_view> m_tokens;

    std::vector<std::string_view>::iterator it;

    std::vector<std::pair<std::string_view, std::string_view>> protopuf_types {
        { "int32", "int32_field" },
        { "unit32", "uint32_field" },
        { "int64", "int64_field" },
        { "uint64", "uint64_field" },
        { "float", "float_field" },
        { "double", "double_field" },
        { "string", "string_field" },
        { "bool", "bool_field" }
    };


    void forward_symbol();

    void rewrite_comment();
    void rewrite_include();
    void rewrite_enum();

    void rewrite_field_comment();
    void rewrite_message_field(std::string comma);
    void rewrite_message();

    bool tokenize();
    bool parse(std::string_view source);

public:
    Parser();

    static Parser *parseFile(std::string filename);

    void merge_symbols(Parser&, Parser&);

};

#endif // PARSER_H
