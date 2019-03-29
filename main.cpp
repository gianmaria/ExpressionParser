#include <iostream> // cout
#include <string>
#include <list>

//#include <vector>
//#include <utility> // pair
//#include <stdexcept> // exception
//#include <algorithm> // count_if
//#include <sstream> // ostringstream
//#include <tuple>
//#include <map>
//#include <functional> // function

#include <cstdio> // getc
//#include <cctype> // isalpha
//#include <cstddef> // size_t
//#include <cassert> // assert

using std::cout;
using std::endl;


#include "tinyxml2.h"
using namespace tinyxml2;

#include "common.h"
#include "enhancer.h"


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
    const XMLElement *rhs = root->FirstChildElement();

    while (rhs)
    {
        const XMLElement *block = rhs->FirstChildElement();
        
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

        rhs = rhs->NextSiblingElement();
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

