#pragma once

#include <string> // string
#include <list> // list
#include <stdexcept> // exception
#include <tuple> // tuple
#include <vector> // vector
#include <functional> // function
#include <map> // map

#include "parser.h"


extern std::map<std::string, std::function<void(Tokenizer&, std::list<rhs>&)>>
functions_map;


bool is_function(const std::string &fn);

void process_simple_function(Tokenizer &tokenizer, std::list<rhs> &res);

void augment_interpolation_1d(Tokenizer &tokenizer, std::list<rhs> &res);

void augment_searchindex(Tokenizer &tokenizer, std::list<rhs> &res);

void augment_searchalpha(Tokenizer &tokenizer, std::list<rhs> &res);

unsigned array_len_lookuptable(Tokenizer &tokenizer, std::list<rhs> &res);

void augment_lookuptable(Tokenizer &tokenizer, std::list<rhs> &res);

std::tuple<unsigned, unsigned, unsigned>
get_args_for_interpolation_2d(Tokenizer &tokenizer, std::list<rhs> &res);

void augment_interpolation_2d(Tokenizer &tokenizer, std::list<rhs> &res);


void process_function(Tokenizer &tokenizer, std::list<rhs> &res);


std::list<rhs> augment(const std::list<rhs> &input);

