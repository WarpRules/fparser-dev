/*==========================================================================
  make_function_name_parser
  -------------------------
  Copyright: Joel Yliluoma
  This program (make_function_name_parser) is distributed under the terms of
  the GNU General Public License (GPL) version 3.
  See gpl.txt for the license text.
============================================================================*/

#define FUNCTIONPARSER_SUPPORT_DEBUGGING

#include <set>
#include <iostream>
#include <sstream>
#include <string.h>

#include "fparser.hh"
#include "fpconfig.hh"
#include "extrasrc/fptypes.hh"

using namespace FUNCTIONPARSERTYPES;

struct FuncDefinition
{
    const char opname[8];
    const char name[8];
    unsigned flags;
};
static const FuncDefinition Functions[] = // indexed by opcode
{
#define o(code, funcname, nparams, options) \
    { #code, #funcname, options | (nparams << 24) },
    FUNCTIONPARSER_LIST_FUNCTION_OPCODES(o)
#undef o
};
static constexpr unsigned FUNC_AMOUNT = sizeof(Functions) / sizeof(*Functions);

static void Compile(std::ostream& outStream, const std::string& prefix, std::size_t length)
{
    // if the prefix matches, check what we've got
    outStream << "/* prefix " << prefix << " */";

    for(std::size_t a=0; a<FUNC_AMOUNT; ++a)
        if(prefix == Functions[a].name
        && length == strlen(Functions[a].name))
        {
            outStream << "return (" << Functions[a].opname << "<<16) | 0x";
            outStream << std::hex << std::uppercase << (Functions[a].flags | length);
            outStream << std::dec << "u;";
            outStream << "\n    ";
            return;
        }

    std::size_t n_possible_children = 0;
    for(std::size_t a=0; a<FUNC_AMOUNT; ++a)
    {
        if(strlen(Functions[a].name) != length) continue;
        if(strlen(Functions[a].name) < prefix.size()) continue;
        if(prefix == std::string(Functions[a].name, prefix.size()))
            ++n_possible_children;
    }

    if(n_possible_children == 1)
    {
        for(std::size_t a=0; a<FUNC_AMOUNT; ++a)
        {
            if(strlen(Functions[a].name) != length) continue;
            if(strlen(Functions[a].name) < prefix.size()) continue;
            if(prefix == std::string(Functions[a].name, prefix.size()))
            {
                if(prefix != Functions[a].name)
                {
                    std::size_t tmpbytes = length - prefix.size();
                    if(tmpbytes > 2)
                    {
                        outStream << "{";
                        outStream << "static const char tmp[" << tmpbytes << "] = {";
                        for(std::size_t b=prefix.size(); b<length; ++b)
                        {
                            if(b > prefix.size()) outStream << ',';
                            outStream << "'" << Functions[a].name[b] << "'";
                        }
                        outStream << "};\n    ";
                    }

                    if(tmpbytes > 2)
                        outStream << "if(std::memcmp(uptr+" << prefix.size() << ", tmp, " << tmpbytes << ") == 0) ";
                    else
                    {
                        outStream << "if(";
                        for(std::size_t b=prefix.size(); b<length; ++b)
                        {
                            if(b != prefix.size()) outStream << "\n    && ";
                            outStream << "'" << Functions[a].name[b] << "' == uptr[" << b << "]";
                        }
                        outStream << ") ";
                    }

                    outStream << "return (" << Functions[a].opname << "<<16) | 0x";
                    outStream << std::hex << std::uppercase << (Functions[a].flags | length);
                    outStream << std::dec << "u;";
                    outStream << "\n    return " << length << ";";
                    if(tmpbytes > 2) outStream << " }";
                    outStream << "\n    ";
                }
            }
        }
        return;
    }

    std::set<char> possible_children;
    for(std::size_t a=0; a<FUNC_AMOUNT; ++a)
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
        outStream << "return " << length << ";\n    ";
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
                outStream << "if('" << *i << "' == uptr[" << prefix.size() << "]) {\n    ";
                std::string tmp(prefix);
                tmp += *i;
                Compile(outStream, tmp, length);
                outStream << "}";
            }
            outStream << "return " << length << ";";
        }
        else
        {
            outStream << "switch(uptr[" << prefix.size() << "]) {\n    ";
            for(std::set<char>::const_iterator
                i = possible_children.begin();
                i != possible_children.end();
                ++i)
            {
                outStream << "case '" << *i << "':\n    ";
                std::string tmp(prefix);
                tmp += *i;
                Compile(outStream, tmp, length);
            }
            outStream << "default: return " << length << "; }\n    ";
        }
    }
}

int main()
{
    std::ostringstream outStream;

    outStream <<
"        switch(nameLength)\n"
"        {\n    ";
    std::set<unsigned> lengthSet;
    for(std::size_t a=0; a<FUNC_AMOUNT; ++a)
        lengthSet.insert(strlen(Functions[a].name));
    for(std::set<unsigned>::iterator
        i = lengthSet.begin(); i != lengthSet.end(); ++i)
    {
        outStream << "         case " << *i << ":\n    ";
        Compile(outStream, "", *i);
        outStream << "\n    ";
    }
    outStream <<
"        default: break;\n"
"        }\n"
"        return nameLength;\n";

    //CPPcompressor Compressor;
    std::cout << outStream.str();
    //std::cout << Compressor.Compress(outStream.str(), "l");
}
