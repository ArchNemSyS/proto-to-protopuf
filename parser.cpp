#include "parser.h"

#include <iostream>
#include <cassert>
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


    // header
    // |enum|ProtoOAErrorCode|{|
    assert(*it == "enum");
    m_output.append("enum ");

    std::advance(it,1);
    m_output.append(*it);
    m_enums.push_back(*it);

    std::advance(it,1);
    assert(*it == "{");
    m_output.append(" {");

    std::advance(it,1);

    while(*it != "}")
    {
        // field - newline
        m_output.append(" \t");

        if( it->starts_with("//") || it->starts_with("/*") || *it == "\n")
        {
            m_output.append(*it);
            //std::advance(it,1);
        }
        else if( *std::next(it,1) == "=" )
        {
            // |ENUM_FIELD|=|1|;|// OPTIONAL COMMENT|
            m_output.append(*it);
            std::advance(it,2);
            m_output.append(" = ");
            m_output.append(*it);
            std::advance(it,1);

            assert(*it == ";");
            m_output.append(",");
            std::advance(it,1);

            while(*it != "\n")
            {
                m_output.append(*it);
                std::advance(it,1);
            }

            assert(*it == "\n");
            m_output.append("\n");

        }
        std::advance(it,1);
    }
    m_output.append("}\n\n");
    std::advance(it,1);

}

void Parser::rewrite_field_comment()
{



}

void Parser::rewrite_message_field(std::string comma)
{
    /*
    |   0     |   1    |   2     | 3 | 4 | 5 |
    |required | int64  | assetId | = | 1 | ; |
    |optional | Day    | weekday | = | 2 | ; |
    |repeated | Student| assets  | = | 3 | ; |

     --convert--

    int64_field<"assetId", 1>,
    enum_field<"weekday", 2, Day>
    message_field<"students", 3, Student, repeated>
    */


    m_output.append("\t");


    bool repeated = false;
    /*
 * | 0 |
 * required
 * optional
 * repeated
 */

    std::cout << "| 0 |" << *it << " | ";
    if(*it == "repeated")   repeated = true;
    std::advance(it,1);



    /*
 * | 1 |
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

    /*
std::map<std::string,std::string> protopuf_types{
    { "int32", "int32_field" },
    { "unit32", "uint32_field" },
    { "int64", "int64_field" },
    { "uint64", "uint64_field" },
    { "float", "float_field" },
    { "double", "double_field" },
    { "string", "string_field" },
    { "bool", "bool_field" }
};
*/

    /*
std::vector lookup_types    = {"int32",         "unit32",       "int64",        "uint64",
                                "float",        "double",       "string",       "bool"};
std::vector protopuf_types  = {"int32_field",   "uint32_field", "int64_field",  "uint64_field",
                                "float_field",  "double_field", "string_field", "bool_field"};

int field_type = -1;
for (int i = 0; i < lookup_types.size(); i++)
{
    if (lookup_types[i] == *it)
    {
        known_type = i;
    }
}
*/



    //  |   0     |   1             |   2               | 3 | 4 | 5      |
    //  |required | int64           | assetId           | = | 1 | ;      |
    //  |optional |ProtoOADayOfWeek |swapRollover3Days  | = | 6 |[default|=|MONDAY]|;|// Day of the week when SWAP charge amount will be tripled. Doesn't impact Rollover Commission.|

    std::cout << "| 1 |" << *it << " | ";
    auto known_type = std::ranges::find(protopuf_types, *it, &std::pair<std::string_view, std::string_view>::first);


    if( known_type != protopuf_types.end() )
    {
        // is proto_puf type
        m_output.append( known_type->second );
        std::cout << "| 1 |" << *it << " | ";
    }
    else if( std::find(m_enums.begin(), m_enums.end(), *it) != m_enums.end() )
    {
        // is enum_field ?
        m_output.append( "enum_field" );
    }
    else if( std::find(m_messages.begin(), m_messages.end(), *it) != m_messages.end() )
    {
        // is message_field ?
        m_output.append( "message_field" );
    }
    else
    {
        assert("unknown feild type");
    }
    std::advance(it,1);

    // | 2 |
    std::cout << "| 2 |" << *it << " | ";

    m_output.append( "<\"" );
    m_output.append( *it );
    m_output.append( "\", " );
    std::advance(it,1);

    // | 3 |
    std::cout << "| 3 |" << *it << " | ";

    assert(*it == "=");
    //m_output.append( " = " );
    std::advance(it,1);

    // | 4 |
    std::cout << "| 4 |" << *it << " | ";

    m_output.append( *it );
    std::advance(it,1);



    // repeated ?
    if(repeated)
    {
        m_output.append( ", repeated" );
    }

    // | 5 | close field
    m_output.append( ">" );
    m_output.append(comma);

    // | >5 |  comment remaining tokens

    m_output.append(" //");
    while(*it != "\n")
    {
        m_output.append( *it );
        std::advance(it,1);
    }
    m_output.append("\n");


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

    /*
    // debug
    while(*it != "}")
    {
        std::cout << "|";
        while(*it != "\n")
        {
            std::cout << *it << "|";
            std::advance(it,1);
        }
        std::advance(it,1);
        std::cout << "\n";
    }
    */

    //message ProtoOAAsset {

    m_output.append("using ");   // message
    std::advance(it,1);         // TYPE
    m_output.append(*it);
    m_messages.push_back(*it);

    m_output.append(" = message< \n");    // {
    std::advance(it,3);


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




    int n_fields = 0;
    auto next = it;
    while(*next != "}")
    {
        if(*next == ";")
        {
            n_fields++;
        }
        std::advance(next,1);
    }




    if(n_fields < 1)
    {
        //empty
        it = next;

    }
    else
    {
        int i = 1;

        while(*it != "}")
        {
            if( it->starts_with("//") || it->starts_with("/*") )
            {
                m_output.append(*it);
                m_output.append("\n");
            }
            else if( *std::next(it,3) == "=" )
            {

                if(i == n_fields)
                {
                    rewrite_message_field("");
                }
                else
                {
                    rewrite_message_field(",");
                }
                i++;
            }

        std::advance(it,1);
        }
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
                    m_tokens.emplace_back("\n");
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
        else if(*second == ';' || *second == '\n')
        {
            m_tokens.emplace_back(first, second - first);
            m_tokens.emplace_back(second, 1);

        }
        else
        {
            // split by whitespace
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




            //std::cout << "|" << *it ;
        }
    }

    std::cout << m_output;



    return true;
}


