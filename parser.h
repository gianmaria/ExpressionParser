#pragma once

#include <string> // string
#include <list> // list
#include <stdexcept> // exception
#include <vector> // vector
#include <utility> // pair


#include "common.h"


enum class Token_Type
{
   function, variable, number, comma,

   equal, not_equal,

   greater_than, greater_than_or_eq,
   less_than, less_than_or_eq,

   plus, minus, times, division,

   open_parenthesis, close_parenthesis,
   open_square_bracket, close_square_bracket,
   open_curly_bracket, close_curly_bracket,
   end_of_tokens,
   unknown
};

std::string token_type_to_str(const Token_Type &type);

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
         throw std::runtime_error("Only one save at a time!");
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
         throw std::runtime_error(error.c_str());
      }

      return token;
   }

};

Token tokenize(const rhs &elem);

Tokenizer parse(const std::list<rhs> &input);
