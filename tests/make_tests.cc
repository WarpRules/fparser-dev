#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include <cstdio>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <unordered_map>

namespace
{
#define DEFINE_TYPES(o) \
    o(d,   1, double,                     FP_TEST_WANT_DOUBLE_TYPE) \
    o(f,   1, float,                      FP_TEST_WANT_FLOAT_TYPE) \
    o(ld,  1, long double,                FP_TEST_WANT_LONG_DOUBLE_TYPE) \
    o(li,  2, long,                       FP_TEST_WANT_LONG_INT_TYPE) \
    o(gi,  2, GmpInt,                     FP_TEST_WANT_GMP_INT_TYPE) \
    o(mf,  4, MpfrFloat,                  FP_TEST_WANT_MPFR_FLOAT_TYPE) \
    o(cf,  8, std::complex<float>,        FP_TEST_WANT_COMPLEX_FLOAT_TYPE) \
    o(cd,  8, std::complex<double>,       FP_TEST_WANT_COMPLEX_DOUBLE_TYPE) \
    o(cld, 8, std::complex<long double>,  FP_TEST_WANT_COMPLEX_LONG_DOUBLE_TYPE) \

    constexpr unsigned highest_mask = (1+(0
        #define o(code,mask,typename,def) |mask
        DEFINE_TYPES(o)
        #undef o
    )) >> 1;  // e.g. (1+(0|1|2|4))>>1 = (1+7)>>1 = 8>>1 = 4

    [[nodiscard]] std::string GetDefinesForCpptype(const std::string& type)
    {
        #define o(code,mask,typename,def) if(type == #typename) return #def;
        DEFINE_TYPES(o)
        #undef o
        return std::string();
    }
    [[nodiscard]] std::string GetTypeForCode(const std::string& typecode)
    {
        #define o(code,mask,typename,def) if(typecode == #code) return #typename;
        DEFINE_TYPES(o)
        #undef o
        return typecode;
    }
    [[nodiscard]] unsigned GetLimitMaskForCode(const std::string& typecode)
    {
        #define o(code,mask,typename,def) if(typecode == #code) return mask;
        DEFINE_TYPES(o)
        #undef o
        return 0;
    }
    [[nodiscard]] std::string GetTestCodeForMask(unsigned mask, const std::string& type)
    {
        std::ostringstream result;
        result << "(false\n";
        #define o(code,msk,typename,def) \
            if(mask & msk) \
                result << "#ifdef " << #def << "\n" \
                          " || std::is_same<" << type << ", " << #typename << ">::value\n" \
                          "#endif\n";
        DEFINE_TYPES(o)
        #undef o
        result << ')';
        return result.str();
    }

    struct TestData
    {
        std::string FuncString{}, ParamString{};
        unsigned ParamAmount = 0;
        std::string ParamValueRanges{};
        bool UseDegrees = false;
        bool UseAbsImag = false;
        std::string TestName{};
        std::set<std::string> DataTypes{};

        TestData() {}
    };
    struct FunctionInfo
    {
        unsigned                                     type_limit_mask;
        std::string                                  code;
        std::unordered_map<std::string, std::string> used_constants;
    };

    std::map<std::string/*c++ datatype*/, std::vector<TestData>> tests;
    std::unordered_map<std::string/*TestName*/, FunctionInfo> all_functions;

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

    [[nodiscard]] std::string ListTests(std::ostream& out)
    {
        std::map<std::string, std::size_t> test_index;
        std::ostringstream test_list;
        std::size_t        test_counter = 0;

        std::map<std::string, std::string> tests_per_defs;

        for(auto& test: tests)
        {
            const std::string& type = test.first;
            const auto&  collection = test.second;
            size_t n_tests = collection.size();

            std::ostringstream listbuffer;

            std::string defines = GetDefinesForCpptype(type);

            listbuffer << "\n";

            unsigned list_width_counter = 0;
            listbuffer <<
                "template<>\n"
                "struct RegressionTests<" << type << ">\n"
                "{\n"
                "    static constexpr const unsigned short Tests[]";
            if(n_tests == 0)
            {
                listbuffer <<
                    " = { };\n";
            }
            else
            {
                listbuffer << " =\n        { ";
                for(const TestData& testdata: collection)
                {
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
                            ranges << std::string(rangesdata,endptr-rangesdata);
                            rangesdata = endptr;
                        }
                        else
                            ranges << *rangesdata++;
                    }

                    bool has_dbl = false;
                    bool has_long = false;

                    if(testdata.DataTypes.find("double") != testdata.DataTypes.end())
                        has_dbl = true;
                    if(testdata.DataTypes.find("long") != testdata.DataTypes.end())
                        has_long = true;
                    if(testdata.DataTypes.find("GmpInt") != testdata.DataTypes.end())
                        has_long = true;

                    linebuf
                        << "o("
                        <<        (testdata.UseDegrees ? 'T' : 'F')
                        <<        (has_dbl ? 'T' : 'F')
                        <<        (has_long ? 'T' : 'F')
                        <<        (testdata.UseAbsImag ? 'T' : 'F')
                        << ","  << std::setw(46) << std::left << testdata.TestName
                        << ","  << std::setw(20) << std::right << ranges.str()
                        << "," << testdata.FuncString
                        << ", " << '"' << testdata.ParamString << '"'
                        << "," << testdata.ParamAmount
                        << ")";

                    if(list_width_counter >= 19)
                    {
                        listbuffer << "\n          ";;
                        list_width_counter = 0;
                    }

                    std::string teststr(linebuf.str());
                    auto i = test_index.lower_bound(teststr);
                    if(i == test_index.end() || i->first != teststr)
                    {
                        test_list << "/*" << std::setw(4) << test_counter << "*/ " << teststr << " \\\n";
                        test_index.emplace_hint(i, teststr, test_counter);
                        listbuffer << std::setw(4) << test_counter << ',';
                        ++test_counter;
                    }
                    else
                        listbuffer << std::setw(4) << i->second << ',';
                    ++list_width_counter;
                }
                listbuffer << "};\n";
            }
            listbuffer << "};\n";
            listbuffer << "constexpr const unsigned short RegressionTests<" << type << ">::Tests[];\n";

            tests_per_defs[defines] += listbuffer.str();
        }
        out << R"(
#define FP_LIST_ALL_TESTS(o) \
)" << test_list.str() << R"(/* end of list */
)";
        std::ostringstream defs;
        for(const auto& section: tests_per_defs)
        {
            const std::string& define    = section.first;
            if(!define.empty())
                defs << "\n#ifdef " << define << "\n";
            defs << section.second;
            if(!define.empty())
                defs << "#endif /*" << define << " */\n";
        }
        return defs.str();
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

    void CompileFunction(const char*& funcstr,
                         std::ostream& codebuf,
                         unsigned& user_param_count,
                         std::unordered_map<std::string,std::string>& used_constants,
                         const std::unordered_map<std::string, std::string>& var_trans)
    {
        unsigned depth    = 0;
        unsigned brackets = 0;

        while(*funcstr && (*funcstr != '}' || depth>0) && (*funcstr != ',' || depth>0))
        {
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
                        funcstr,
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
                    codebuf << "c." << codename;
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
                        if(identifier == "i") identifier = "c.i";
                        codebuf << identifier;
                        funcstr += length;
                    }
                    else
                        codebuf << *funcstr++;
                }
            }
        }
    }

    /*std::string MakeFuncName(const std::string& testname)
    {
    #if 0
        static unsigned counter = 0;
        std::string result = "qZ";
        for(unsigned p = counter++; p != 0; p /= 63)
            result += cbuf[p % 63];
        return result;
    #else
        std::string base = "t" + testname;

        size_t p = base.rfind('/');
        std::string classname = base.substr(0, p);

        classname = base.substr(0,3) + base.substr(base.find('/'));

        std::string methodname = base.substr(p+1);
        str_replace_inplace(classname, std::string("/"), std::string("_"));
        str_replace_inplace(methodname, std::string("/"), std::string("_"));
        // Change the method name to prevent clashes with
        // with reserved words or the any namespace
        if(isdigit(methodname[0]))
            methodname.insert(0, "t");
        else
            methodname[0] = (char)std::toupper(methodname[0]);
        return classname + "_" + methodname;
    #endif
    }*/

    void CompileTest(const std::string& testname, FILE* fp,
                     unsigned&           user_param_count)
    {
        std::string linebuf;

        TestData test;
        std::set<std::string> DataTypes;

        test.TestName = testname;
        str_replace_inplace(test.TestName, std::string("tests/"), std::string(""));

        std::unordered_map<std::string, std::string> var_trans;

        unsigned datatype_limit_mask = ~0u;

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
                case 'D': // DEG
                    test.UseDegrees = true;
                    break;
                case 'I': // IMAG: ABS
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

                            datatype_limit_mask |= GetLimitMaskForCode(type);
                            std::string cpptype = GetTypeForCode(type);
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
                        test.TestName = test.TestName; //MakeFuncName(test.TestName);

                        std::ostringstream codebuf;
                        std::unordered_map<std::string,std::string> used_constants;

                        const char* valuepos_1 = valuepos;
                        CompileFunction(
                            valuepos_1,
                            codebuf,
                            user_param_count,
                            used_constants,
                            var_trans);

                        std::string code = codebuf.str();
                        all_functions.emplace(test.TestName,
                            FunctionInfo{datatype_limit_mask, std::move(code), std::move(used_constants)}
                                              );
                    }
                    break;
            }
        }

        for(auto type: DataTypes)
            tests[type].push_back(test);
    }

    /* Asciibetical comparator, with in-string integer values sorted naturally */
    [[nodiscard]] bool natcomp(const std::string& a, const std::string& b)
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

    [[nodiscard]] bool WildMatch(const char *pattern, const char *what)
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

    std::unordered_map<std::string, unsigned> test_index;
    for(auto& coll: tests)
        for(auto& test: coll.second)
            test_index.emplace(test.TestName, test_index.size());

    auto gen_funcs = [&](
        unsigned m,
        std::string (*gen)(unsigned, const std::string&, const FunctionInfo&),
        void        (*makecase)(std::ostream&,
                                const std::string& /*caseno*/, const std::string& /* code */)
                        )
    {
        constexpr unsigned customtest_index = ~0u;
        std::map<std::string/*code*/, std::set<unsigned/*case*/>> cases;
        for(const auto& f: all_functions)
        {
            std::string what = gen(m, f.first, f.second);
            if(what.empty()) continue;

            auto i = test_index.find(f.first);
            unsigned caseno = 0;
            if(i == test_index.end())
            {
                if(f.first == "defaultTypeTest")
                    caseno = customtest_index;
                else
                    { std::cerr << "Test " << f.first << " not found\n"; }
            }
            else
                caseno = i->second;
            cases[what].insert(caseno);
        }
        for(auto& what: cases)
            for(auto& c: what.second)
                if(c == customtest_index)
                    makecase(out, "customtest_index", what.first);
                else
                    makecase(out, std::to_string(c), what.first);
    };

    all_functions.emplace("defaultTypeTest",
        FunctionInfo{GetLimitMaskForCode("d"), "OptimizerTests::evaluateFunction(vars)", {}});

    static const char lesser_opt_begin[] = ""
        "#pragma GCC push_options\n"
        "#pragma GCC optimize   (\"O0\")\n"
    ;
    static const char lesser_opt_end[] = ""
        "#pragma GCC pop_options\n"
    ;
    static const char unreachable[] = R"(
    unreachable_helper();
    return Value_t{};
)";

    std::string test_tables = ListTests(out);

    gen_funcs(0, [](unsigned, const std::string&, const FunctionInfo& info)
    {
        return info.code;
    }, [](std::ostream& out, const std::string& caseno, const std::string& code)
    {
        std::ostringstream temp;
        temp << 'T' << caseno;
        out << "#define " << std::setw(5) << temp.str()
            << "(_,__)_ " << code << " __\n";
    });

    out << R"(
#define FP_LIST_ALL_CONST()";
    for(unsigned m=1; m<=highest_mask; m*=2)
    {
        if(m>1) out << ',';
        out << 'o' << m;
    }
    out << ") \\\n";
    {std::unordered_map<std::string, std::pair<unsigned,std::string>> all_const;
    for(const auto& f: all_functions)
        for(auto& c: f.second.used_constants)
            all_const.emplace(c.first, std::make_pair(0,c.second)).first->second.first |= f.second.type_limit_mask;
    for(auto& c: all_const)
    {
        for(unsigned m=1; m<=highest_mask; m*=2)
            if(c.second.first & m)
                out << " o" << m << "(" << std::setw(8) << c.first << ',' << std::setw(7) << c.second.second << ")";
        out << " \\\n";
    }}

    for(unsigned m=1; m<=highest_mask; m*=2)
    {
        bool complex = false;
        #define o(code,mask,typename,def) \
            if(mask == m && std::string(#code)[0] == 'c') \
                complex = true;
        DEFINE_TYPES(o)
        #undef o
        if(complex)
            out << " o" << m << "(i, Value_t(0,1)+(Value_t)0) \\\n";
    }
out << R"(/* end of list */
}/*break out from anonymous namespace*/

const TestType AllTests[] =
{
#define q(c) (c=='T'||c=='Y')
#define o(opt,testname, min,max,step, funcstring, paramstr, nparams) \
    { #testname,#funcstring,paramstr, #min,#max,#step, nparams, q(#opt[0]),q(#opt[1]),q(#opt[2]),q(#opt[3]) },
FP_LIST_ALL_TESTS(o)
#undef o
#undef q
};
namespace {

[[noreturn]] inline void unreachable_helper()
{
  #if defined(__cpp_lib_unreachable) && __cpp_lib_unreachable >= 202202L
    std::unreachable();
  #else
   #ifdef __GNUC__ /* GCC, Clang, ICC */
    __builtin_unreachable();
   #elif defined(_MSC_VER) /* MSVC */
    __assume(false);
   #endif
  #endif
}

template<typename Value_t>
struct fp_type_mask : public std::integral_constant<unsigned, 0)";
for(unsigned m=1; m<=highest_mask; m<<=1)
    out << "\n    | (" << m << "*int(" << GetTestCodeForMask(m, "Value_t") << "))";
out << R"(> {};

#define _(name,lit)
template<typename Value_t, unsigned n = fp_type_mask<Value_t>::value>
struct const_container {};
#define _(name,lit)
)";
for(unsigned m=1; m<=highest_mask; m<<=1)
{
    if(m == GetLimitMaskForCode("mf"))
        out << "#define o(name,lit) const MpfrFloat name{#lit, nullptr};\n";
    else
        out << "#define o(name,lit) const Value_t name = static_cast<Value_t>(lit##l);\n";
    out << "#if(0";
#define o(code,mask,typename,def) if(m == mask) out << "||defined(" << #def << ")";
    DEFINE_TYPES(o)
#undef o
    out << R"()
template<typename Value_t> struct const_container<Value_t,)" << m << "> { FP_LIST_ALL_CONST(";
    for(unsigned n=1; n<=highest_mask; n<<=1)
    {
        if(n>1) out << ',';
        out << ((n==m) ? 'o' : '_');
    }
    out << ") };\n"
          "#endif\n"
          "#undef o\n\n";
}
    out << "#undef _\n" << lesser_opt_begin << R"(
#if defined(__cpp_if_constexpr) && __cpp_if_constexpr >= 201606L
template<typename Value_t, unsigned type_mask = fp_type_mask<Value_t>::value>
static Value_t evaluate_test(unsigned which, const Value_t* vars)
{
    static const_container<Value_t> c;
    [[maybe_unused]] Value_t uparam[)" << user_param_count << R"(];
    using namespace FUNCTIONPARSERTYPES;
    switch(which)
    {
        #define M(n,m) T##n(case n:if constexpr(type_mask&m)return,;else break;)
)";
    gen_funcs(0, [](unsigned, const std::string&, const FunctionInfo& info)
    {
        return
            "if constexpr(type_mask&" + std::to_string(info.type_limit_mask)+")" +
            "return " + info.code + "; else break;";
    }, [](std::ostream& out, const std::string& caseno, const std::string& code)
    {
        static unsigned counter = 0;
        if(counter == 10) { out << '\n'; counter = 0; }
        auto p  = code.find('&');
        auto p2 = code.find(')');
        std::ostringstream temp;
        temp << "M(" << caseno  << ',' << code.substr(p+1,p2-p);
        out << std::left << std::setw(10) << temp.str();
        ++counter;
    });
    out << R"(
        #undef M
default: break;
    })" << unreachable << R"(
}
#else // "if constexpr" is not available
template<typename Value_t, unsigned type_mask = fp_type_mask<Value_t>::value>
struct evaluator { };

#define M(n) T##n(case n:return,;)
)";
    for(unsigned m=1; m<=highest_mask; m<<=1)
    {
        out << "#if(0";
#define o(code,mask,typename,def) if(m == mask) out << "||defined(" << #def << ")";
        DEFINE_TYPES(o)
#undef o
        out << ")";
        out << R"(
template<typename Value_t> struct evaluator<Value_t,)" << m << R"(>
{ static Value_t calc(unsigned which, const Value_t* vars)
{
    static const_container<Value_t> c;
    [[maybe_unused]] Value_t uparam[)" << user_param_count << R"(];
    using namespace FUNCTIONPARSERTYPES;
    switch(which) {
)";
        gen_funcs(m, [](unsigned m, const std::string&, const FunctionInfo& info)
        {
            return (info.type_limit_mask & m) ? info.code : std::string{};
        }, [](std::ostream& out, const std::string& caseno, const std::string& /*code*/)
        {
            static unsigned counter = 0;
            if(counter == 15) { out << '\n'; counter = 0; }
            std::ostringstream temp;
            temp << "M(" << caseno << ')';
            out << std::left << std::setw(7) << temp.str();
            ++counter;
        });
        out << R"(
default: break;
})" << unreachable << "} };\n";
        out << "#endif\n";
    }
    out << R"(
#undef M
template<typename Value_t>
static Value_t evaluate_test(unsigned which, const Value_t* vars)
{
    return evaluator<Value_t>::calc(which, vars);
}
#endif
)" << lesser_opt_end; // End of "if constexpr" implementation

    out << test_tables;

    //MakeStringBuffer(out);
    //outStream << "extern const char ts[" << StringBuffer.size() << "];\n";

    outStream << out.str();

    return 0;
}
