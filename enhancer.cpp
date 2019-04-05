#include "enhancer.h"

#include "common.h"

std::map<std::string, std::function<void(Tokenizer&, std::list<rhs_instr>&)>>
functions_map =
{
{"interpolation",   augment_interpolation_1d},
{"lookuptable",     augment_lookuptable},
{"searchindex",     augment_searchindex},
{"searchalpha",     augment_searchalpha},
{"interpolation2D", augment_interpolation_2d},

{"div", process_simple_function},
{"power", process_simple_function},
{"exponent", process_simple_function},
{"minimum", process_simple_function},
{"maximum", process_simple_function},
{"abs", process_simple_function},
{"atan2", process_simple_function},
{"cos", process_simple_function},
{"sin", process_simple_function},
{"acos", process_simple_function},
{"asin", process_simple_function},
{"sqrt", process_simple_function},
{"log", process_simple_function},
{"exp", process_simple_function},
{"tan", process_simple_function},
{"atan", process_simple_function},
{"sign", process_simple_function},
{"round", process_simple_function},
{"fix", process_simple_function},
{"ceil", process_simple_function},
{"floor", process_simple_function},
{"limiter", process_simple_function},
{"switch", process_simple_function}
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

void process_simple_function(Tokenizer& tokenizer, std::list<rhs_instr> &res)
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
}

// @NOTE: searchindex, searchalpha and interpolation_1d are basically the same
//        function, can they be merged into one?
void augment_interpolation_1d(Tokenizer &tokenizer, std::list<rhs_instr> &res)
{
   res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
   res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

   unsigned args_before_array = 2;
   for (unsigned arg = 1;
        arg <= args_before_array;
        ++arg)
   {
      Token *current_token = tokenizer.next_token();

      while (*current_token != Token_Type::comma)
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

      res.push_back(current_token->to_rhs()); // ','
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

void augment_searchindex(Tokenizer &tokenizer, std::list<rhs_instr> &res)
{
   res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
   res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

   Token *current_token = tokenizer.next_token();

   while (*current_token != Token_Type::comma)
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

   res.push_back(current_token->to_rhs()); // ','

   res.push_back(std::make_pair(BlockType::txt, "["));

   current_token = tokenizer.next_token();

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

void augment_searchalpha(Tokenizer &tokenizer, std::list<rhs_instr> &res)
{
   res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
   res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

   Token *current_token = tokenizer.next_token();

   while (*current_token != Token_Type::comma)
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

   res.push_back(current_token->to_rhs()); // ','

   res.push_back(std::make_pair(BlockType::txt, "["));

   current_token = tokenizer.next_token();

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



unsigned array_len_lookuptable(Tokenizer &tokenizer, std::list<rhs_instr> &res)
{
   tokenizer.save_state();

   unsigned array_len = 0;

   Token *current_token = tokenizer.next_token();

   while (*current_token != Token_Type::close_curly_bracket)
   {
      if (*current_token == Token_Type::function)
      {
         process_function(tokenizer, res);
      }
      else if (*current_token == Token_Type::comma)
      {
         ++array_len;
      }

      current_token = tokenizer.next_token();
   }
   ++array_len; // for the last argument without comma at the end

   tokenizer.restore_state();

   return array_len;
}

void augment_lookuptable(Tokenizer &tokenizer, std::list<rhs_instr> &res)
{
   res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
   res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

   Token *current_token = tokenizer.next_token();

   while (*current_token != Token_Type::comma)
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

   res.push_back(current_token->to_rhs()); // ','

   unsigned array_len = array_len_lookuptable(tokenizer, res);

   if ((array_len % 2) != 0)
   {
      throw enhancer_exception("Expected even number of elements in function lookuptable");
   }

   unsigned half_array_len = array_len / 2;
   unsigned args_counter = 0;

   res.push_back(std::make_pair(BlockType::txt, "["));

   current_token = tokenizer.next_token();

   while (*current_token != Token_Type::close_curly_bracket)
   {
      if (*current_token == Token_Type::function)
      {
         process_function(tokenizer, res);
      }
      else if (*current_token == Token_Type::comma)
      {
         ++args_counter;

         if (args_counter == half_array_len)
         {
            res.push_back(std::make_pair(BlockType::txt, "]"));
            res.push_back(std::make_pair(BlockType::txt, ","));
            res.push_back(std::make_pair(BlockType::txt, "["));
         }
         else
         {
            res.push_back(current_token->to_rhs());
         }
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


std::tuple<unsigned, unsigned, unsigned>
get_args_for_interpolation_2d(Tokenizer &tokenizer, std::list<rhs_instr> &res)
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
         process_function(tokenizer, res);
      }
      else if (*current_token == Token_Type::comma)
      {
         ++args_counter;
      }

      current_token = tokenizer.next_token();
   }
   ++args_counter; // for the last argument without comma at the end

   cols = (unsigned)tokenizer.prev_token(1)->num;
   rows = (unsigned)tokenizer.prev_token(3)->num;

   args_counter -= 2; // last two token are the number of cols and rows

   tokenizer.restore_state();

   return std::make_tuple(args_counter, rows, cols);
}

void augment_interpolation_2d(Tokenizer &tokenizer, std::list<rhs_instr> &res)
{
   res.push_back(tokenizer.current_token()->to_rhs()); // name of the function
   res.push_back(tokenizer.require_next_token(Token_Type::open_curly_bracket)->to_rhs());

   unsigned num_params_before_array = 4;

   for (unsigned param = 1;
        param <= num_params_before_array;
        ++param)
   {
      Token *current_token = tokenizer.next_token();

      while (*current_token != Token_Type::comma)
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

      res.push_back(current_token->to_rhs()); // ','
   }

   std::tuple<unsigned, unsigned, unsigned> args_interpolation_2d = get_args_for_interpolation_2d(tokenizer, res);

   unsigned array_len, rows, cols;
   array_len = std::get<0>(args_interpolation_2d);
   rows = std::get<1>(args_interpolation_2d);
   cols = std::get<2>(args_interpolation_2d);

   if (array_len != (rows * cols))
   {
      throw enhancer_exception("(rows * cols) does not match len of array in interpolation2d!");
   }

   res.push_back(std::make_pair(BlockType::txt, "["));
   res.push_back(std::make_pair(BlockType::txt, "["));

   unsigned elem_counter = 1;

   Token *current_token = tokenizer.next_token();

   while(elem_counter <= array_len)
   {
      if (*current_token == Token_Type::function)
      {
         process_function(tokenizer, res);
      }
      else if (*current_token == Token_Type::comma)
      {
         if ((elem_counter % cols == 0) &&
             (elem_counter != array_len)) // is not the last element
         {
            res.push_back(std::make_pair(BlockType::txt, "]"));
            res.push_back(current_token->to_rhs()); // ,
            res.push_back(std::make_pair(BlockType::txt, "["));
         }
         else
         {
            if (elem_counter != array_len) // is not the last element
            {
               res.push_back(current_token->to_rhs()); // ,
            }
            else
            {
               int stop = 0;
            }
         }
         ++elem_counter;
      }
      else
      {
         res.push_back(current_token->to_rhs());
      }

      current_token = tokenizer.next_token();
   }

   res.push_back(std::make_pair(BlockType::txt, "]"));
   res.push_back(std::make_pair(BlockType::txt, "]"));























#if 0 // old method to delete
   // interpolation2D(x,y,z,k,a11,a12,a13,b11,b12,b13,2,3}->
   //   interpolation2D(x,y,z,k,[[a11,a12,a13],[b11,b12,b13]]}
   while(elem_counter <= array_len)
   {
      Token *current_token = tokenizer.next_token();

      if (*current_token == Token_Type::function)
      {
         process_function(tokenizer, res);
         ++elem_counter;
      }
      else if (*current_token == Token_Type::number ||
               *current_token == Token_Type::variable)
      {
         if (elem_counter % cols == 0)
         {
            res.push_back(current_token->to_rhs());
            if (elem_counter != array_len)
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
         ++elem_counter;
      }
      else if (*current_token == Token_Type::comma)
      {
         res.push_back(current_token->to_rhs());
      }
      else
      {
         throw enhancer_exception("Unexpected token: " + token_type_to_str(current_token->type));
      }
   }

   res.push_back(std::make_pair(BlockType::txt, "]"));
   res.push_back(std::make_pair(BlockType::txt, "]"));
#endif

   tokenizer.require_next_token(Token_Type::comma); tokenizer.require_next_token(Token_Type::number); // cols token

   res.push_back(tokenizer.require_next_token(Token_Type::close_curly_bracket)->to_rhs());
}



void process_function(Tokenizer &tokenizer, std::list<rhs_instr> &res)
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

std::list<rhs_instr>
augment(const std::list<rhs_instr> &input)
{
   Tokenizer tokenizer = parse(input);

   std::list<rhs_instr> res;

   Token *current_token = tokenizer.current_token();

   while (*current_token != Token_Type::end_of_tokens)
   {
      if (*current_token == Token_Type::function)
      {
         process_function(tokenizer, res);
      }
      else
      {
         rhs_instr rhs = std::make_pair(current_token->block_type, current_token->value);
         res.push_back(rhs);
      }

      current_token = tokenizer.next_token();
   }

   return res;
}

