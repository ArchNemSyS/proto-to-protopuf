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
    std::vector<std::string_view> m_tokens;

    void rewrite_comment(std::vector<std::string_view>::iterator &it);
    void rewrite_include(std::vector<std::string_view>::iterator &it);
    void rewrite_enum(std::vector<std::string_view>::iterator &it);
    void rewrite_message(std::vector<std::string_view>::iterator &it);

    bool tokenize();

public:
    Parser();
    bool parse(std::string_view source);

};

#endif // PARSER_H
