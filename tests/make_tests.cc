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
#include <unordered_map>
#include <functional>

namespace
{
    std::string GetDefinesFor(const std::string& type)
    {
        if(type == "float") return "FP_TEST_WANT_FLOAT_TYPE";
        if(type == "long double") return "FP_TEST_WANT_LONG_DOUBLE_TYPE";
        if(type == "long") return "FP_TEST_WANT_LONG_INT_TYPE";
        if(type == "double") return "FP_TEST_WANT_DOUBLE_TYPE";
        if(type == "MpfrFloat") return "FP_TEST_WANT_MPFR_FLOAT_TYPE";
        if(type == "GmpInt") return "FP_TEST_WANT_GMP_INT_TYPE";
        if(type == "std::complex<double>") return "FP_TEST_WANT_COMPLEX_DOUBLE_TYPE";
        if(type == "std::complex<float>") return "FP_TEST_WANT_COMPLEX_FLOAT_TYPE";
        if(type == "std::complex<long double>") return "FP_TEST_WANT_COMPLEX_LONG_DOUBLE_TYPE";
        return std::string();
    }
    std::string GetTypeForDefine(const std::string& def)
    {
        if(def == "FP_TEST_WANT_FLOAT_TYPE") return "float";
        if(def == "FP_TEST_WANT_LONG_DOUBLE_TYPE") return "long double";
        if(def == "FP_TEST_WANT_LONG_INT_TYPE") return "long";
        if(def == "FP_TEST_WANT_DOUBLE_TYPE") return "double";
        if(def == "FP_TEST_WANT_MPFR_FLOAT_TYPE") return "MpfrFloat";
        if(def == "FP_TEST_WANT_GMP_INT_TYPE") return "GmpInt";
        if(def == "FP_TEST_WANT_COMPLEX_DOUBLE_TYPE") return "std::complex<double>";
        if(def == "FP_TEST_WANT_COMPLEX_FLOAT_TYPE") return "std::complex<float>";
        if(def == "FP_TEST_WANT_COMPLEX_LONG_DOUBLE_TYPE") return "std::complex<long double>";
        return "double";
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
        else if(typecode == "cd")
            return ("std::complex<double>");
        else if(typecode == "cf")
            return ("std::complex<float>");
        else if(typecode == "cld")
            return ("std::complex<long double>");
        return typecode;
    }
    unsigned GetLimitMaskFor(const std::string& typecode)
    {
        if(typecode ==  "d") return 16;
        if(typecode ==  "f" || typecode ==  "ld") return 1;
        if(typecode ==  "li" || typecode ==  "gi") return 2;
        if(typecode ==  "mf") return 4;
        if(typecode ==  "cf" || typecode ==  "cd" || typecode ==  "cld") return 8;
        return 0;
    }

    struct TestData
    {
        std::string IfDef{};

        std::string FuncString{}, ParamString{};
        unsigned ParamAmount = 0;
        std::string ParamValueRanges{};
        bool UseDegrees = false;
        bool UseAbsImag = false;
        std::string TestFuncName{}, TestName{};
        std::set<std::string> DataTypes{};

        TestData() {}
    };

    typedef std::vector<TestData> TestCollection;

    std::map<std::string/*datatype*/,
             TestCollection> tests;

    std::map<std::string, std::string/*list of tests*/> define_sections;

    struct function_info
    {
        unsigned                                     type_limit_mask;
        std::string                                  code;
        std::unordered_map<std::string, std::string> used_constants;
    };

    std::unordered_map<std::string/*TestFuncName*/, function_info> all_functions;

    std::string TranslateString(const std::string& str);

    static const char cbuf[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";

    template<typename CharT>
    static void
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

    void ListTests(std::ostream& outStream)
    {
        unsigned DefineCounter=0;
        std::map<std::string, std::string> TestDefines;

        for(std::map<std::string, TestCollection>::const_iterator
            i = tests.begin();
            i != tests.end();
            ++i)
        {
            std::ostringstream listbuffer;

            const std::string& type = i->first;
            std::string defines = GetDefinesFor(type);
            size_t n_tests         = i->second.size();

            listbuffer << "\n";

            listbuffer <<
                "template<>\n"
                "struct RegressionTests<" << type << ">\n"
                "{\n"
                "    static constexpr const TestType<" << type << "> Tests[]";
            if(n_tests == 0)
            {
                listbuffer <<
                    " = { };\n";
            }
            else
            {
                listbuffer << " =\n{\n";
                for(size_t a=0; a<n_tests; ++a)
                {
                    const TestData& testdata = i->second[a];

                    std::ostringstream linebuf;

                    std::ostringstream ranges;
                    const char* rangesdata = testdata.ParamValueRanges.c_str();
                    while(*rangesdata)
                    {
                        char* endptr = 0;
                        std::strtod(rangesdata, &endptr);
                        if(endptr && endptr != rangesdata)
                        {
                            /* Complex number support: */
                            if(*endptr == 'i' || *endptr == 'I')
                                ++endptr;
                            else if(*endptr == '+' || *endptr == '-')
                            {
                                std::strtod(endptr, &endptr);
                                if(*endptr == 'i' || *endptr == 'I') ++endptr;
                            }
                            while(*rangesdata == ' ') ++rangesdata; // skip spaces
                            ranges << '"' << std::string(rangesdata,endptr-rangesdata) << '"';
                            rangesdata = endptr;
                        }
                        else
                            ranges << *rangesdata++;
                    }

                    int n_duplicates = (int)testdata.DataTypes.size();

                    bool has_dbl = false;
                    bool has_long = false;

                    if(testdata.DataTypes.find("double") != testdata.DataTypes.end())
                        has_dbl = true;
                    if(testdata.DataTypes.find("long") != testdata.DataTypes.end())
                        has_long = true;
                    if(testdata.DataTypes.find("GmpInt") != testdata.DataTypes.end())
                        has_long = true;

                    linebuf
                        << "    { " << testdata.ParamAmount
                        << ", " << ranges.str()
                        << ", TestIndex_t::" << testdata.TestFuncName
                        << ", " << (testdata.UseDegrees ? "true" : "false")
                        << ", " << (has_dbl ? "true" : "false")
                        << ", " << (has_long ? "true" : "false")
                        << ", " << (testdata.UseAbsImag ? "true" : "false")
                        << ", " << TranslateString(testdata.ParamString)
                        << ", " << TranslateString(testdata.TestName)
                        << ", " << TranslateString(testdata.FuncString)
                        << " },\n";

                    if(!testdata.IfDef.empty())
                        listbuffer << "#if " << testdata.IfDef << "\n";

                    if(n_duplicates > 1)
                    {
                        std::string teststr(linebuf.str());
                        std::map<std::string, std::string>::iterator
                            i = TestDefines.lower_bound(teststr);
                        if(i == TestDefines.end() || i->first != teststr)
                        {
                            char MacroName[32], *m = MacroName;
                            unsigned p = DefineCounter++;
                            *m++ = "STUWY"[p%5]; p/=5;
                            for(; p != 0; p /= 63)
                                *m++ = cbuf[p % 63];
                            *m++ = '\0';
                            TestDefines.insert(i, std::pair<std::string,std::string>
                                (teststr, MacroName));

                            str_replace_inplace(teststr,
                                std::string("\n"), std::string(" "));
                            /*while(!teststr.empty() && (teststr[teststr.size()-1]==' '
                                                    || teststr[teststr.size()-1]==','))
                                teststr.erase(teststr.size()-1);
                            */
                            outStream << "#define " << MacroName << " " << teststr << "\n";
                            listbuffer << MacroName << "\n";
                        }
                        else
                            listbuffer << i->second << "\n";
                    }
                    else
                    {
                        listbuffer << linebuf.str();
                    }

                    if(!testdata.IfDef.empty())
                        listbuffer << "#endif /*" << testdata.IfDef << " */\n";
                }
                listbuffer << "};\n";
            }
            listbuffer << "};\n";
            listbuffer << "constexpr const TestType<" << type << "> RegressionTests<" << type << ">::Tests[];\n";

            define_sections[defines] += listbuffer.str();
        }
    }

    namespace
    {
        /* Reads an UTF8-encoded sequence which forms a valid identifier name from
           the given input string and returns its length.
        */
        unsigned readIdentifierCommon(const char* input)
        {
    #define FP_NO_TEST_FOR_FUNCTIONS
    #include "extrasrc/fp_identifier_parser.inc"
            return 0;
        }
    }

    void CompileFunction(const char*& funcstr, const std::string& eval_name,
                         std::ostream& codebuf,
                         unsigned& user_param_count,
                         std::unordered_map<std::string,std::string>& used_constants,
                         const std::unordered_map<std::string, std::string>& var_trans)
    {
        unsigned depth    = 0;
        unsigned brackets = 0;

        while(*funcstr && (*funcstr != '}' || depth>0) && (*funcstr != ',' || depth>0))
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
                unsigned first_index = ~0u;

                codebuf << "(";
                for(;;)
                {
                    while(std::isspace(*funcstr)) ++funcstr;
                    if(!*funcstr) break;
                    if(*funcstr == '}') { ++funcstr; break; }

                    if(first_index == ~0u)
                        first_index = user_param_count;

                    codebuf << "uparam[" << user_param_count++ << "]=(";

                    // Recursion
                    CompileFunction(
                        funcstr, eval_name,
                        codebuf,
                        user_param_count,
                        used_constants,
                        var_trans);

                    codebuf << "), ";
                    if(*funcstr == ',') ++funcstr;
                }

                if(first_index != ~0u)
                    codebuf << "uparam+" << first_index;
                else
                    codebuf << "nullptr";
                codebuf << "))";
                while(std::isspace(*funcstr)) ++funcstr;
                if(*funcstr == ')') ++funcstr;
            }
            else
            {
                if(*funcstr == '(') ++depth;
                if(*funcstr == ')') --depth;
                if(*funcstr == '{') ++depth;
                if(*funcstr == '}') --depth;
                if(*funcstr == '[') ++brackets;
                if(*funcstr == ']') --brackets;

                char prevchar = funcstr[-1];
                if(prevchar == ' ') prevchar = funcstr[-2];

                char* endptr = 0;
                if((*funcstr >= '0' && *funcstr <= '9')
                || *funcstr == '.'
                || (*funcstr == '-' && (prevchar == '('
                                     || prevchar == '+'
                                     || prevchar == '*'
                                     || prevchar == ','
                                     || prevchar == '?'
                                     || prevchar == ':'
                   )                   )
                  )
                    std::strtod(funcstr, &endptr);

                if(endptr && endptr != funcstr && !brackets)
                {
                    // Generate numeric literal
                    std::string literal(funcstr, endptr-funcstr);
                    std::string codename = "l"+literal;
                    str_replace_inplace(codename, std::string("."), std::string("_"));
                    str_replace_inplace(codename, std::string("+"), std::string("p"));
                    str_replace_inplace(codename, std::string("-"), std::string("m"));
                    used_constants[codename] = literal;
                    codebuf << "N(" << codename << ")";
                    funcstr = endptr;
                }
                else
                {
                    // Generate identifier
                    unsigned length = readIdentifierCommon(funcstr);
                    if(length)
                    {
                        std::string identifier(funcstr, funcstr+length);
                        auto i = var_trans.find(identifier);
                        if(i != var_trans.end())
                        {
                            identifier = i->second;
                        }
                        if(identifier == "i") identifier = "img_unit<Value_t>()";
                        codebuf << identifier;
                        funcstr += length;
                    }
                    else
                        codebuf << *funcstr++;
                }
            }
        }
    }

    //std::string StringBuffer;
    std::string TranslateString(const std::string& str)
    {
        std::string val = str;
        str_replace_inplace(val, std::string("/"), std::string("\"\"/\"\""));
        str_replace_inplace(val, std::string("+"), std::string("\"\"+\"\""));
        str_replace_inplace(val, std::string("*"), std::string("\"\"*\"\""));
        str_replace_inplace(val, std::string("x"), std::string("\"\"x\"\""));
        str_replace_inplace(val, std::string("&"), std::string("\"\"&\"\""));
        str_replace_inplace(val, std::string("("), std::string("\"\"(\"\""));
        str_replace_inplace(val, std::string(")"), std::string("\"\")\"\""));
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
            methodname[0] = (char)std::toupper(methodname[0]);
        return std::make_pair(classname, methodname);
    #endif
    }

    void CompileTest(const std::string& testname, FILE* fp,
                     unsigned&           user_param_count)
    {
        std::string linebuf;

        TestData test;
        std::set<std::string> DataTypes;

        test.TestName = testname;
        str_replace_inplace(test.TestName, std::string("tests/"), std::string(""));

        std::unordered_map<std::string, std::string> var_trans;

        unsigned datatype_limit_mask = 255;

        unsigned linenumber = 0;
        char Buf[4096]={0};
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
                case 'I': // imag: abs
                    test.UseAbsImag = true;
                    break;
                case 'T': // list of applicable types
                    if(valuepos)
                    {
                        datatype_limit_mask = 0;
                        for(;;)
                        {
                            while(*valuepos == ' ') ++valuepos;
                            if(!*valuepos) break;

                            const char* space = std::strchr(valuepos, ' ');
                            if(!space) space = std::strrchr(valuepos, '\0');
                            std::string type(valuepos, space);

                            datatype_limit_mask |= GetLimitMaskFor(type);
                            std::string cpptype = GetTypeFor(type);
                            DataTypes.insert(cpptype);

                            valuepos = space;
                        }

                        /* FLT: float, double, long double, complex types?
                         * INT: integer
                         * MPFR: Mpfr?
                         * Options:
                         *    FLT only
                         *    INT only
                         *    MPFR only
                         *    FLT + MPFR
                         *    FLT + INT + MPFR
                         */
                        test.DataTypes = DataTypes; // Don't move
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

                        for(size_t a=0; a<vars.size(); ++a)
                            var_trans[vars[a]] = "vars[" + std::to_string(a) + "]";
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
                        std::pair<std::string,std::string> funcname = MakeFuncName(test.TestName);
                        test.TestFuncName = funcname.first+"_"+funcname.second;

                        std::ostringstream codebuf;
                        std::unordered_map<std::string,std::string> used_constants;

                        const char* valuepos_1 = valuepos;
                        CompileFunction(
                            valuepos_1, funcname.second,
                            codebuf,
                            user_param_count,
                            used_constants,
                            var_trans);

                        std::string code = codebuf.str();
                        all_functions.emplace(test.TestFuncName,
                            function_info{datatype_limit_mask, std::move(code), std::move(used_constants)}
                                              );
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
    static bool natcomp(const std::string& a, const std::string& b)
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

    static bool WildMatch(const char *pattern, const char *what)
    {
        for(; *what || *pattern; ++what, ++pattern)
            if(*pattern == '*')
            {
                while(*++pattern == '*') {}
                for(; *what; ++what)
                    if(WildMatch(pattern, what))
                        return true;
                return !*pattern;
            }
            else if(*pattern != '?' && *pattern != *what)
                return false;
        return true;
    }
}

int main(int argc, char* argv[])
{
    const char* outputFileName = 0;
    std::ofstream outputFileStream;

    std::ostringstream out;

    std::vector<std::string> files, ignore_patterns;

    for(int a=1; a<argc; ++a)
    {
        if(std::strcmp(argv[a], "-o") == 0)
        {
            if(++a == argc)
            {
                std::cerr << "make_tests: Expected output file name after -o\n";
                return 1;
            }
            outputFileName = argv[a];
            outputFileStream.open(argv[a]);
            if(!outputFileStream)
            {
                std::cerr << "make_tests: Could not write to " << argv[a] << "\n";
                return 1;
            }
            continue;
        }
        else if(std::strcmp(argv[a], "--ignore") == 0)
        {
            if(++a == argc)
            {
                std::cerr << "make_tests: Expected ignore-pattern after --ignore\n";
                return 1;
            }
            ignore_patterns.push_back(argv[a]);
            continue;
        }

        std::string fn ( argv[a] );
        if(fn.empty()) continue;

        files.push_back(std::move(fn));
    }

    files.erase(
        std::remove_if(files.begin(), files.end(), [&ignore_patterns](const std::string& s)
            {
                for(const auto& pattern: ignore_patterns)
                    if(WildMatch(pattern.c_str(), s.c_str()))
                        return true;
                return false;
            }),
        files.end());
    std::sort(files.begin(), files.end(), natcomp);

    std::ostream& outStream = outputFileName ? outputFileStream : std::cout;
    //const char* outStreamName = outputFileName ? outputFileName : "<stdout>";

    unsigned           user_param_count{};

    for(const auto& filename: files)
    {
        FILE* fp = std::fopen(filename.c_str(), "rt");
        if(!fp)
        {
            std::perror(filename.c_str());
            continue;
        }
        CompileTest(filename, fp, user_param_count);
        fclose(fp);
    }

    std::string declbuf;
    if(user_param_count)
        declbuf = "    Value_t uparam[" + std::to_string(user_param_count) + "];\n";

    auto gen_funcs = [&](std::function<
        std::pair<bool,std::string>(
            const std::pair<const std::string, function_info>&
                                   )> gen)
    {
        std::map<std::string/*code*/, std::set<std::string/*case*/>> cases;
        for(const auto& f: all_functions)
        {
            std::pair<bool, std::string> what = gen(f);
            if(what.first)
                cases[what.second].insert(f.first);
        }
        for(auto& what: cases)
        {
            for(auto& c: what.second)
                out << "case TestIndex_t::" << c << ":\n";
            out << "    " << what.first << '\n';
        }
    };

    static const char lesser_opt_begin[] = ""
    //    "#pragma GCC push_options\n"
    //    "#pragma GCC optimize   (\"O1\")\n"
    ;
    static const char lesser_opt_end[] = ""
    //    "#pragma GCC pop_options\n"
    ;

    out << "}enum class TestIndex_t {\n";
    out <<     "    defaultTypeTest,\n";
    for(const auto& f: all_functions)
    {
        out << "    " << f.first << ",\n";
    }
    out << "};namespace {\n";

    out <<
        "template<typename Value_t>\n"
        "Value_t img_unit()\n"
        "{\n"
        "    return FUNCTIONPARSERTYPES::fp_sqrt((Value_t)(-1));\n"
        "}\n";

    out <<
        "template<typename Value_t,\n"
        "    bool d = std::is_same<Value_t, double>::value,\n"
        "    bool i = (FUNCTIONPARSERTYPES::IsIntType<Value_t>::value\n"
        "  #ifdef FP_SUPPORT_GMP_INT_TYPE\n"
        "          || std::is_same<Value_t, GmpInt>::value\n"
        "  #endif\n"
        "    ),\n"
        "  #ifdef FP_SUPPORT_MPFR_FLOAT_TYPE\n"
        "    bool m = std::is_same<Value_t, MpfrFloat>::value,\n"
        "  #else\n"
        "    bool m = false,\n"
        "  #endif\n"
        "    bool c = FUNCTIONPARSERTYPES::IsComplexType<Value_t>::value>\n"
        "struct fp_type_mask : public std::integral_constant<unsigned, (d?16:(m?4:(c?8:(i?2:(1))))) > { };\n";

    out <<
        "template<typename Value_t>\n"
        "struct const_container\n"
        "{\n";

    {std::unordered_map<std::string, std::string> all_const;
    for(const auto& f: all_functions)
        for(auto& c: f.second.used_constants)
            all_const[c.first] = c.second;
      for(auto& c: all_const)
      {
        std::string lit = c.second;
        out << "    const Value_t " << c.first << " = static_cast<Value_t>(" << lit << "l);\n";
      }
    }

    out <<
        "};\n"
        "#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE\n"
        "template<>\n"
        "struct const_container<MpfrFloat>\n"
        "{\n";

    {std::unordered_map<std::string, std::string> all_const;
    for(const auto& f: all_functions)
        if(f.second.type_limit_mask & 4) // mpfr?
            for(auto& c: f.second.used_constants)
                all_const[c.first] = c.second;
    for(auto& c: all_const)
        out << "    const MpfrFloat " << c.first << "{\"" << c.second << "\", nullptr};\n";
    }
    out <<
        "};\n"
        "#endif\n"
        "\n"
        "template<typename Value_t>\n"
        "const_container<Value_t>& TestConst()\n"
        "{\n"
        "    static const_container<Value_t> container;\n"
        "    return container;\n"
        "}\n"
        "#define N(name) TestConst<Value_t>().name\n";

    ListTests(out);

    all_functions.emplace("defaultTypeTest",
        function_info{16u, "OptimizerTests::evaluateFunction(vars)", {}});

    std::string else_branch =
        "  #if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L\n"
        "    std::unreachable();\n"
        "  #else\n"
        "    #ifdef __GNUC__\n"
        "      __builtin_unreachable();\n"
        "    #elif defined(_MSC_VER) /* MSVC */\n"
        "      __assume(false);\n"
        "    #else\n"
        "      return Value_t{};\n"
        "    #endif\n"
        "  #endif\n";

    out <<
        lesser_opt_begin <<
        "#if defined(__cpp_if_constexpr) && __cpp_if_constexpr >= 201606L\n"
        "template<typename Value_t>\n"
        "static Value_t evaluate_test(TestIndex_t which, const Value_t* vars)\n"
        "{\n"
        "    constexpr unsigned type_mask = fp_type_mask<Value_t>::value;\n"
        "    using namespace FUNCTIONPARSERTYPES;\n" << declbuf <<
        "    switch(which)\n"
        "    {\n";
    gen_funcs([&](const std::pair<const std::string, function_info>& f)
    {
        auto& info = f.second;
        return std::make_pair(true,
            "if constexpr(type_mask&" + std::to_string(info.type_limit_mask)+")" +
            "return " + info.code + "; else break;");
    });
    out <<
        "        default: break;\n"
        "    }\n" << else_branch <<
        "}\n";
    out <<
        "#else\n"; // "if constexpr" is not available

    out <<
        "template<typename Value_t, unsigned mask = fp_type_mask<Value_t>::value>\n"
        "struct evaluator { };\n";
    for(unsigned m=1; m<=16; m<<=1)
    {
        out <<
            "template<typename Value_t> struct evaluator<Value_t," << m << ">\n"
            "{ Value_t operator()(TestIndex_t which, const Value_t* vars) {\n"
            "    using namespace FUNCTIONPARSERTYPES;\n" << declbuf <<
            "    switch(which) {\n";
        gen_funcs([&](const std::pair<const std::string, function_info>& f)
        {
            auto& info = f.second;
            return std::make_pair(bool(info.type_limit_mask & m), "return " + info.code + ";");
        });
        out <<
            "    default: break;\n"
            " }\n" << else_branch << "} };\n";
    }
    out <<
        "template<typename Value_t>\n"
        "static Value_t evaluate_test(TestIndex_t which, const Value_t* vars)\n"
        "{\n"
        "    return evaluator<Value_t>{}(which, vars);\n"
        "}\n";

    out <<
        "#endif\n" << lesser_opt_end; // End of "if constexpr" implementation

    for(const auto& section: define_sections)
    {
        const std::string& define   = section.first;
        const std::string& test_list = section.second;

        if(!define.empty())
            out << "\n#ifdef " << define << "\n";

        out << test_list;

        if(!define.empty())
            out << "#endif /*" << define << " */\n";
    }

    //MakeStringBuffer(out);
    //outStream << "extern const char ts[" << StringBuffer.size() << "];\n";

    outStream << out.str();

    return 0;
}
