#pragma once

#include <list>
#include <string>

enum class BlockType : unsigned int
{
    txt,
    tp,
    tmp,
    mem,
    upd
};

using rhs = std::pair<BlockType, std::string>;

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
