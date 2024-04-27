#include "parser.h"

#include <iostream>
#include <algorithm>


Parser::Parser(){}
void Parser::rewrite_comment()
{
    m_output.append(*it);
    m_output.append("\n");
}

void Parser::rewrite_include()
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

void Parser::rewrite_enum()
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
    m_enums.push_back(*it);

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

void Parser::rewrite_message_field()
{
    /*
    |   0     |   1   |   2     | 3 | 4 | 5 |
    |required | int64 | assetId | = | 1 | ; |
    repeated Student assets = 3;

     --convert--

    int64_field<"assetId", 1>,
    message_field<"students", 3, Student, repeated>


    fields
    int32   = int32_field
    unit32  = uint32_field
    int64   = int64_field
    uint64  = uint64_field

    float   = float_field
    double  = double_field

    string  = string_field

    bool    = bool_field
    unkown_type = lookup enum_field AND message_field

    */

    bool repeated = false;
    int pos = 0;

    switch (pos) {
    case 0:
        // repeat
        if(*it == "repeated") repeated = true;

        break;
    case 1:
        // type

        break;
    case 3:
        // wireid

        break;
    }
    pos++;

    /*
    else if(*it == ";")
    {
        if( std::next(it)->starts_with("//") || std::next(it)->starts_with("/*") )
        {
            m_output.append(">,");
            std::advance(it,1);
            m_output.append(*it);
            m_output.append("\n\t<");
        }
        else
        {
            m_output.append(">,");
            m_output.append("\n\t<");
        }

        // reset field flags
        repeated = false;
    }
    */

}

void Parser::rewrite_message()
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
    m_messages.push_back(*it);

    m_output.append(" = message< \n\t<");    // {
    std::advance(it,2);


    //constexpr std::string_view close("}");




     //mark feilds ;
    /*
    std::vector< std::vector<std::string_view>::iterator > fields;
    auto start = it;

    while(*it != "}")
    {
        if(*it == ";")
        {

            fields.push_back(it);

        }
        std::advance(it,1);
    }
    // it = }
    */

    /*
    for (auto feild_end = fields.begin(); feild_end != std::prev(fields.end()); ++feild_end)
    {
        rewrite_message_field(start, *feild_end);
    }
    std::cout << "last\n";
    rewrite_message_field(start, *fields.end());
    */








    while(*it != "}")
    {


        if( it->starts_with("//") || it->starts_with("/*") )
        {
            m_output.append(*it);
            m_output.append("\n\t");
        }
        else
        {
            // don't iterate do the line
            // in feild line
            rewrite_message_field();





        }


    std::advance(it,1);
    }


    m_output.append(">;\n\n");

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
    m_output.clear();
    m_output.reserve(source.size());
    m_source = source;

    m_enums.clear();
    m_messages.clear();
    m_tokens.clear();

    if(tokenize())
    {
        for (it = m_tokens.begin(); it != m_tokens.end(); ++it) {

            // current index
            //auto i = std::distance(m_tokens.begin(), it);
            //std::cout << i << "\n";

            if      (it->starts_with("//") || it->starts_with("/*"))     rewrite_comment();
            else if (it->starts_with("import"))                          rewrite_include();
            else if (it->starts_with("enum"))                            rewrite_enum();
            else if (it->starts_with("message"))                         rewrite_message();
        }
    }

    std::cout << m_output;



    return true;
}


