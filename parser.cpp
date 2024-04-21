#include "parser.h"

#include <iostream>
#include <algorithm>


Parser::Parser(){}
void Parser::rewrite_comment(std::vector<std::string_view>::iterator &it)
{
    m_output.append(*it);
    m_output.append("\n");
}

void Parser::rewrite_include(std::vector<std::string_view>::iterator &it)
{
/*
import "filename.proto";
    --convert--
include "filename.proto.h"
*/
    m_output.append("include ");

    std::advance(it,1); // "filename.proto"
    it->remove_suffix(1);

    m_output.append( *it );
    m_output.append(".h\"\n"); // "filename.proto.h"

}

void Parser::rewrite_enum(std::vector<std::string_view>::iterator &it)
{
    /*
    enum TYPE {
        FIELD_NAME = INT;
        FIELD_NAME = INT;
    }

    --convert--

        enum TYPE {
            FIELD_NAME = INT,
            FIELD_NAME = INT,
        }
    */


    m_output.append("enum ");   // enum
    std::advance(it,1);         // TYPE
    m_output.append(*it);

    m_output.append("{ \n\t");    // {
    std::advance(it,2);


    while(*it != "}")
    {
        if(*it == ";")
        {
            if( std::next(it)->starts_with("//") || std::next(it)->starts_with("/*") )
            {
                m_output.append(",");
                std::advance(it,1);
                m_output.append(*it);
                m_output.append("\n\t");
            }
            else
            {
                m_output.append(",");
                m_output.append("\n\t");
            }
        }
        else if( it->starts_with("//") || it->starts_with("/*") )
        {
            m_output.append(*it);
            m_output.append("\n\t");
        }
        else
        {
            m_output.append(*it);
        }
        std::advance(it,1);
    }

    m_output.append("}\n\n");


}



void Parser::rewrite_message(std::vector<std::string_view>::iterator &it)
{
/*
    message Student {
        uint32 id = 1;
        string name = 3;
    }

    message Class {
        string name = 8;
        repeated Student students = 3;
    }

--convert--

    using Student = message<
        uint32_field<"id", 1>,
        string_field<"name", 3>
    >;

    using Class = message<
        string_field<"name", 8>,
        message_field<"students", 3, Student, repeated>
    >;
 */

    m_output.append("using ");   // message
    std::advance(it,1);         // TYPE
    m_output.append(*it);


    m_output.append(" = message< \n\t");    // {
    std::advance(it,2);

    // foreach subrange contains x do y ?
    // last no comma

    while(*it != "}")
    {
        if(*it == ";")
        {
            if( std::next(it)->starts_with("//") || std::next(it)->starts_with("/*") )
            {
                m_output.append(",");
                std::advance(it,1);
                m_output.append(*it);
                m_output.append("\n\t");
            }
            else
            {
                m_output.append(",");
                m_output.append("\n\t");
            }
        }
        else if( it->starts_with("//") || it->starts_with("/*") )
        {
            m_output.append(*it);
            m_output.append("\n\t");
        }
        else
        {
            m_output.append(*it);
        }
        std::advance(it,1);
    }

    m_output.append(">\n\n");

}


bool Parser::tokenize()
{
    //will get fooled by weird filespaths literals that look like comments


    constexpr std::string_view delims(";/\n ");

    for (auto first = m_source.data(), second = m_source.data(), last = first + m_source.size(); second != last && first != last; first = second + 1) {
        second = std::find_first_of(first, last, std::cbegin(delims), std::cend(delims));


        if(*first == '/')
        {
            auto next = std::next(first);
            const char* end = last;
            if(*next == '/')
            {

                //single line comment
                end = std::find(first, last, '\n');

                if (end != last)
                {
                    m_tokens.emplace_back(first, end - first);
                    second = end;
                }
            }
            else if(*next == '*')
            {
                //multiline comment
                constexpr std::string_view multiend("*/");
                end = std::search(next, last, multiend.begin(), multiend.end());

                if (end != last)
                {
                    m_tokens.emplace_back(first, end+2 - first);
                    second = end;
                }
            }
        }
        else if(*second == ';')
        {
            m_tokens.emplace_back(first, second - first);
            m_tokens.emplace_back(";");
        }
        else
        {
            // split by whitespace newline
            if (first != second)
            {
                m_tokens.emplace_back(first, second - first);
            }
        }



    }

    return true;
}

bool Parser::parse(std::string_view source)
{
    //m_output.clear();
    m_output.reserve(source.size());
    m_source = source;

    if(tokenize())
    {
        for (auto it = m_tokens.begin(); it != m_tokens.end(); ++it) {

            // current index
            //auto i = std::distance(m_tokens.begin(), it);
            //std::cout << i << "\n";

            if      (it->starts_with("//") || it->starts_with("/*"))     rewrite_comment(it);
            else if (it->starts_with("import"))                          rewrite_include(it);
            else if (it->starts_with("enum"))                            rewrite_enum(it);
            else if (it->starts_with("message"))                         rewrite_message(it);
        }
    }

    std::cout << m_output;



    return true;
}


