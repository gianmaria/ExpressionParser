#if 0

interpolation{
    BF_CLC03_GOVLIM_IDX03_INDEX ,
    BF_CLC03_GOVLIM_IDX03_ALPHA ,
    7.5000000000 , 16.0000000000 , 25.0000000000 , 40.5000000000 , 35.5000000000
}

lookuptable{
    CLW07_FLAPS,
    0.0000000000, 5.0000000000, 10.0000000000, 15.0000000000, 20.0000000000, 25.0000000000, 30.0000000000, 35.0000000000, 40.0000000000, 0.0000000000, 0.1000000000, 0.2000000000, 0.3150000000, 0.4163000000, 0.4801000000, 0.5438000000, 0.6076000000, 0.6714000000
}

interpolation2D{
    searchindex{
        BF_CLA02_NZAOA_S03 ,
        -0.2000000000 , 0.5000000000 , 0.7120000000 , 1.1120000000 , 1.1200000000 , 1.2120000000 , 1.5000000000 , 1.6270000000
    } ,
    searchalpha{
        BF_CLA02_NZAOA_S03 ,
        -0.2000000000 , 0.5000000000 , 0.7120000000 , 1.1120000000 , 1.1200000000 , 1.2120000000 , 1.5000000000 , 1.6270000000
    } ,
    searchindex{
        MACH_SEL ,
        0.2000000000 , 0.3000000000 , 0.4000000000 , 0.5000000000
    } ,
    searchalpha{
        MACH_SEL ,
        0.2000000000 , 0.3000000000 , 0.4000000000 , 0.5000000000
    } ,
    -6.1300000000 , -6.1300000000 , -6.1340000000 , -6.1340000000 , 0.8750000000 , 0.8750000000 , 0.8767000000 , 1.0000000000 , 3.0000000000 , 3.0000000000 , 3.0000000000 , 1.0000000000 , 7.0000000000 , 7.0000000000 , 8.8800000000 , 1.0000000000 , 7.0800000000 , 7.1600000000 , 9.0000000000 , 1.0000000000 , 8.0000000000 , 9.0000000000 , 9.0000000000 , 1.0000000000 , 14.2450000000 , 15.0000000000 , 9.0000000000 , 1.0000000000 , 17.0000000000 , 15.0000000000 , 9.0000000000 , 1.0000000000 ,
    8.0 ,
    4.0
}

searchindex{
    BF_CLA02_NZAOA_S03 ,
    -0.2000000000 , 0.5000000000 , 0.7120000000 , 1.1120000000 , 1.1200000000 , 1.2120000000 , 1.5000000000 , 1.6270000000
}

searchalpha{
    MACH_SEL ,
    0.2000000000 , 0.3000000000 , 0.4000000000 , 0.5000000000
}

#endif

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <cctype> // isalpha
#include <cassert> // assert
#include <cstddef> // size_t
#include <utility> // pair
#include <stdexcept> // exception
#include <algorithm> // count_if
#include <sstream> // ostringstream

#include <cstdio> // getc

using std::cout;
using std::endl;


#include "tinyxml2.h"
using namespace tinyxml2;

enum class BlockType : unsigned int
{
    txt,
    tp,
    tmp,
    mem,
    upd
};

enum class Token_Type
{
    function, variable, number, comma,
    plus_sign, minus_sign,
    open_round_bracket, close_round_bracket,
    open_square_bracket, close_square_bracket,
    open_curly_bracket, close_curly_bracket,
    end_of_tokens, unknown
};

std::string token_type_to_str(const Token_Type &type)
{
    switch (type)
    {
        case Token_Type::function: return "function";
        case Token_Type::variable: return "variable";
        case Token_Type::number: return "number";
        case Token_Type::comma: return "comma";
        case Token_Type::plus_sign: return "plus_sign";
        case Token_Type::minus_sign: return "minus_sign";
        case Token_Type::open_round_bracket: return "open_round_bracket";
        case Token_Type::close_round_bracket: return "close_round_bracket";
        case Token_Type::open_square_bracket: return "open_square_bracket";
        case Token_Type::close_square_bracket: return "close_square_bracket";
        case Token_Type::open_curly_bracket: return "open_curly_bracket";
        case Token_Type::close_curly_bracket: return "close_curly_bracket";
        case Token_Type::end_of_tokens: return "end_of_tokens";
        case Token_Type::unknown: return "unknown";
        default: throw std::exception("Unknown token type!");
    }
}

struct Token
{
    Token_Type type = Token_Type::unknown;
    //BlockType  xml_type = BlockType::txt;
    std::string value = "";
    float num = 0.0f;

    unsigned line = 1;
    unsigned col = 1;
};

struct Tokenizer
{
    std::string input = "";
    std::vector<Token> tokens;
    unsigned line = 1;
    unsigned col = 1;

    unsigned current_token_idx = 0;
    unsigned old_token_idx = 0;

    void save_state()
    {
        if (old_token_idx != 0)
        {
            throw std::exception("Only one save at a time!");
        }

        old_token_idx = current_token_idx;
    }

    void restore_state()
    {
        current_token_idx = old_token_idx;
        old_token_idx = 0;
    }

    Token& current_token()
    {
        if (current_token_idx < tokens.size())
        {
            Token &token = tokens[current_token_idx];
            return token;
        }
        else
        {
            Token &token = tokens.back(); // last token is always Token_Type::end_of_tokens
            return token;
        }
    }

    Token& next_token()
    {
        ++current_token_idx;

        Token &token = current_token();

        return token;
    }

    Token& peek_token()
    {
        ++current_token_idx;

        Token &token = current_token();

        --current_token_idx;

        return token;
    }

    Token& prev_token()
    {
        if (current_token_idx > 0)
        {
            Token &token = tokens[current_token_idx - 1];

            return token;
        }
        else
        {
            Token &token = tokens.front();

            return token;
        }
    }

    Token& require_token(const Token_Type &type)
    {
        Token &token = next_token();

        if (token.type != type)
        {
            std::string error = "Expected token: *" + token_type_to_str(type) + "*   " +
                "found: *" + token_type_to_str(token.type) + "*  " +
                "Line:" + std::to_string(token.line) + " Col:" + std::to_string(token.col);
            throw std::exception(error.c_str());
        }

        return token;
    }

};

unsigned read_number(const std::string &input, float &num)
{
    unsigned pos_end_of_num = 0;

    while (std::isdigit(input[pos_end_of_num]) ||
           input[pos_end_of_num] == '.')
    {
        ++pos_end_of_num;
    }

    std::string num_to_parse = input.substr(0, pos_end_of_num);

    num = std::stof(num_to_parse);

    return pos_end_of_num;
}

std::string read_str(const std::string &input)
{
    std::ostringstream ret;

    for (char c : input)
    {
        if (isalnum(c) ||
            c == '_')
        {
            ret << c;
        }
        else
        {
            break;
        }
    }

    return ret.str();
}

std::list<std::string> function_list = { "lookuptable", "interpolation", "searchindex", "searchalpha", "interpolation2D" };

bool is_function(const std::string &fn)
{
    auto res = std::find(function_list.begin(), function_list.end(), fn);

    return (res != std::end(function_list));

    return true;
}


Tokenizer tokenize(const std::string &input)
{
    Tokenizer tokenizer;
    tokenizer.input = input;

    unsigned index = 0;
    while (index < input.size())
    {
        Token token;
        token.line = tokenizer.line;
        token.col = tokenizer.col;

        char c = input[index];

        switch (c)
        {
            case ',':
            {
                token.type = Token_Type::comma;
                token.value = ",";
            } break;

            case '{':
            {
                token.type = Token_Type::open_curly_bracket;
                token.value = "{";
            } break;

            case '}':
            {
                token.type = Token_Type::close_curly_bracket;
                token.value = "}";
            } break;

            case '-': // @TODO: parsare i numeri negativi direttamente qui?
            {
                token.type = Token_Type::minus_sign;
                token.value = "-";
            } break;

            case '+':
            {
                token.type = Token_Type::plus_sign;
                token.value = "+";
            } break;

            case ' ':
            {
                tokenizer.col += 1;
                index += 1;
                continue;
            } break;

            case '\n':
            {
                tokenizer.col = 1;
                tokenizer.line += 1;
                index += 1;
                continue;
            } break;

            default:
            {
                if (std::isdigit(c))
                {
                    float num = 0;
                    unsigned number_len = read_number(input.substr(index), num);

                    token.type = Token_Type::number;
                    token.value = input.substr(index, number_len);
                    token.num = num;

                    tokenizer.col += (unsigned)token.value.length();
                    index += (unsigned)token.value.length();

                    Token &last_token = tokenizer.tokens.back();
                    if (last_token.type == Token_Type::minus_sign)
                    {
                        token.value = "-" + token.value; // @TODO: lasciare il numero senza il segno
                        token.num = -token.num;

                        tokenizer.tokens.pop_back();
                    }

                    tokenizer.tokens.push_back(token);
                    continue;
                }
                else if (std::isalpha(c))
                {
                    std::string str = read_str(input.substr(index));

                    token.type = is_function(str) ? Token_Type::function : Token_Type::variable;
                    token.value = str;
                }
                else
                {
                    std::string error = "Invalid char: '"; error.push_back(c); error += "'";
                    throw std::exception(error.c_str());
                }
            }
        }

        tokenizer.col += (unsigned)token.value.length();
        index += (unsigned)token.value.length();

        tokenizer.tokens.push_back(token);
    }

    Token end;
    end.type = Token_Type::end_of_tokens;
    tokenizer.tokens.push_back(end);

    return tokenizer;
}



unsigned array_len_lookuptable(Tokenizer &tokenizer)
{
    tokenizer.save_state();

    unsigned array_len = 0;

    do
    {
        tokenizer.require_token(Token_Type::comma);

        tokenizer.require_token(Token_Type::number);

        ++array_len;

    } while (tokenizer.peek_token().type != Token_Type::close_curly_bracket);

    tokenizer.restore_state();

    return array_len;
}

void parse_function(Tokenizer &tokenizer);

void parse_lookuptable(Tokenizer &tokenizer)
{
    cout << tokenizer.current_token().value << " "; // the name of the function
    cout << tokenizer.require_token(Token_Type::open_curly_bracket).value << " ";

    if (tokenizer.peek_token().type == Token_Type::variable)
    {
        cout << tokenizer.next_token().value << " ";
    }
    else if (tokenizer.peek_token().type == Token_Type::function)
    {
        tokenizer.next_token();
        parse_function(tokenizer);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token().type) +
            " Line:" + std::to_string(tokenizer.peek_token().line) + " Col:" + std::to_string(tokenizer.peek_token().col);
        throw std::exception(error.c_str());
    }
    cout << ", ";

    unsigned array_len = array_len_lookuptable(tokenizer);

    if ((array_len % 2) != 0)
    {
        throw std::exception("Expected even number of elements in array");
    }

    unsigned half_array_len = array_len / 2;
    unsigned args_counter = 1;

    cout << " [";
    do
    {
        tokenizer.require_token(Token_Type::comma);

        Token &token = tokenizer.require_token(Token_Type::number);

        if (args_counter == half_array_len)
        {
            cout << std::fixed << token.num << "], [";
        }
        else
        {
            cout << std::fixed << token.num << ", ";
        }

        ++args_counter;

    } while (tokenizer.peek_token().type != Token_Type::close_curly_bracket);
    cout << "]";

    cout << tokenizer.require_token(Token_Type::close_curly_bracket).value;

    //cout << ss.str() << endl;
    int stop = 0;
}


void parse_interpolation_1d(Tokenizer &tokenizer)
{
    cout << tokenizer.current_token().value << " "; // the name of the function
    cout << tokenizer.require_token(Token_Type::open_curly_bracket).value << " ";

    if (tokenizer.peek_token().type == Token_Type::variable)
    {
        cout << tokenizer.next_token().value << " ";
    }
    else if (tokenizer.peek_token().type == Token_Type::function)
    {
        tokenizer.next_token();
        parse_function(tokenizer);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token().type) +
            " Line:" + std::to_string(tokenizer.peek_token().line) + " Col:" + std::to_string(tokenizer.peek_token().col);
        throw std::exception(error.c_str());
    }

    cout << tokenizer.require_token(Token_Type::comma).value << " ";

    if (tokenizer.peek_token().type == Token_Type::variable)
    {
        cout << tokenizer.next_token().value << " ";
    }
    else if (tokenizer.peek_token().type == Token_Type::function)
    {
        tokenizer.next_token();
        parse_function(tokenizer);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token().type) +
            " Line:" + std::to_string(tokenizer.peek_token().line) + " Col:" + std::to_string(tokenizer.peek_token().col);
        throw std::exception(error.c_str());
    }

    cout << ", ";

    cout << " [";
    do
    {
        tokenizer.require_token(Token_Type::comma);

        Token &token = tokenizer.require_token(Token_Type::number);

        cout << std::fixed << token.num << ", ";

    } while (tokenizer.peek_token().type != Token_Type::close_curly_bracket);
    cout << "]";

    cout << tokenizer.require_token(Token_Type::close_curly_bracket).value;

    int stop = 0;
}

void parse_searchindex(Tokenizer &tokenizer)
{
    cout << tokenizer.current_token().value << " "; // the name of the function
    cout << tokenizer.require_token(Token_Type::open_curly_bracket).value << " ";

    if (tokenizer.peek_token().type == Token_Type::variable)
    {
        cout << tokenizer.next_token().value << " ";
    }
    else if (tokenizer.peek_token().type == Token_Type::function)
    {
        tokenizer.next_token();
        parse_function(tokenizer);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token().type) +
            " Line:" + std::to_string(tokenizer.peek_token().line) + " Col:" + std::to_string(tokenizer.peek_token().col);
        throw std::exception(error.c_str());
    }

    cout << ", ";

    cout << " [";
    do
    {
        tokenizer.require_token(Token_Type::comma);

        Token &token = tokenizer.require_token(Token_Type::number);

        cout << std::fixed << token.num << ", ";

    } while (tokenizer.peek_token().type != Token_Type::close_curly_bracket);
    cout << "]";

    cout << tokenizer.require_token(Token_Type::close_curly_bracket).value;

    int stop = 0;
}

void parse_searchalpha(Tokenizer &tokenizer)
{
    cout << tokenizer.current_token().value << " "; // the name of the function
    cout << tokenizer.require_token(Token_Type::open_curly_bracket).value << " ";

    if (tokenizer.peek_token().type == Token_Type::variable)
    {
        cout << tokenizer.next_token().value << " ";
    }
    else if (tokenizer.peek_token().type == Token_Type::function)
    {
        tokenizer.next_token();
        parse_function(tokenizer);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token().type) +
            " Line:" + std::to_string(tokenizer.peek_token().line) + " Col:" + std::to_string(tokenizer.peek_token().col);
        throw std::exception(error.c_str());
    }

    cout << ", ";

    cout << " [";
    do
    {
        tokenizer.require_token(Token_Type::comma);

        Token &token = tokenizer.require_token(Token_Type::number);

        cout << std::fixed << token.num << ", ";

    } while (tokenizer.peek_token().type != Token_Type::close_curly_bracket);
    cout << "]";

    cout << tokenizer.require_token(Token_Type::close_curly_bracket).value;

    int stop = 0;
}

void parse_function(Tokenizer &tokenizer)
{
    const Token &token = tokenizer.current_token();

    if (token.value == "lookuptable")
    {
        parse_lookuptable(tokenizer);
    }
    else if (token.value == "interpolation")
    {
        parse_interpolation_1d(tokenizer);
    }
    else if (token.value == "searchindex")
    {
        parse_searchindex(tokenizer);
    }
    else if (token.value == "searchalpha")
    {
        parse_searchalpha(tokenizer);
    }

}

void parse(const std::string &input)
{
    Tokenizer tokenizer = tokenize(input);

    std::ostringstream ss;

    do
    {
        const Token &token = tokenizer.current_token();

        if (token.type == Token_Type::function)
        {
            parse_function(tokenizer);
        }

    } while (tokenizer.next_token().type != Token_Type::end_of_tokens);

}


int main()
{
    //  R "delimiter( raw_characters )delimiter"	
    //std::string input_xml_str = R"FOO(raw string yo!)FOO";

    std::ostringstream ss;

    const char *filename = "test_001.xml";

    XMLDocument doc;

    if (doc.LoadFile(filename) != XML_SUCCESS)
    {
        cout << "impossible to open file: '" << filename << "'" << endl;
    }

    const XMLElement *root = doc.FirstChildElement();
    const XMLElement *block = root->FirstChildElement();
    do
    {
        const char *text = block->GetText();

        if (text)
            ss << block->GetText() << " ";

        block = block->NextSiblingElement();
    } while (block);

    try
    {
        std::string test_input;

        test_input = "interpolation{ var1, var2, 1.0, 2.0, -3.0, -4.0, 5.0 }";
        test_input = "interpolation{ interpolation{ var1, var2, 1.0, -45.9}, var3, 1.0, 2.0, -3.0, -4.0, 5.0 }";
        test_input = "interpolation{ var3, interpolation{ var1, var2, 1.0, -45.9}, 1.0, 2.0, -3.0, -4.0, 5.0 }";
        test_input = "interpolation{ interpolation{ var1, var2, 3.14, -45.9}, interpolation{ var3, var4, 7.0, -88.9}, 1.0, -3.0, -4.0, 5.0 }";
        test_input = "interpolation { BF_CLC03_GOVLIM_IDX03_INDEX, BF_CLC03_GOVLIM_IDX03_ALPHA, 7.5000000000, 16.0000000000, 25.0000000000, 40.5000000000, 35.5000000000}";
        test_input = "lookuptable { interpolation { var1, var2, 1.0, 2.0, -3.0, -4.0, 5.0 }, -10.0, 20.0}";
        test_input = "searchindex{ var4, 4, 5, -6}";
        test_input = "searchindex{ searchindex{ var1, 1}, 4, 5, -6}";
        test_input = R"FOO(
searchindex {
    interpolation {
        interpolation { var1, var2, 3.14, -45.9},
        lookuptable {
            interpolation { var1, var2, 1.0, 2.0, -3.0, -4.0, 5.0 },
            -10.0, 20.0},
        1.0, -3.0, -4.0, 5.0 },
    -0.2000000000, 0.5000000000, 0.7120000000, 1.1120000000, 1.1200000000, 1.2120000000, 1.5000000000, 1.6270000000}
)FOO";

        test_input = R"FOO(
searchalpha{ 
    searchindex {
        interpolation {
            interpolation { var1, var2, 3.14, -45.9},
            lookuptable {
                interpolation { var1, var2, 1.0, 2.0, -3.0, -4.0, 5.0 },
                -10.0, 20.0},
            1.0, -3.0, -4.0, 5.0 },
        -0.2000000000, 0.5000000000, 0.7120000000, 1.1120000000, 1.1200000000, 1.2120000000, 1.5000000000, 1.6270000000}, 4, 5, -6}
)FOO";

        cout << "INPUT: " << endl << test_input << endl << endl;
        parse(test_input);
        cout << endl << endl;

#if 0
        test_input = "lookuptable { lookuptable { lookuptable { CLW07_FLAPS, -10.0, 20.0}, -10.0, 20.0}, 30.0, -40.0}";
        parse(test_input);
        cout << endl << endl;

        test_input = ss.str();
        parse(test_input);
        cout << endl << endl;

        test_input = "interpolation { BF_CLC03_GOVLIM_IDX03_INDEX, BF_CLC03_GOVLIM_IDX03_ALPHA, 7.5000000000, 16.0000000000, 25.0000000000, 40.5000000000, 35.5000000000}";
        parse(test_input);

        test_input = "searchindex{BF_CLA02_NZAOA_S03,-0.2000000000, 0.5000000000, 0.7120000000, 1.1120000000, 1.1200000000, 1.2120000000, 1.5000000000, 1.6270000000}";
        parse(test_input);

        test_input = "	searchalpha{MACH_SEL,0.2000000000, 0.3000000000, 0.4000000000, 0.5000000000}";
        parse(test_input);

        test_input = "interpolation2D{	searchindex{		BF_CLA02_NZAOA_S03 ,		-0.2000000000 , 0.5000000000 , 0.7120000000 , 1.1120000000 , 1.1200000000 , 1.2120000000 , 1.5000000000 ,1.6270000000	} ,	searchalpha{		BF_CLA02_NZAOA_S03 ,		-0.2000000000 , 0.5000000000 , 0.7120000000 , 1.1120000000 , 1.1200000000 , 1.2120000000 , 1.5000000000 , 1.6270000000	} ,	searchindex{		MACH_SEL ,		0.2000000000 , 0.3000000000 , 0.4000000000 , 0.5000000000	} ,	searchalpha{		MACH_SEL ,		0.2000000000 , 0.3000000000 , 0.4000000000 , 0.5000000000	} ,	-6.1300000000 , -6.1300000000 , -6.1340000000 , -6.1340000000 , 0.8750000000 , 0.8750000000 , 0.8767000000 , 1.0000000000 , 3.0000000000 , 3.0000000000 , 3.0000000000 , 1.0000000000 , 7.0000000000 , 7.0000000000 , 8.8800000000 , 1.0000000000 , 7.0800000000 , 7.1600000000 , 9.0000000000 , 1.0000000000 , 8.0000000000 , 9.0000000000 , 9.0000000000 , 1.0000000000 , 14.2450000000 , 15.0000000000 , 9.0000000000 , 1.0000000000 , 17.0000000000 , 15.0000000000 , 9.0000000000 , 1.0000000000 ,	8.0 ,	4.0}";
        parse(test_input);
#endif // 0

    }
    catch (const std::exception &e)
    {
        std::cout << endl << endl << "[EXCEPTION] " << e.what() << endl;
        int stop = 0;
    }

    std::getc(stdin);

    return 0;
}