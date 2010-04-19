/*==========================================================================
  make_function_name_parser
  -------------------------
  Copyright: Joel Yliluoma
  This program (make_function_name_parser) is distributed under the terms of
  the GNU General Public License (GPL) version 3.
  See gpl.txt for the license text.
============================================================================*/

#define FUNCTIONPARSER_SUPPORT_DEBUG_OUTPUT

#include <set>
#include <iostream>
#include <string.h>

#include "fparser.hh"
#include "fpconfig.hh"
#include "fptypes.hh"

#include "../fpoptimizer/opcodename.cc"

using namespace FUNCTIONPARSERTYPES;

static void Compile(const std::string& prefix, size_t length)
{
    // if the prefix matches, check what we've got
    std::cout << "/* prefix " << prefix << " */";

    for(size_t a=0; a<FUNC_AMOUNT; ++a)
        if(prefix == Functions[a].name
        && length == strlen(Functions[a].name))
        {
            std::string o = FP_GetOpcodeName(OPCODE(a));
            std::cout << "return Functions[" << o << "].enabled() ? (" << o << "<<16) | 0x";
            std::cout << std::hex << (0x80000000U | length);
            std::cout << std::dec << "U : " << length << ";";
            std::cout << "\n    ";
            return;
        }

    size_t n_possible_children = 0;
    for(size_t a=0; a<FUNC_AMOUNT; ++a)
    {
        if(strlen(Functions[a].name) != length) continue;
        if(strlen(Functions[a].name) < prefix.size()) continue;
        if(prefix == std::string(Functions[a].name, prefix.size()))
            ++n_possible_children;
    }

    if(n_possible_children == 1)
    {
        for(size_t a=0; a<FUNC_AMOUNT; ++a)
        {
            if(strlen(Functions[a].name) != length) continue;
            if(strlen(Functions[a].name) < prefix.size()) continue;
            if(prefix == std::string(Functions[a].name, prefix.size()))
            {
                if(prefix != Functions[a].name)
                {
                    size_t tmpbytes = length - prefix.size();
                    if(tmpbytes > 2)
                    {
                        std::cout << "{";
                        std::cout << "static const char tmp[" << tmpbytes << "] = {";
                        for(size_t b=prefix.size(); b<length; ++b)
                        {
                            if(b > prefix.size()) std::cout << ',';
                            std::cout << "'" << Functions[a].name[b] << "'";
                        }
                        std::cout << "};\n    ";
                    }

                    if(tmpbytes > 2)
                        std::cout << "if(std::memcmp(uptr+" << prefix.size() << ", tmp, " << tmpbytes << ") == 0) ";
                    else
                    {
                        std::cout << "if(";
                        for(size_t b=prefix.size(); b<length; ++b)
                        {
                            if(b != prefix.size()) std::cout << "\n    && ";
                            std::cout << "'" << Functions[a].name[b] << "' == uptr[" << b << "]";
                        }
                        std::cout << ") ";
                    }

                    std::string o = FP_GetOpcodeName(OPCODE(a));
                    std::cout << "return Functions[" << o << "].enabled() ? (" << o << "<<16) | 0x";
                    std::cout << std::hex << (0x80000000U | length);
                    std::cout << std::dec << "U : " << length << ";";
                    std::cout << "\n    return " << length << ";";
                    if(tmpbytes > 2) std::cout << " }";
                    std::cout << "\n    ";
                }
            }
        }
        return;
    }

    std::set<char> possible_children;
    for(size_t a=0; a<FUNC_AMOUNT; ++a)
    {
        if(strlen(Functions[a].name) != length) continue;
        if(strlen(Functions[a].name) <= prefix.size()) continue;
        if(prefix == std::string(Functions[a].name, prefix.size()))
        {
            char c = Functions[a].name[prefix.size()];
            possible_children.insert(c);
        }
    }

    if(possible_children.empty())
    {
        std::cout << "return " << length << ";\n    ";
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
                std::cout << "if('" << *i << "' == uptr[" << prefix.size() << "]) {\n    ";
                std::string tmp(prefix);
                tmp += *i;
                Compile(tmp, length);
                std::cout << "}";
            }
            std::cout << "return " << length << ";";
        }
        else
        {
            std::cout << "switch(uptr[" << prefix.size() << "]) {\n    ";
            for(std::set<char>::const_iterator
                i = possible_children.begin();
                i != possible_children.end();
                ++i)
            {
                std::cout << "case '" << *i << "':\n    ";
                std::string tmp(prefix);
                tmp += *i;
                Compile(tmp, length);
            }
            std::cout << "default: return " << length << "; }\n    ";
        }
    }
}

int main()
{
    std::cout <<
"        switch(nameLength)\n"
"        {\n    ";
    std::set<unsigned> lengthSet;
    for(size_t a=0; a<FUNC_AMOUNT; ++a)
        lengthSet.insert(strlen(Functions[a].name));
    for(std::set<unsigned>::iterator
        i = lengthSet.begin(); i != lengthSet.end(); ++i)
    {
        std::cout << "         case " << *i << ":\n    ";
        Compile("", *i);
        std::cout << "\n    ";
    }
    std::cout <<
"        default: break;\n"
"        }\n"
"        return nameLength;\n";
}
