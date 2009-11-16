#define FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT

#include <set>
#include <iostream>
#include <string.h>
#include "fparser.hh"
#include "fpconfig.hh"
#include "fptypes.hh"

#include "fpoptimizer/fpoptimizer_opcodename.cc"

using namespace FUNCTIONPARSERTYPES;

static void Compile(const std::string& prefix, size_t parent_maxlen)
{
    // if the prefix matches, check what we've got
    std::cout << "/* prefix " << prefix << " */";

    size_t n_possible_children = 0;
    for(size_t a=0; a<FUNC_AMOUNT; ++a)
    {
        if(strlen(Functions[a].name) < prefix.size()) continue;
        if(prefix == std::string(Functions[a].name, prefix.size()))
            ++n_possible_children;
    }

    if(n_possible_children == 1)
    {
        for(size_t a=0; a<FUNC_AMOUNT; ++a)
        {
            if(strlen(Functions[a].name) < prefix.size()) continue;
            if(prefix == std::string(Functions[a].name, prefix.size()))
            {
                if(prefix != Functions[a].name
                || prefix.size() != parent_maxlen)
                {
                    if(strlen(Functions[a].name) != parent_maxlen
                    || strlen(Functions[a].name) > prefix.size())
                        std::cout << "if(functionName.nameLength == " << strlen(Functions[a].name) << "\n    ";
                    else
                        std::cout << "if(true\n    ";

                    for(size_t b=prefix.size(); b<strlen(Functions[a].name); ++b)
                        std::cout << "&& '" << Functions[a].name[b] << "' == functionName.name[" << b << "]\n    ";
                    std::cout << ") ";
                    std::cout << "return Functions+" << FP_GetOpcodeName(OPCODE(a)) << ";/*"
                              << Functions[a].name
                              << "*/\n    else return 0;\n    ";
                }
                else
                {
                    std::cout << "return Functions+" << FP_GetOpcodeName(OPCODE(a)) << ";/*"
                              << Functions[a].name
                              << "*/\n    ";
                }
            }
        }
        return;
    }

    if(prefix.size() != parent_maxlen)
        std::cout << "if(functionName.nameLength == " << prefix.size() << ") ";
    std::cout << "return ";
    for(size_t a=0; a<FUNC_AMOUNT; ++a)
        if(prefix == Functions[a].name)
        {
            std::cout << "Functions+" << FP_GetOpcodeName(OPCODE(a)) << ";/*" << prefix << "*/\n    ";
            goto cont;
        }
    std::cout << "0;\n    ";
cont:;

    std::set<char> possible_children;
    for(size_t a=0; a<FUNC_AMOUNT; ++a)
    {
        if(strlen(Functions[a].name) <= prefix.size()) continue;
        if(prefix == std::string(Functions[a].name, prefix.size()))
        {
            char c = Functions[a].name[prefix.size()];
            possible_children.insert(c);
        }
    }

    size_t max_length = 0;
    for(size_t a=0; a<FUNC_AMOUNT; ++a)
    {
        if(strlen(Functions[a].name) <= prefix.size()) continue;
        if(prefix == std::string(Functions[a].name, prefix.size()))
        {
            size_t length = strlen(Functions[a].name);
            if(length > max_length) max_length = length;
        }
    }
    if(max_length > 0 && max_length < parent_maxlen)
    {
        std::cout << "if(functionName.nameLength > " << max_length << ") return 0;\n    ";
    }

    if(possible_children.empty())
    {
        std::cout << "return 0;\n    ";
    }
    else
    {
        if(possible_children.size() == 1)
        {
            for(std::set<char>::const_iterator
                i = possible_children.begin();
                i != possible_children.end();
                ++i)
            {
                std::cout << "if('" << *i << "' == functionName.name[" << prefix.size() << "]) {\n    ";
                std::string tmp(prefix);
                tmp += *i;
                Compile(tmp, max_length);
                std::cout << "}";
            }
            std::cout << "else return 0;";
        }
        else
        {
            std::cout << "switch(functionName.name[" << prefix.size() << "]) {\n    ";
            for(std::set<char>::const_iterator
                i = possible_children.begin();
                i != possible_children.end();
                ++i)
            {
                std::cout << "case '" << *i << "':\n    ";
                std::string tmp(prefix);
                tmp += *i;
                Compile(tmp, max_length);
            }
            std::cout << "default: return 0; }";
        }
    }
}

int main()
{
    std::cout <<
"    inline const FuncDefinition* findFunction(const NamePtr& functionName)\n"
"    {\n    ";
    Compile("", 255);
    std::cout <<
"}\n";
}
