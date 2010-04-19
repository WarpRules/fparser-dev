#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdio>
#include <cctype>
#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>

namespace
{
    std::string GetDefinesFor(const std::string& type)
    {
        if(type == "float") return "FP_SUPPORT_FLOAT_TYPE";
        if(type == "long double") return "FP_SUPPORT_LONG_DOUBLE_TYPE";
        if(type == "long") return "FP_SUPPORT_LONG_INT_TYPE";
        if(type == "MpfrFloat") return "FP_SUPPORT_MPFR_FLOAT_TYPE";
        if(type == "GmpInt") return "FP_SUPPORT_GMP_INT_TYPE";
        return std::string();
    }
    std::string NumConst(const std::string& type, const std::string& value)
    {
        if(type == "long")        return value + "l";

        std::string fltvalue = value;

        char* endptr = 0;
        strtol(value.c_str(), &endptr, 10);
        if(endptr && !*endptr)
            fltvalue += ".0";

        if(type == "float")       return fltvalue + "f";
        if(type == "long double") return fltvalue + "l";
        if(type == "double")      return fltvalue;
        return value;
    }
    std::string GetTypeFor(const std::string& typecode)
    {
        if(typecode == "d")
            return ("double");
        else if(typecode == "f")
            return ("float");
        else if(typecode == "ld")
            return ("long double");
        else if(typecode == "li")
            return ("long");
        else if(typecode == "mf")
            return ("MpfrFloat");
        else if(typecode == "gi")
            return ("GmpInt");
        return typecode;
    }
}


struct TestData
{
    std::string IfDef;

    std::string FuncString, ParamString;
    unsigned ParamAmount;
    std::string ParamValueRanges;
    bool UseDegrees;
    std::string TestFuncName, TestName;
    std::set<std::string> DataTypes;

    TestData():
        FuncString(), ParamString(),
        ParamAmount(0),
        ParamValueRanges(),
        UseDegrees(false),
        TestFuncName(), TestName()
    {
    }
};

typedef std::vector<TestData> TestCollection;

std::map<std::string/*datatype*/,
         TestCollection> tests;

std::set<std::string> mpfrconst_set;

std::map<std::string, std::string> define_sections;
std::string default_function_section;
std::map<std::string, std::string> class_declarations;

std::string TranslateString(const std::string& str);

void ListTests(std::ostream& outStream)
{
    for(std::map<std::string, TestCollection>::const_iterator
        i = tests.begin();
        i != tests.end();
        ++i)
    {
        const std::string& type = i->first;
        std::string defines = GetDefinesFor(type);
        size_t n_tests         = i->second.size();

        outStream << "\n";

        if(!defines.empty())
            outStream << "#ifdef " << defines << "\n";
        outStream << "#define Value_t " << type << "\n";

        outStream <<
            "template<>\n"
            "struct RegressionTests<Value_t>\n"
            "{\n"
            "    static const TestType<Value_t> Tests[];\n"
            "};\n"
            //"template<>\n"
            "const TestType<Value_t>\n"
            "    RegressionTests<Value_t>::Tests[]";
        if(n_tests == 0)
        {
            outStream <<
                " = { TestType<Value_t>() };\n";
        }
        else
        {
            outStream << " =\n{\n";
            for(size_t a=0; a<n_tests; ++a)
            {
                const TestData& testdata = i->second[a];

                if(!testdata.IfDef.empty())
                    outStream << "#if " << testdata.IfDef << "\n";

                std::ostringstream ranges;
                const char* rangesdata = testdata.ParamValueRanges.c_str();
                while(*rangesdata)
                {
                    char* endptr = 0;
                    std::strtod(rangesdata, &endptr);
                    if(endptr && endptr != rangesdata)
                    {
                        ranges << NumConst(type, std::string(rangesdata,endptr-rangesdata));
                        rangesdata = endptr;
                    }
                    else
                        ranges << *rangesdata++;
                }

                outStream
                    << "    { " << testdata.ParamAmount
                    << ", " << ranges.str()
                    << ", " << (testdata.UseDegrees ? "true" : "false")
                    << ", " << testdata.TestFuncName << "<Value_t>";
                if(type == "MpfrFloat"
                && testdata.DataTypes.find("double")
                != testdata.DataTypes.end())
                {
                    // If the same test is defined for both "double" and
                    // "MpfrFloat", include an extra pointer to the "double"
                    // test in the "MpfrFloat" test.
                    outStream
                        << ", " << testdata.TestFuncName << "<double>";
                }
                else
                    outStream
                        << ", 0";

                if(type == "GmpInt"
                && testdata.DataTypes.find("long")
                != testdata.DataTypes.end())
                {
                    // If the same test is defined for both "long" and
                    // "GmpInt", include an extra pointer to the "long"
                    // test in the "GmpInt" test.
                    outStream
                        << ", " << testdata.TestFuncName << "<long>,";
                }
                else
                    outStream
                        << ", 0,";

                outStream
                    << "\n      " << TranslateString(testdata.ParamString)
                    << ", " << TranslateString(testdata.TestName)
                    << ", " << TranslateString(testdata.FuncString)
                    << " },\n";
                if(!testdata.IfDef.empty())
                    outStream << "#endif /*" << testdata.IfDef << " */\n";
            }
            outStream << "    TestType<Value_t>()\n};\n";
        }

        outStream << "#undef Value_t\n";
        if(!defines.empty())
            outStream << "#endif /*" << defines << " */\n";
    }
}

template<typename CharT>
void
str_replace_inplace(std::basic_string<CharT>& where,
                    const std::basic_string<CharT>& search,
                    const std::basic_string<CharT>& with)
{
    for(typename std::basic_string<CharT>::size_type a = where.size();
        (a = where.rfind(search, a)) != where.npos;
        )
    {
        where.replace(a, search.size(), with);
        if(a--==0) break;
    }
}


void CompileFunction(const char*& funcstr, const std::string& eval_name,
                     std::ostream& declbuf,
                     std::ostream& codebuf,
                     const std::string& limited_to_datatype)
{
    static unsigned BufCounter = 0;

    unsigned depth = 0;

    while(*funcstr && *funcstr != '}' && (*funcstr != ',' || depth>0))
    {
        if(strncmp(funcstr, "EVAL", 4) == 0)
        {
            codebuf << eval_name;
            funcstr += 4;
            continue;
        }
        if(funcstr[0] == '(' && funcstr[1] == '{')
        {
            codebuf << "<Value_t>(";
            funcstr += 2;
            unsigned NParams = 0;
            std::string BufName;

            codebuf << "(";
            for(;;)
            {
                while(std::isspace(*funcstr)) ++funcstr;
                if(!*funcstr) break;
                if(*funcstr == '}') { ++funcstr; break; }

                ++NParams;
                if(NParams == 1)
                {
                    std::ostringstream BufNameBuf;
                    BufNameBuf << "b" << BufCounter++;
                    BufName = BufNameBuf.str();
                }

                codebuf << BufName << "[" << (NParams-1) << "]=(";

                CompileFunction(funcstr, eval_name,
                    declbuf, codebuf, limited_to_datatype);

                codebuf << "), ";
                if(*funcstr == ',') ++funcstr;
            }

            if(NParams)
            {
                declbuf << "    Value_t " << BufName << "[" << NParams << "];\n";
                codebuf << BufName;
            }
            else
            {
                codebuf << "0";
            }
            codebuf << "))";
            while(std::isspace(*funcstr)) ++funcstr;
            if(*funcstr == ')') ++funcstr;
        }
        else
        {
            if(*funcstr == '(') ++depth;
            if(*funcstr == ')') --depth;

            char* endptr = 0;
            if((*funcstr >= '0' && *funcstr <= '9') || *funcstr == '.')
                std::strtod(funcstr, &endptr);
            if(endptr && endptr != funcstr)
            {
                if(limited_to_datatype == "MpfrFloat")
                {
                    std::string num(funcstr, endptr-funcstr);
                    char* endptr2 = 0;
                    strtol(funcstr, &endptr2, 10);
                    /*if(endptr2 && std::strcmp(endptr2, ".0") == 0)
                    {
                        num.erase(num.size()-2, num.size()); // made-int
                        codebuf << "Value_t(" << num << ")";
                    }
                    else*/ if(endptr2 && endptr2 == endptr) // an int or long
                    {
                        codebuf << "Value_t(" << num << ")";
                    }
                    else
                    {
                        std::string mpfrconst_name = "mflit" + num;
                        str_replace_inplace(mpfrconst_name, std::string("."), std::string("_"));
                        str_replace_inplace(mpfrconst_name, std::string("+"), std::string("p"));
                        str_replace_inplace(mpfrconst_name, std::string("-"), std::string("m"));

                        if(mpfrconst_set.insert(mpfrconst_name).second)
                        {
                            define_sections["FP_SUPPORT_MPFR_FLOAT_TYPE"]
                                += "static const Value_t " + mpfrconst_name
                                 + "\n    = Value_t::parseString(\"" + num + "\", 0);\n";
                        }

                        codebuf << mpfrconst_name;
                    }
                    //if(*endptr == 'f' || *endptr == 'l') ++endptr;
                }
                else
                {
                    std::string num(funcstr, endptr-funcstr);
                    if(limited_to_datatype.empty())
                        codebuf << "Value_t(" << num << "l)";
                    else
                        codebuf << NumConst(limited_to_datatype, num);
                    /*
                    if(*endptr == 'f' || *endptr == 'l')
                        num += *endptr++;
                    else
                        num += 'l';
                    codebuf << "Value_t(" << num << ")";
                    */
                }
                funcstr = endptr;
            }
            else if((*funcstr >= 'A' && *funcstr <= 'Z')
                 || (*funcstr >= 'a' && *funcstr <= 'z')
                 || *funcstr == '_')
            {
                do {
                    codebuf << *funcstr++;
                } while((*funcstr >= 'A' && *funcstr <= 'Z')
                     || (*funcstr >= 'a' && *funcstr <= 'z')
                     || (*funcstr >= '0' && *funcstr <= '9')
                     || *funcstr == '_');
            }
            else
                codebuf << *funcstr++;
        }
    }
}

std::string ReplaceVars(const char* function,
                        const std::map<std::string, std::string>& var_trans)
{
    std::string result = function;

    for(std::map<std::string, std::string>::const_iterator
        i = var_trans.begin();
        i != var_trans.end();
        ++i)
    {
        str_replace_inplace(result, i->first, i->second);
    }

    return result;
}

//std::string StringBuffer;
std::string TranslateString(const std::string& str)
{
    std::string val = str;
    str_replace_inplace(val, std::string("/"), std::string("\"\"/\"\""));
    str_replace_inplace(val, std::string("+"), std::string("\"\"+\"\""));
    str_replace_inplace(val, std::string("*"), std::string("\"\"*\"\""));
    str_replace_inplace(val, std::string("x"), std::string("\"\"x\"\""));
    str_replace_inplace(val, std::string("|"), std::string("\"\"|\"\""));
    str_replace_inplace(val, std::string("&"), std::string("\"\"&\"\""));
    str_replace_inplace(val, std::string("pow"), std::string("\"\"pow\"\""));
    str_replace_inplace(val, std::string("sin"), std::string("\"\"sin\"\""));
    if(val[0] == '"') val.erase(0,1); else val.insert(val.begin(), '"');
    if(val[val.size()-1] == '"') val.erase(val.size()-1, 1); else val += '"';
    str_replace_inplace(val, std::string("\"\"\"\""), std::string(""));
    return val;
    /*
    if(str.size() <= 6)
    {
        return '"' + str + '"';
    }
    std::string keyword = str;
    keyword += '\0';
    size_t p = StringBuffer.find(keyword);
    if(p == StringBuffer.npos)
    {
        p = StringBuffer.size();
        StringBuffer += keyword;
    }
    char Buf[128];
    std::sprintf(Buf, "ts+%u", (unsigned)p);
    return Buf;
    */
}
/*
void MakeStringBuffer(std::ostream& out)
{
    size_t pos = 26; bool quote = false;
    out << "const char ts[" << StringBuffer.size() << "] = ";
    for(size_t a=0; a < StringBuffer.size(); ++a)
    {
        //if(pos >= 70) { if(quote) { quote=false; out << '"'; } out << "\n"; pos = 0; }
        if(!quote) { quote=true; out << '"'; ++pos; }
        if(StringBuffer[a] == '\0')
            { out << "\\0"; pos += 2;
              if(a+1 < StringBuffer.size()
              && std::isdigit(StringBuffer[a+1]))
                { out << '"'; quote=false; ++pos; }
            }
        else
            { out << StringBuffer[a]; pos += 1;
              if(StringBuffer[a] == '/')
                { out << '"'; quote=false; ++pos; }
            }
    }
    if(quote) out << '"';
    out << ";\n";
}*/

static const char cbuf[] =
"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";

std::pair<std::string, std::string>
    MakeFuncName(const std::string& testname)
{
#if 0
    static unsigned counter = 0;
    std::string result = "qZ";
    for(unsigned p = counter++; p != 0; p /= 63)
        result += cbuf[p % 63];
    return result;
#else
    std::string base = "cpp/" + testname;

    size_t p = base.rfind('/');
    std::string classname = base.substr(0, p);
    std::string methodname = base.substr(p+1);
    str_replace_inplace(classname, std::string("/"), std::string("_"));
    str_replace_inplace(methodname, std::string("/"), std::string("_"));
    // Change the method name to prevent clashes with
    // with reserved words or the any namespace
    if(isdigit(methodname[0]))
        methodname.insert(0, "t");
    else
        methodname[0] = std::toupper(methodname[0]);
    return std::make_pair(classname, methodname);
#endif
}

void CompileTest(const std::string& testname, FILE* fp)
{
    char Buf[4096]={0};
    std::string linebuf;

    TestData test;
    std::set<std::string> DataTypes;

    test.TestName = testname;
    str_replace_inplace(test.TestName, std::string("tests/"), std::string(""));

    std::ostringstream declbuf;

    std::map<std::string, std::string> var_trans;

    std::string limited_to_datatype;

    unsigned linenumber = 0;
    while(fgets(Buf,sizeof(Buf)-1,fp))
    {
        ++linenumber;
        const char* line = Buf;
        while(*line == ' ' || *line == '\t') ++line;
        std::strtok(Buf, "\r");
        std::strtok(Buf, "\n");

        const char* backslash = std::strchr(line, '\\');
        if(backslash && backslash[1] == '\0')
        {
            linebuf = "";
            for(;;)
            {
                // Append the line, sans backslash
                linebuf.append(line, backslash-line);
                linebuf += ' ';

                if(!fgets(Buf,sizeof(Buf)-1,fp)) break;
                ++linenumber;
                const char* line = Buf;
                while(*line == ' ' || *line == '\t') ++line;
                std::strtok(Buf, "\r");
                std::strtok(Buf, "\n");
                backslash = std::strchr(line, '\\');

                if(backslash && backslash[1] == '\0')
                    continue;

                // add the final, backslash-less line
                linebuf += line;
                break;
            }
            line = linebuf.c_str();
        }
        else
        {
            // no backslash on the line
            linebuf = Buf;
        }

        const char* valuepos = std::strchr(line, '=');
        if(valuepos)
        {
            ++valuepos;
            while(*valuepos == ' ' || *valuepos == '\t') ++valuepos;
        }

        switch(line[0])
        {
            case '#':
                continue; // comment line
            case '\0':
                continue; // blank line
            case 'D': // test define condition
                if(line[1] == 'E')
                    test.UseDegrees = true;
                else if(valuepos)
                    test.IfDef = valuepos;
                break;
            case 'T': // list of applicable types
                if(valuepos)
                {
                    for(;;)
                    {
                        while(*valuepos == ' ') ++valuepos;
                        if(!*valuepos) break;

                        const char* space = std::strchr(valuepos, ' ');
                        if(!space) space = std::strrchr(valuepos, '\0');
                        std::string type(valuepos, space);

                        DataTypes.insert(GetTypeFor(type));

                        valuepos = space;
                    }

                    if(DataTypes.size() == 1)
                        limited_to_datatype = *DataTypes.begin();

                    test.DataTypes = DataTypes;
                }
                break;
            case 'V': // variable list
                if(valuepos)
                {
                    test.ParamString = valuepos;
                    test.ParamAmount = test.ParamString.empty() ? 0 : 1;

                    const char* begin = valuepos;

                    std::vector<std::string> vars;

                    for(; *valuepos; ++valuepos)
                        if(*valuepos == ',')
                        {
                            vars.push_back( std::string(begin,valuepos-begin) );
                            begin = valuepos+1;
                            ++test.ParamAmount;
                        }

                    if(begin != valuepos)
                        vars.push_back(begin);

                    bool outputted_line_stmt = false;

                    for(size_t a=0; a<vars.size(); ++a)
                    {
                        std::string oldvarname = vars[a];
                        std::string newvarname = vars[a];
                        bool needs_replacement = false;
                        for(size_t b=0; b<oldvarname.size(); ++b)
                        {
                            char c = oldvarname[b];
                            if((c >= '0' && c <= '9')
                            || c == '_'
                            || (c >= 'A' && c <= 'Z')
                            || (c >= 'a' && c <= 'z')) continue;
                            needs_replacement = true; break;
                        }
                        if(needs_replacement)
                        {
                            static unsigned var_counter = 0;
                            std::ostringstream varnamebuf;
                            varnamebuf << "rvar" << var_counter++;
                            newvarname = varnamebuf.str();
                            var_trans[oldvarname] = newvarname;
                        }

                        if(!outputted_line_stmt)
                        {
                            outputted_line_stmt = true;
                            //declbuf << "#line " << linenumber << " \"" << testname << "\"\n";
                            declbuf << "    const Value_t";
                        }
                        else
                            declbuf << ",";
                        declbuf << " &" << newvarname
                                << " = vars[" << a << "]";
                    }
                    if(outputted_line_stmt)
                        declbuf << ";\n";
                }
                break;
            case 'R': // parameter value ranges
                if(valuepos)
                    test.ParamValueRanges = valuepos;
                break;
            case 'F': // the function string
                if(valuepos)
                    test.FuncString = valuepos;
                break;
            case 'C': // the C++ template function
                if(valuepos)
                {
                    std::string Replaced;
                    if(!var_trans.empty())
                    {
                        Replaced = ReplaceVars(valuepos, var_trans);
                        valuepos = Replaced.c_str();
                    }

                    std::pair<std::string,std::string>
                        funcname = MakeFuncName(test.TestName);
                    test.TestFuncName = funcname.first+"::"+funcname.second;

                    bool includes_mpfr = DataTypes.find("MpfrFloat") != DataTypes.end();
                    bool unitype = DataTypes.size() == 1;

                    std::ostringstream out;

                    if(!unitype || !includes_mpfr)
                    {
                        std::ostringstream declbuf1, codebuf1;
                        declbuf1 << declbuf.str();
                        //declbuf1 << "#line " << linenumber << " \"" << testname << "\"\n";

                        const char* valuepos_1 = valuepos;
                        CompileFunction(valuepos_1, funcname.second, declbuf1, codebuf1,
                                        limited_to_datatype);

                        std::ostringstream body;

                        body <<
                            "{\n" <<
                            declbuf1.str();
                        //out << "#line " << linenumber << " \"" << testname << "\"\n";
                        body <<
                            "    return " << codebuf1.str() << ";\n"
                            "}\n";

                        std::ostringstream decl;

                        if(limited_to_datatype.empty() || limited_to_datatype == "double")
                        {
                            std::string bodystr = body.str();
                            str_replace_inplace(bodystr, std::string("\n"), std::string("\n    "));
                            decl <<
                                "    template<typename Value_t> static Value_t " << funcname.second
                                           << "(const Value_t* vars)\n"
                                           << "    " << bodystr << "\n";
                            class_declarations[funcname.first] += decl.str();
                        }
                        else
                        {
                            decl <<
                                "    template<typename Value_t> static Value_t " << funcname.second
                                           << "(const Value_t* vars);\n";
                            class_declarations[funcname.first] += decl.str();

                            out <<
                                "template<typename Value_t>\n"
                                "Value_t " << funcname.first << "::" << funcname.second
                                           << "(const Value_t* vars)\n"
                                           << body.str();
                        }
                    }
                    else
                    {
                        std::ostringstream decl;
                        decl <<
                            "    template<typename Value_t> static Value_t " << funcname.second << "(const Value_t* vars);\n";
                        class_declarations[funcname.first] += decl.str();
                    }

                    ((limited_to_datatype.empty() || limited_to_datatype == "double")
                        ? default_function_section
                        : define_sections[GetDefinesFor(limited_to_datatype)]) += out.str();

                    if(includes_mpfr)
                    {
                        std::ostringstream declbuf2, codebuf2;
                        declbuf2 << declbuf.str();
                        //declbuf2 << "#line " << linenumber << " \"" << testname << "\"\n";

                        CompileFunction(valuepos, funcname.second,
                                        declbuf2, codebuf2, "MpfrFloat");

                        if(codebuf2.str().find("mflit") != codebuf2.str().npos
                        || unitype)
                        {
                            std::ostringstream out2;

                            if(!test.IfDef.empty())
                                out2 << "#if " << test.IfDef << "\n";

                            out2 <<
                                "template<>\n"
                                "Value_t " << funcname.first << "::" << funcname.second
                                             << "<Value_t> (const Value_t* vars)\n"
                                "{\n"
                                << declbuf2.str();
                            //out2 << "#line " << linenumber << " \"" << testname << "\"\n";
                            std::string code = codebuf2.str();
                            str_replace_inplace(code, std::string("MpfrFloat"), std::string("Value_t"));
                            out2 <<
                                "    return " << code << ";\n"
                                "}\n";

                            if(!test.IfDef.empty())
                                out2 << "#endif /* " << test.IfDef << " */\n";

                            define_sections["FP_SUPPORT_MPFR_FLOAT_TYPE"] += out2.str();
                        }
                    }
                }
                break;
        }
    }

    for(std::set<std::string>::const_iterator
        i = DataTypes.begin();
        i != DataTypes.end();
        ++i)
    {
        tests[*i].push_back(test);
    }
}

/* Asciibetical comparator, with in-string integer values sorted naturally */
bool natcomp(const std::string& a, const std::string& b)
{
    size_t ap=0, bp=0;
    while(ap < a.size() && bp < b.size())
    {
        if(a[ap] >= '0' && a[ap] <= '9'
        && b[bp] >= '0' && b[bp] <= '9')
        {
            unsigned long aval = (a[ap++] - '0');
            unsigned long bval = (b[bp++] - '0');
            while(ap < a.size() && a[ap] >= '0' && a[ap] <= '9')
                aval = aval*10ul + (a[ap++] - '0');
            while(bp < b.size() && b[bp] >= '0' && b[bp] <= '9')
                bval = bval*10ul + (b[bp++] - '0');
            if(aval != bval)
                return aval < bval;
        }
        else
        {
            if(a[ap] != b[ap]) return a[ap] < b[ap];
            ++ap; ++bp;
        }
    }
    return (bp < b.size() && ap >= a.size());
}

#include "../util/cpp_compress.hh"

int main(int argc, char* argv[])
{
    const char* outputFileName = 0;
    std::ofstream outputFileStream;

    std::ostringstream out;

    std::vector<std::string> files;

    for(int a=1; a<argc; ++a)
    {
        if(std::strcmp(argv[a], "-o") == 0)
        {
            if(++a == argc)
            {
                std::cerr << "Expecting output file name after -o\n";
                return 1;
            }
            outputFileName = argv[a];
            outputFileStream.open(argv[a]);
            if(!outputFileStream)
            {
                std::cerr << "Could not write to " << argv[a] << "\n";
                return 1;
            }
            continue;
        }

        std::string fn ( argv[a] );
        if(fn.empty()) continue;

        if(fn[fn.size()-1] == '~') continue; // ignore backup files
        if(fn[0] == '.') continue;           // ignore special files

        files.push_back(fn);
    }

    std::ostream& outStream = outputFileName ? outputFileStream : std::cout;
    const char* outStreamName = outputFileName ? outputFileName : "<stdout>";

    std::sort(files.begin(), files.end(), natcomp);

    for(size_t a=0; a<files.size(); ++a)
    {
        FILE* fp = std::fopen(files[a].c_str(), "rt");
        if(!fp)
        {
            std::perror(files[a].c_str());
            continue;
        }
        CompileTest(files[a], fp);
        fclose(fp);
    }

    out << "namespace { using namespace FUNCTIONPARSERTYPES;\n";

    for(std::map<std::string, std::string>::const_iterator
        i = class_declarations.begin();
        i != class_declarations.end();
        ++i)
    {
        out << "struct " << i->first << "\n"
            << "{\n"
            << i->second
            << "};\n";
    }
    out << default_function_section;

    for(std::map<std::string, std::string>::const_iterator
        i = define_sections.begin(); i != define_sections.end(); ++i)
    {
        if(i->first != "FP_SUPPORT_MPFR_FLOAT_TYPE")
            out << "\n#ifdef " << i->first << "\n" << i->second
                << "#endif /*" << i->first << " */\n";
    }
    std::map<std::string, std::string>::const_iterator
        i = define_sections.find("FP_SUPPORT_MPFR_FLOAT_TYPE");
    out << "\n#ifdef " << i->first << "\n"
        << "#define Value_t MpfrFloat\n"
        << i->second
        << "#undef Value_t\n"
        << "#endif /*" << i->first << " */\n";

    out << "}\n";

    ListTests(out);


    //MakeStringBuffer(out);
    //outStream << "extern const char ts[" << StringBuffer.size() << "];\n";

    CPPcompressor Compressor;

    //outStream << out.str();
    outStream << Compressor.Compress(out.str());

    return 0;
}
