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

using std::cout;
using std::endl;


#include "tinyxml2.h"
using namespace tinyxml2;

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
    std::string value = "";
    float num = 0.0f;

    unsigned row = 0;
    unsigned col = 0;
};

struct Tokenizer
{
    std::string input = "";
    std::vector<Token> tokens;
    unsigned current_token_idx = 0;
    unsigned old_token_idx = 0;

    void save_state()
    {
        old_token_idx = current_token_idx;
    }

    void restore_state()
    {
        current_token_idx = old_token_idx;
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
            std::string error = "Expected token: *" + token_type_to_str(type) + "*" +
                                "  found: *" + token_type_to_str(token.type) + "*";
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

    for (unsigned pos = 0;
         pos < input.size();
         )
    {
        Token token;
        char c = input[pos];

        switch (c)
        {
            case ',':
            {
                token.type = Token_Type::comma;
                token.value = ",";
                token.col = pos + 1;

                ++pos;
            } break;

            case '{':
            {
                token.type = Token_Type::open_curly_bracket;
                token.value = "{";
                token.col = pos + 1;

                ++pos;
            } break;

            case '}':
            {
                token.type = Token_Type::close_curly_bracket;
                token.value = "}";
                token.col = pos + 1;

                ++pos;
            } break;

            case '-':
            {
                token.type = Token_Type::minus_sign;
                token.value = "-";
                token.col = pos + 1;

                ++pos;
            } break;

            case '+':
            {
                token.type = Token_Type::plus_sign;
                token.value = "+";
                token.col = pos + 1;

                ++pos;
            } break;

            default:
            {
                if (std::isdigit(c))
                {
                    float num = 0;
                    unsigned number_len = read_number(input.substr(pos), num);

                    token.type = Token_Type::number;
                    token.value = input.substr(pos, number_len);
                    token.num = num;
                    token.col = pos + 1;

                    Token &last_token = tokenizer.tokens.back();
                    if (last_token.type == Token_Type::minus_sign)
                    {
                        token.value = "-" + token.value;
                        token.num = -token.num;

                        tokenizer.tokens.pop_back();
                    }

                    pos += number_len;
                }
                else if (std::isalpha(c))
                {
                    std::string str = read_str(input.substr(pos));

                    if (is_function(str))
                    {
                        token.type = Token_Type::function;
                        token.value = str;
                        token.col = pos + 1;
                    }
                    else
                    {
                        token.type = Token_Type::variable;
                        token.value = str;
                        token.col = pos + 1;
                    }

                    pos += (unsigned)str.length();
                }
                else if (std::isspace(c))
                {
                    ++pos;
                    continue;
                }
                else
                {
                    std::string error = "Invalid char: '";
                    error.push_back(c);
                    error += "'";
                    throw std::exception(error.c_str());
                }
            }
        }

        tokenizer.tokens.push_back(token);
    }

    Token end;
    end.type = Token_Type::end_of_tokens;
    tokenizer.tokens.push_back(end);

    return tokenizer;
}

void parse_lookuptable(Tokenizer &tokenizer, std::ostringstream &ss)
{
    cout << tokenizer.current_token().value << " ";
    cout << tokenizer.require_token(Token_Type::open_curly_bracket).value << " ";

    if (tokenizer.peek_token().type == Token_Type::variable)
    {
        cout << tokenizer.next_token().value << " ";
    }
    else if (tokenizer.peek_token().type == Token_Type::function)
    {
        tokenizer.next_token();
        parse_lookuptable(tokenizer, ss);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token().type);
        throw std::exception(error.c_str());
    }

    cout << tokenizer.require_token(Token_Type::comma).value;
    tokenizer.require_token(Token_Type::number);

    tokenizer.save_state();

    unsigned values = 0;
    do
    {
        const Token &token = tokenizer.current_token();

        if (token.type == Token_Type::number)
        {
            //cout << "token: " << token.value << endl;
            ++values;
        }
    } while (tokenizer.next_token().type != Token_Type::close_curly_bracket);

    tokenizer.restore_state();

    if ((values % 2) != 0)
    {
        throw std::exception("Expected even number of elements in array");
    }
    unsigned cols = values / 2;
    unsigned col = 1;
    
    cout << " [";
    do
    {
        const Token &token = tokenizer.current_token();

        if (token.type == Token_Type::number)
        {
            if (col == cols)
            {
                cout << std::fixed << token.num << "], [";
                tokenizer.next_token(); // skip the next ','
            }
            else
            {
                cout << std::fixed << token.num;
            }
            ++col;
        }
        else
        {
            cout << token.value << " ";
        }
    } while (tokenizer.next_token().type != Token_Type::close_curly_bracket);
    cout << "]";
    cout << tokenizer.current_token().value;

    //cout << ss.str() << endl;
    int stop = 0;
}

void parse(const std::string &input)
{
    Tokenizer tokenizer = tokenize(input);

    do
    {
        const Token &token = tokenizer.current_token();

        if (token.type == Token_Type::function)
        {
            if (token.value == "lookuptable")
            {
                std::ostringstream ss;
                parse_lookuptable(tokenizer, ss);
            }
        }
    } while (tokenizer.next_token().type != Token_Type::end_of_tokens);


#if 0
    cout << "Before:" << endl << "  " << input << endl;

    tokenizer tokenizer = tokenize(input);

    cout << "After:" << endl << "  ";
    for (token &token : tokenizer.tokens)
    {
        if (token.type == token_type::unknown)
        {
            std::string error = "unknown token found in pos: " + std::to_string(token.col) + " value: " + token.value;
            throw std::exception(error.c_str());
        }

        if (token.type == token_type::number)
            cout << token.num << " ";
        else
            cout << token.value << " ";
    }

    int count_fn = std::count_if(tokenizer.tokens.begin(), tokenizer.tokens.end(), [](const token &token) {
        return (token.type == token_type::function);
    });
    int count_var = std::count_if(tokenizer.tokens.begin(), tokenizer.tokens.end(), [](const token &token) {
        return (token.type == token_type::variable);
    });

    cout << endl << "function/s found: " << count_fn << " var found: " << count_var;

    cout << endl << endl;
#endif

}

int main(int argc, char* argv[])
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

        test_input = "lookuptable { lookuptable { lookuptable { CLW07_FLAPS, -10.0, 20.0}, -10.0, 20.0}, 30.0, -40.0}";
        parse(test_input);

#if 0
        test_input = ss.str();
        parse(test_input);

        test_input = "interpolation { BF_CLC03_GOVLIM_IDX03_INDEX, BF_CLC03_GOVLIM_IDX03_ALPHA, 7.5000000000, 16.0000000000, 25.0000000000, 40.5000000000, 35.5000000000}";
        parse(test_input);

        test_input = "searchindex{BF_CLA02_NZAOA_S03,-0.2000000000, 0.5000000000, 0.7120000000, 1.1120000000, 1.1200000000, 1.2120000000, 1.5000000000, 1.6270000000}";
        parse(test_input);

        test_input = "	searchalpha{MACH_SEL,0.2000000000, 0.3000000000, 0.4000000000, 0.5000000000}";
        parse(test_input);

        test_input = "interpolation2D{	searchindex{		BF_CLA02_NZAOA_S03 ,		-0.2000000000 , 0.5000000000 , 0.7120000000 , 1.1120000000 , 1.1200000000 , 1.2120000000 , 1.5000000000 ,1.6270000000	} ,	searchalpha{		BF_CLA02_NZAOA_S03 ,		-0.2000000000 , 0.5000000000 , 0.7120000000 , 1.1120000000 , 1.1200000000 , 1.2120000000 , 1.5000000000 , 1.6270000000	} ,	searchindex{		MACH_SEL ,		0.2000000000 , 0.3000000000 , 0.4000000000 , 0.5000000000	} ,	searchalpha{		MACH_SEL ,		0.2000000000 , 0.3000000000 , 0.4000000000 , 0.5000000000	} ,	-6.1300000000 , -6.1300000000 , -6.1340000000 , -6.1340000000 , 0.8750000000 , 0.8750000000 , 0.8767000000 , 1.0000000000 , 3.0000000000 , 3.0000000000 , 3.0000000000 , 1.0000000000 , 7.0000000000 , 7.0000000000 , 8.8800000000 , 1.0000000000 , 7.0800000000 , 7.1600000000 , 9.0000000000 , 1.0000000000 , 8.0000000000 , 9.0000000000 , 9.0000000000 , 1.0000000000 , 14.2450000000 , 15.0000000000 , 9.0000000000 , 1.0000000000 , 17.0000000000 , 15.0000000000 , 9.0000000000 , 1.0000000000 ,	8.0 ,	4.0}";
        parse(test_input);
#endif // 0

    } catch (const std::exception &e)
    {
        std::cout << "[EXCEPTION] " << e.what() << endl;
        int stop = 0;
    }

    return 0;
}