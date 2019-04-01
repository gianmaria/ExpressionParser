#include "parser.h"
#include "enhancer.h"

#include <cctype> // isalpha


std::string token_type_to_str(const Token_Type &type)
{
   switch (type)
   {
      case Token_Type::function: return "function";
      case Token_Type::variable: return "variable";
      case Token_Type::number: return "number";
      case Token_Type::comma: return "comma";

      case Token_Type::plus: return "plus_sign";
      case Token_Type::minus: return "minus_sign";
      case Token_Type::times: return "multiply_sign";
      case Token_Type::division: return "division_sign";

      case Token_Type::equal: return "equal";
      case Token_Type::not_equal : return "greater_than";
      case Token_Type::greater_than : return "greater_than";
      case Token_Type::greater_than_or_eq: return "greater_than_or_eq";
      case Token_Type::less_than: return "less_than";
      case Token_Type::less_than_or_eq: return "less_than_or_eq";

      case Token_Type::open_parenthesis: return "open_parenthesis";
      case Token_Type::close_parenthesis: return "close_parenthesis";
      case Token_Type::open_square_bracket: return "open_square_bracket";
      case Token_Type::close_square_bracket: return "close_square_bracket";
      case Token_Type::open_curly_bracket: return "open_curly_bracket";
      case Token_Type::close_curly_bracket: return "close_curly_bracket";
      case Token_Type::end_of_tokens: return "end_of_tokens";
      case Token_Type::unknown: return "unknown";
      default: throw std::runtime_error("Unknown token type!");
   }
}

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
      token.type = Token_Type::plus;
   }
   else if (input == "-")
   {
      token.type = Token_Type::minus;
   }
   else if (input == "*")
   {
      token.type = Token_Type::times;
   }
   else if (input == "/")
   {
      token.type = Token_Type::division;
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
   else if (input == "=")
   {
      token.type = Token_Type::equal;
   }
   else if (input == "!=")
   {
      token.type = Token_Type::not_equal;
   }
   else if (input == ">")
   {
      token.type = Token_Type::greater_than;
   }
   else if (input == ">=")
   {
      token.type = Token_Type::greater_than_or_eq;
   }
   else if (input == "<")
   {
      token.type = Token_Type::less_than;
   }
   else if (input == "<=")
   {
      token.type = Token_Type::less_than_or_eq;
   }
   else
   {
      char c = input[0];

      if (c == '-' || std::isdigit(c)) // we have negative number as a single token
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
         throw std::runtime_error(error.c_str());
      }
   }

   return token;
}

Tokenizer parse(const std::list<rhs> &input)
{
   Tokenizer tokenizer;
   tokenizer.input = input;

   unsigned line = 1;
   for (const rhs &elem : input)
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
