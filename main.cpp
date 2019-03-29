#include <iostream> // cout
#include <string>
#include <vector>
#include <list>
#include <utility> // pair
#include <stdexcept> // exception
#include <algorithm> // count_if
#include <sstream> // ostringstream
#include <tuple>
#include <map>
#include <functional> // function

#include <cstdio> // getc
#include <cctype> // isalpha
#include <cstddef> // size_t
#include <cassert> // assert

using std::cout;
using std::endl;


#include "tinyxml2.h"
using namespace tinyxml2;

// common.h
enum class BlockType : unsigned int
{
    txt,
    tp,
    tmp,
    mem,
    upd
};

using rhs = std::pair<BlockType, std::string>;

std::string block_type_to_str(BlockType type)
{
    switch (type)
    {
        case BlockType::txt: { return "txt"; }
        case BlockType::tp:  { return "tp";  }
        case BlockType::tmp: { return "tmp"; }
        case BlockType::mem: { return "mem"; }
        case BlockType::upd: { return "upd"; }
        default: return "unknown";
    }
}

// common.h


// parser.h
enum class Token_Type
{
    function, variable, number, comma,
    plus_sign, minus_sign, multiply_sign,
    open_parenthesis, close_parenthesis,
    open_square_bracket, close_square_bracket,
    open_curly_bracket, close_curly_bracket,
    end_of_tokens,
    unknown
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
        case Token_Type::open_parenthesis: return "open_parenthesis";
        case Token_Type::close_parenthesis: return "close_parenthesis";
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
    BlockType  block_type = BlockType::txt;
    std::string value = "";
    float num = 0.0f; // @TODO: usare un optional<float> ?

    unsigned line = 1;
    unsigned col = 1;

    bool operator==(Token_Type expected) const
    {
        bool res = (expected == type);
        return res;
    }

    bool operator!=(Token_Type expected) const
    {
        bool res = (expected != type);
        return res;
    }

    rhs to_rhs()
    {
        rhs ret = std::make_pair(block_type, value);

        return ret;
    }
};

struct Tokenizer
{
    std::list<rhs> input;
    std::vector<Token> tokens;

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

    Token* current_token()
    {
        Token *token = nullptr;

        if (current_token_idx < tokens.size())
        {
            token = &tokens[current_token_idx];
        }
        else
        {
            token = &tokens.back(); // last token is always end_of_tokens
        }
        return token;
    }

    Token* next_token()
    {
        ++current_token_idx;

        Token *token = current_token();

        return token;
    }

    Token* peek_token()
    {
        ++current_token_idx;

        Token *token = current_token();

        --current_token_idx;

        return token;
    }

    Token* prev_token(unsigned back = 1)
    {
        Token *token = nullptr;

        unsigned desired_token_idx = current_token_idx - back;

        if (desired_token_idx < current_token_idx) // check for underflow
        {
            token = &tokens[desired_token_idx];
        }
        else
        {
            token = &tokens.front();
        }
        return token;
    }

    Token* require_next_token(const Token_Type &type)
    {
        Token *token = next_token();

        if (*token != type)
        {
            std::string error = "Expected " + token_type_to_str(type) + " " +
                "found " + token_type_to_str(token->type) + "  " +
                "Line:" + std::to_string(token->line) + " Col:" + std::to_string(token->col);
            throw std::exception(error.c_str());
        }

        return token;
    }

};

unsigned read_float(const std::string &input, float &num)
{
    unsigned number_len = 0;

    while (std::isdigit(input[number_len]) ||
           input[number_len] == '.')
    {
        ++number_len;
    }

    std::string num_to_parse = input.substr(0, number_len);

    num = std::stof(num_to_parse);

    return number_len;
}

std::string read_str(const std::string &input)
{
    std::ostringstream ret;

    for (char c : input)
    {
        if (isalnum(c) ||
            c == '_' ||
            c == '.')
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
// parser.h


// enhancer.h
void augment_searchindex(Tokenizer&, std::list<rhs> &);
void augment_searchalpha(Tokenizer&, std::list<rhs> &);
void augment_interpolation_1d(Tokenizer&, std::list<rhs> &);

void augment_lookuptable(Tokenizer&, std::list<rhs> &);
void augment_interpolation_2d(Tokenizer&, std::list<rhs> &);

void process_simple_function(Tokenizer&, std::list<rhs> &);


std::map<std::string, std::function<void(Tokenizer&, std::list<rhs>&)>> 
functions_map = 
{
    {"interpolation",   augment_interpolation_1d},
    {"lookuptable",     augment_lookuptable},
    {"searchindex",     augment_searchindex},
    {"searchalpha",     augment_searchalpha},
    {"interpolation2D", augment_interpolation_2d},

    {"sin",             process_simple_function},
    {"DIVISION",        process_simple_function},
    {"abs",             process_simple_function},
    {"switch",          process_simple_function},
};


bool is_function(const std::string &fn)
{
    bool res = false;

    auto it = functions_map.find(fn);

    if (it != functions_map.end())
    {
        res = true;
    }

    return res;
}



void process_function(Tokenizer&, std::list<rhs>&);

void process_simple_function(Tokenizer& tokenizer, std::list<rhs> &res)
{
    res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
    res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

    Token *current_token = tokenizer.next_token();

    while (*current_token != Token_Type::close_curly_bracket)
    {
        if (*current_token == Token_Type::function)
        {
            process_function(tokenizer, res);
        }
        else
        {
            res.push_back(current_token->to_rhs());
        }

        current_token = tokenizer.next_token();
    }

    res.push_back(current_token->to_rhs()); // }

    int stop = 0;
}


void augment_interpolation_1d(Tokenizer &tokenizer, std::list<rhs> &res)
{
    res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
    res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

    unsigned args_before_array = 2;
    for (unsigned arg = 1;
         arg <= args_before_array;
         ++arg)
    {
        if (*tokenizer.peek_token() == Token_Type::variable)
        {
            res.push_back(tokenizer.next_token()->to_rhs());
        }
        else if (*tokenizer.peek_token() == Token_Type::function)
        {
            tokenizer.next_token();
            process_function(tokenizer, res);
        }
        else
        {
            std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token()->type) +
                " Line:" + std::to_string(tokenizer.peek_token()->line) + " Col:" + std::to_string(tokenizer.peek_token()->col);
            throw std::exception(error.c_str());
        }
        
        res.push_back(tokenizer.require_next_token(Token_Type::comma)->to_rhs());
    }

    res.push_back(std::make_pair(BlockType::txt, "["));

    Token *current_token = tokenizer.next_token();

    while (*current_token != Token_Type::close_curly_bracket)
    {
        if (*current_token == Token_Type::function)
        {
            process_function(tokenizer, res);
        }
        else
        {
            res.push_back(current_token->to_rhs());
        }

        current_token = tokenizer.next_token();
    }

    res.push_back(std::make_pair(BlockType::txt, "]"));
    
    res.push_back(current_token->to_rhs()); // }
}

void augment_searchindex(Tokenizer &tokenizer, std::list<rhs> &res)
{
    res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
    res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

    if (*tokenizer.peek_token() == Token_Type::variable)
    {
        res.push_back(tokenizer.next_token()->to_rhs());
    }
    else if (*tokenizer.peek_token() == Token_Type::function)
    {
        tokenizer.next_token();
        process_function(tokenizer, res);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token()->type) +
            "  (Line:" + std::to_string(tokenizer.peek_token()->line) + " Col:" + std::to_string(tokenizer.peek_token()->col) + ")";
        throw std::exception(error.c_str());
    }

    res.push_back(tokenizer.require_next_token(Token_Type::comma)->to_rhs());

    res.push_back(std::make_pair(BlockType::txt, "["));

    Token *current_token = tokenizer.next_token();

    while (*current_token != Token_Type::close_curly_bracket)
    {
        if (*current_token == Token_Type::function)
        {
            process_function(tokenizer, res);
        }
        else
        {
            res.push_back(current_token->to_rhs());
        }

        current_token = tokenizer.next_token();
    }

    res.push_back(std::make_pair(BlockType::txt, "]"));

    res.push_back(current_token->to_rhs()); // }
}

void augment_searchalpha(Tokenizer &tokenizer, std::list<rhs> &res)
{
    res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
    res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

    if (*tokenizer.peek_token() == Token_Type::variable)
    {
        res.push_back(tokenizer.next_token()->to_rhs());
    }
    else if (*tokenizer.peek_token() == Token_Type::function)
    {
        tokenizer.next_token();
        process_function(tokenizer, res);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token()->type) +
            "  (Line:" + std::to_string(tokenizer.peek_token()->line) + " Col:" + std::to_string(tokenizer.peek_token()->col) + ")";
        throw std::exception(error.c_str());
    }

    res.push_back(tokenizer.require_next_token(Token_Type::comma)->to_rhs());

    res.push_back(std::make_pair(BlockType::txt, "["));

    Token *current_token = tokenizer.next_token();

    while (*current_token != Token_Type::close_curly_bracket)
    {
        if (*current_token == Token_Type::function)
        {
            process_function(tokenizer, res);
        }
        else
        {
            res.push_back(current_token->to_rhs());
        }

        current_token = tokenizer.next_token();
    }

    res.push_back(std::make_pair(BlockType::txt, "]"));

    res.push_back(current_token->to_rhs()); // }
}




unsigned array_len_lookuptable(Tokenizer &tokenizer, std::list<rhs> &res)
{
    tokenizer.save_state();

    unsigned array_len = 0;

    Token *current_token = tokenizer.next_token();

    while (*current_token != Token_Type::close_curly_bracket)
    {
        if (*current_token == Token_Type::function)
        {
            process_function(tokenizer, res);
            ++array_len;
        }
        else if (*current_token == Token_Type::number)
        {
            ++array_len;
        }
        else if (*current_token == Token_Type::comma)
        {
            // nothing to do here
        }
        else
        {
            // error?
            std::string error = "Something unexpected happen, found token: " + token_type_to_str(current_token->type);
            throw std::exception(error.c_str());
        }

        current_token = tokenizer.next_token();
    }

    tokenizer.restore_state();

    return array_len;
}

void augment_lookuptable(Tokenizer &tokenizer, std::list<rhs> &res)
{
    res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
    res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

    if (*tokenizer.peek_token() == Token_Type::variable)
    {
        res.push_back(tokenizer.next_token()->to_rhs());
    }
    else if (*tokenizer.peek_token() == Token_Type::function)
    {
        tokenizer.next_token();
        process_function(tokenizer, res);
    }
    else
    {
        std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token()->type) +
            " Line:" + std::to_string(tokenizer.peek_token()->line) + " Col:" + std::to_string(tokenizer.peek_token()->col);
        throw std::exception(error.c_str());
    }
    res.push_back(tokenizer.require_next_token(Token_Type::comma)->to_rhs());

    unsigned array_len = array_len_lookuptable(tokenizer, res);

    if ((array_len % 2) != 0)
    {
        throw std::exception("Expected even number of elements in array");
    }

    unsigned half_array_len = array_len / 2;
    unsigned args_counter = 1;

    res.push_back(std::make_pair(BlockType::txt, "["));
    
    Token *current_token = tokenizer.next_token();

    while (*current_token != Token_Type::close_curly_bracket)
    {
        if (*current_token == Token_Type::function)
        {
            process_function(tokenizer, res);
        }
        else if (*current_token == Token_Type::number)
        {
            res.push_back(current_token->to_rhs());

            if (args_counter == half_array_len)
            {
                res.push_back(std::make_pair(BlockType::txt, "]"));
                res.push_back(std::make_pair(BlockType::txt, ","));
                res.push_back(std::make_pair(BlockType::txt, "["));

                tokenizer.next_token(); // skip the ','
            }
            ++args_counter;
        }
        else
        {
            res.push_back(current_token->to_rhs());
        }

        current_token = tokenizer.next_token();
    }

    res.push_back(std::make_pair(BlockType::txt, "]"));

    res.push_back(current_token->to_rhs()); // }

    int stop = 0;
}




std::tuple<unsigned, unsigned, unsigned>
get_args_for_interpolation_2d(Tokenizer &tokenizer, std::list<rhs> &res)
{
    tokenizer.save_state();

    unsigned rows = 0;
    unsigned cols = 0;
    unsigned args_counter = 0;

    Token *current_token = tokenizer.next_token();

    while (*current_token != Token_Type::close_curly_bracket)
    {

        if (*current_token == Token_Type::function)
        {
            ++args_counter;
            process_function(tokenizer, res);
        }
        else if (*current_token == Token_Type::number)
        {
            ++args_counter;
        }
        else
        {
            // ','
        }

        current_token = tokenizer.next_token();
    }

    cols = (unsigned)tokenizer.prev_token(1)->num;
    rows = (unsigned)tokenizer.prev_token(3)->num;

    args_counter -= 2;

    tokenizer.restore_state();

    return std::make_tuple(args_counter, rows, cols);
}

void augment_interpolation_2d(Tokenizer &tokenizer, std::list<rhs> &res)
{
    cout << tokenizer.current_token()->value << " "; // name of the function
    cout << tokenizer.require_next_token(Token_Type::open_curly_bracket)->value << " ";

    const unsigned num_params_before_array = 4;
    for (unsigned param = 1;
         param <= num_params_before_array;
         ++param)
    {
        if (*tokenizer.peek_token() == Token_Type::variable)
        {
            cout << tokenizer.next_token()->value << " ";
        }
        else if (*tokenizer.peek_token() == Token_Type::function)
        {
            tokenizer.next_token();
            process_function(tokenizer, res);
        }
        else
        {
            std::string error = "Expected token: *variable* of *function*,  found: " + token_type_to_str(tokenizer.peek_token()->type) +
                " Line:" + std::to_string(tokenizer.peek_token()->line) + " Col:" + std::to_string(tokenizer.peek_token()->col);
            throw std::exception(error.c_str());
        }

        if (param != num_params_before_array)
        {
            cout << tokenizer.require_next_token(Token_Type::comma)->value << " ";
        }
    }

    std::tuple<unsigned, unsigned, unsigned> args_interpolation_2d = get_args_for_interpolation_2d(tokenizer, res);

    unsigned array_len, rows, cols;
    array_len = std::get<0>(args_interpolation_2d);
    rows = std::get<1>(args_interpolation_2d);
    cols = std::get<2>(args_interpolation_2d);

    if (array_len != (rows * cols))
    {
        throw std::exception("(rows * cols) does not match len of array!");
    }

    cout << ", [ [ ";
    tokenizer.next_token(); // skip the ','

    for (unsigned elem = 1;
         elem <= array_len;
         )
    {
        Token *current_token = tokenizer.next_token();

        if (*current_token == Token_Type::function)
        {
            process_function(tokenizer, res);
            ++elem;
        }
        else if (*current_token == Token_Type::number)
        {
            if (elem % cols == 0)
            {
                cout << current_token->value;
                if (elem != array_len)
                {
                    cout << "], [";
                    tokenizer.next_token(); // skip the ','
                }
            }
            else
            {
                cout << current_token->value << " ";
            }
            ++elem;
        }
        else
        {
            cout << current_token->value << " "; // ','
        }
    }

    cout << "] ]";

    tokenizer.require_next_token(Token_Type::comma); tokenizer.require_next_token(Token_Type::number); // rows token
    tokenizer.require_next_token(Token_Type::comma); tokenizer.require_next_token(Token_Type::number); // cols token

    cout << tokenizer.require_next_token(Token_Type::close_curly_bracket)->value;

    int stop = 0;
}



void process_function(Tokenizer &tokenizer, std::list<rhs> &res)
{
    Token *token = tokenizer.current_token();

    const std::string &function_name = token->value;

    auto it = functions_map.find(function_name);

    if (it != functions_map.end())
    {
        auto func = it->second;
        func(tokenizer, res);
    }
}
// enhancer.h

// parser.h
Token tokenize(const rhs &elem)
{
    std::string input = elem.second;
    
    Token token;
    
    token.block_type = elem.first;
    token.value = input;
    
    if (input == ",")
    {
        token.type = Token_Type::comma;
    }
    else if (input == "+")
    {
        token.type = Token_Type::plus_sign;
    }
    else if (input == "-")
    {
        token.type = Token_Type::minus_sign;
    }
    else if (input == "*")
    {
        token.type = Token_Type::multiply_sign;
    }
    else if (input == "(")
    {
        token.type = Token_Type::open_parenthesis;
    }
    else if (input == ")")
    {
        token.type = Token_Type::close_parenthesis;
    }
    else if (input == "{")
    {
        token.type = Token_Type::open_curly_bracket;
    }
    else if (input == "}")
    {
        token.type = Token_Type::close_curly_bracket;
    }
    else
    {
        char c = input[0];

        if (c == '-' || std::isdigit(c)) // we have negative as a single token
        {
            token.type = Token_Type::number;
            
            float num = std::stof(input);
            token.num = num;
        }
        else if (std::isalpha(c))
        {
            Token_Type type = is_function(input) ? Token_Type::function : Token_Type::variable;
            token.type = type;
        }
        else
        {
            std::string error = "Invalid token: '" + input + "'" +
                " Line:" + std::to_string(token.line) + " Col:" + std::to_string(token.col);
            throw std::exception(error.c_str());
        }
    }

    return token;
}

Tokenizer parse(const std::list<rhs> &input)
{
    Tokenizer tokenizer;
    tokenizer.input = input;

    unsigned line = 1;
    for (const auto &elem : input)
    {
        Token token = tokenize(elem);
        token.line = line++;

        tokenizer.tokens.push_back(token);
    }

    Token end;
    end.type = Token_Type::end_of_tokens;
    tokenizer.tokens.push_back(end);

    return tokenizer;
}
// parser.h

// enhancer.h
std::list<rhs>
augment(const std::list<rhs> &input)
{
    Tokenizer tokenizer = parse(input);

    std::list<rhs> res;

    Token *current_token = tokenizer.current_token();

    while (*current_token != Token_Type::end_of_tokens)
    {
        if (*current_token == Token_Type::function)
        {
            process_function(tokenizer, res);
        }
        else
        {
            rhs rhs = std::make_pair(current_token->block_type, current_token->value);
            res.push_back(rhs);
        }

        current_token = tokenizer.next_token();
    }

    return res;
}
// enhancer.h



BlockType str_to_block_type(const std::string &block_type)
{
    if (block_type == "txt") { return BlockType::txt; }
    else if (block_type == "tp") { return BlockType::tp; }
    else if (block_type == "tmp") { return BlockType::tmp; }
    else if (block_type == "mem") { return BlockType::mem; }
    else if (block_type == "upd") { return BlockType::upd; }
    else
    {
        std::string error = "Unknown BlockType: " + block_type;
        throw std::exception(error.c_str());
    }
}

std::ostream &operator<<(std::ostream &os, const std::list<rhs> &list) 
{
    for (const rhs &elem : list)
    {
        //cout << elem.second << " Type: " << block_type_to_str(elem.first) << endl;
        cout << elem.second << " ";
    }
    cout << endl;

    return os;
}

std::list<rhs> get_input_from_xml_file(const char *filename)
{
    std::list<rhs> input;

    XMLDocument doc;

    if (doc.LoadFile(filename) != XML_SUCCESS)
    {
        cout << "can't load file: '" << filename << "'" << endl;
        return input;
    }


    const XMLElement *root = doc.FirstChildElement();
    const XMLElement *block = root->FirstChildElement();

    while (block)
    {
        const char *block_type_ = block->Attribute("type");
        const char *text_ = block->GetText();

        int line_num = block->GetLineNum();

        if (!block_type_ || !text_)
        {
            std::string error = "BlockType or text not found. Line: " + std::to_string(line_num);
            throw std::exception(error.c_str());
        }

        std::string text(text_);
        BlockType block_type = str_to_block_type(block_type_);

        auto pair = std::make_pair(block_type, text);

        input.push_back(pair);

        block = block->NextSiblingElement();
    }

    return input;
}

int main()
{
    // R"delimiter( raw_characters )delimiter"	
    // std::string input_xml_str = R"FOO(raw string yo!)FOO";

    try
    {
        std::list<rhs> input = get_input_from_xml_file("test_001.xml");
        cout << "INPUT:" << endl << input << endl << endl;

        std::list<rhs> output = augment(input);
        cout << "OUTPUT:" << endl << output << endl << endl;


        std::getc(stdin);
    }
    catch (const std::exception &e)
    {
        std::cout << endl << endl << "[EXCEPTION] " << e.what() << endl;
        int stop = 0;
    }

    return 0;
}

