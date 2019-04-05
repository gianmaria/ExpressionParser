#pragma once

#include <list>
#include <string>

#include "SDDParameters.h"

#define enhancer_exception(msg) ( \
    std::runtime_error(std::string("[EXCEPTION]") + std::string("\n") + \
        std::string("File: ") + std::string(__FILE__) + std::string("\n") + \
        std::string("Function: ") + std::string(__func__) + std::string("():") + std::to_string(__LINE__) + std::string("\n") + \
        std::string("Msg: ") + std::string(msg)) \
)

using rhs = std::pair<BlockType, std::string>;
using rhs_instr = std::pair<BlockType, std::string>;

inline std::string block_type_to_str(BlockType type)
{
    switch (type)
    {
        case BlockType::txt: { return "txt"; }
        case BlockType::tp: { return "tp";  }
        case BlockType::tmp: { return "tmp"; }
        case BlockType::mem: { return "mem"; }
        case BlockType::upd: { return "upd"; }
        default: return "unknown";
    }
}
