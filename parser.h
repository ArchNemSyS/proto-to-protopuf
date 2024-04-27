#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <string_view>
#include <vector>


class Parser
{
private:
    std::string_view m_source;
    std::string m_output;
    std::vector<std::string_view> m_enums;
    std::vector<std::string_view> m_messages;
    std::vector<std::string_view> m_tokens;

    std::vector<std::string_view>::iterator it;

    void rewrite_comment();
    void rewrite_include();
    void rewrite_enum();

    void rewrite_message_field();
    void rewrite_message();

    bool tokenize();

public:
    Parser();
    bool parse(std::string_view source);

};

#endif // PARSER_H
