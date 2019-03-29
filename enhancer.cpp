#pragma once

#include "enhancer.h"

#include "common.h"

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

    args_counter -= 2; // last two token are cols number and rows number

    tokenizer.restore_state();

    return std::make_tuple(args_counter, rows, cols);
}

void augment_interpolation_2d(Tokenizer &tokenizer, std::list<rhs> &res)
{
    res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
    res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

    unsigned num_params_before_array = 4;
    for (unsigned param = 1;
         param <= num_params_before_array;
         ++param)
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

    std::tuple<unsigned, unsigned, unsigned> args_interpolation_2d = get_args_for_interpolation_2d(tokenizer, res);

    unsigned array_len, rows, cols;
    array_len = std::get<0>(args_interpolation_2d);
    rows = std::get<1>(args_interpolation_2d);
    cols = std::get<2>(args_interpolation_2d);

    if (array_len != (rows * cols))
    {
        throw std::exception("(rows * cols) does not match len of array!");
    }

    res.push_back(std::make_pair(BlockType::txt, "["));
    res.push_back(std::make_pair(BlockType::txt, "["));

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
                res.push_back(current_token->to_rhs());
                if (elem != array_len)
                {
                    res.push_back(std::make_pair(BlockType::txt, "]"));
                    res.push_back(tokenizer.require_next_token(Token_Type::comma)->to_rhs()); // ,
                    res.push_back(std::make_pair(BlockType::txt, "["));
                }
            }
            else
            {
                res.push_back(tokenizer.current_token()->to_rhs());
            }
            ++elem;
        }
        else
        {
            res.push_back(tokenizer.current_token()->to_rhs());
        }
    }

    res.push_back(std::make_pair(BlockType::txt, "]"));
    res.push_back(std::make_pair(BlockType::txt, "]"));

    tokenizer.require_next_token(Token_Type::comma); tokenizer.require_next_token(Token_Type::number); // rows token
    tokenizer.require_next_token(Token_Type::comma); tokenizer.require_next_token(Token_Type::number); // cols token

    res.push_back(tokenizer.require_next_token(Token_Type::close_curly_bracket)->to_rhs());

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

