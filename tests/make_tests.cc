#include <string>
#include <sstream>
#include <cctype>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>

#include "stringutil.hh"

namespace
{
#define LITERAL_MAKER_DEFAULT "const Value_t name = static_cast<Value_t>(lit##l);"
#define LITERAL_MAKER_MPFR    "const MpfrFloat name{#lit, nullptr};"
#define DEFINE_TYPES(o) \
    o(d,   1, double,                     FP_TEST_WANT_DOUBLE_TYPE, LITERAL_MAKER_DEFAULT) \
    o(f,   2, float,                      FP_TEST_WANT_FLOAT_TYPE, LITERAL_MAKER_DEFAULT) \
    o(ld,  4, long double,                FP_TEST_WANT_LONG_DOUBLE_TYPE, LITERAL_MAKER_DEFAULT) \
    o(li,  8, long,                       FP_TEST_WANT_LONG_INT_TYPE, LITERAL_MAKER_DEFAULT) \
    o(gi, 16, GmpInt,                     FP_TEST_WANT_GMP_INT_TYPE, LITERAL_MAKER_DEFAULT) \
    o(mf, 32, MpfrFloat,                  FP_TEST_WANT_MPFR_FLOAT_TYPE, LITERAL_MAKER_MPFR) \
    o(cld,64, std::complex<long double>,  FP_TEST_WANT_COMPLEX_LONG_DOUBLE_TYPE, LITERAL_MAKER_DEFAULT) \
    o(cf,128, std::complex<float>,        FP_TEST_WANT_COMPLEX_FLOAT_TYPE, LITERAL_MAKER_DEFAULT) \
    o(cd,256, std::complex<double>,       FP_TEST_WANT_COMPLEX_DOUBLE_TYPE, LITERAL_MAKER_DEFAULT) \

    constexpr unsigned highest_mask = (1+(0
        #define o(code,mask,typename,def,lit) |mask
        DEFINE_TYPES(o)
        #undef o
    )) >> 1;  // e.g. (1+(0|1|2|4))>>1 = (1+7)>>1 = 8>>1 = 4

    [[nodiscard]] std::string GetDefinesForCpptype(const std::string& type)
    {
        #define o(code,mask,typename,def,lit) if(type == #typename) return #def;
        DEFINE_TYPES(o)
        #undef o
        return std::string();
    }
    [[nodiscard]] std::string GetCpptypeForDefines(const std::string& defs)
    {
        #define o(code,mask,typename,def,lit) if(defs == #def) return #typename;
        DEFINE_TYPES(o)
        #undef o
        return std::string();
    }
    [[nodiscard]] std::string GetTypeForCode(const std::string& typecode)
    {
        #define o(code,mask,typename,def,lit) if(typecode == #code) return #typename;
        DEFINE_TYPES(o)
        #undef o
        return typecode;
    }
    [[nodiscard]] unsigned GetLimitMaskForCode(const std::string& typecode)
    {
        #define o(code,mask,typename,def,lit) if(typecode == #code) return mask;
        DEFINE_TYPES(o)
        #undef o
        return 0;
    }
    [[nodiscard]] std::string GetTestCodeForMask(unsigned mask, const std::string& type)
    {
        std::ostringstream result;
        result << "(false\n";
        #define o(code,msk,typename,def,lit) \
            if(mask & msk) \
                result << "#ifdef " #def "\n" \
                          " || std::is_same<" << type << ", " #typename ">::value\n" \
                          "#endif\n";
        DEFINE_TYPES(o)
        #undef o
        result << ')';
        return result.str();
    }
    [[nodiscard]] unsigned GetMaskForDefines(const std::string& defs)
    {
        #define o(code,mask,typename,def,lit) \
            if(defs == #def) return mask;
        DEFINE_TYPES(o)
        #undef o
        return 0;
    }

    struct TestData
    {
        std::string FuncString{}, ParamString{};
        unsigned ParamAmount = 0;
        std::string ParamValueRanges{};
        bool UseDegrees = false;
        bool UseAbsImag = false;
        std::string TestName{};
        std::unordered_set<std::string> DataTypes{};

        TestData() {}
    };
    struct FunctionInfo
    {
        unsigned                                     type_limit_mask;
        std::string                                  code;
        std::unordered_map<std::string, std::string> used_constants;
    };

    std::unordered_map<std::string/*c++ datatype*/, std::vector<TestData>> tests;
    std::unordered_map<std::string/*TestName*/, FunctionInfo> all_functions;

    [[nodiscard]] std::string ListTests(std::ostream& out, const std::string& macroname)
    {
        std::unordered_map<std::string, std::size_t> test_index;
        std::ostringstream defs_list;
        std::ostringstream test_list;
        std::size_t        test_counter = 0;
        test_list << "#define " << macroname << "(o) \\\n";
        std::unordered_map<std::string, unsigned>    tests_per_defs_count;
        std::unordered_map<std::string, std::string> tests_per_defs;

        std::map<std::string, std::string> test_prefixes;
        auto compressTestName = [&](const std::string& name)
        {
            std::size_t p = name.find('/');
            if(p != name.npos)
            {
                std::string prefix = name.substr(0, ++p);
                auto i = test_prefixes.lower_bound(prefix);
                if(i == test_prefixes.end() || i->second != prefix)
                {
                    std::string np = "t" + prefix.substr(0,2);
                    test_prefixes.emplace_hint(i, std::move(prefix), np);
                    prefix = std::move(np);
                }
                else
                    prefix = i->second;
                return prefix + "\"" + name.substr(p) + "\"";
            }
            return "\"" + name + "\"";
        };

        for(auto& test: tests)
        {
            const std::string& type = test.first;
            const auto&  collection = test.second;
            size_t n_tests = collection.size();

            std::ostringstream listbuffer;

            std::string defines = GetDefinesForCpptype(type);

            unsigned list_width_counter = 0;
            unsigned list_test_count = 0;

            listbuffer <<
                "const unsigned short RegressionTests<" << type << ">::Tests[]";
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

                    // FIXME: Use DEFINE_TYPES here
                    if(testdata.DataTypes.find("double") != testdata.DataTypes.end())
                        has_dbl = true;
                    if(testdata.DataTypes.find("long") != testdata.DataTypes.end())
                        has_long = true;
                    if(testdata.DataTypes.find("GmpInt") != testdata.DataTypes.end())
                        has_long = true;

                    // Example: t20"/cmpge/mulmul_imm_neg"
                    //          ^^^^^^^^^^^^^^^^^^^^^^^^^^ 26 letters
                    std::string cname = compressTestName(testdata.TestName);
                    std::string limits = " " + ranges.str();
                    int w1 = 16 - std::max(0, int(limits.size()-20));
                    int w2 = 20 - std::max(0, int(cname.size()-w1));
                    linebuf
                        << "o("
                        <<        (testdata.UseDegrees ? 'T' : 'F')
                        <<        (has_dbl  ? 'T' : 'F')
                        <<        (has_long ? 'T' : 'F')
                        <<        (testdata.UseAbsImag ? 'T' : 'F')
                        <<        testdata.ParamAmount
                        << ","  << std::setw(w1) << std::left << cname
                        << ","  << std::setw(w2) << std::right << limits
                        << "," << testdata.FuncString
                        << ", " << '"' << testdata.ParamString << '"'
                        << ")";

                    if(list_width_counter >= 19)
                    {
                        listbuffer << "\n          ";;
                        list_width_counter = 0;
                    }

                    std::string teststr(linebuf.str());
                    auto i = test_index.find(teststr);
                    if(i == test_index.end())
                    {
                        test_list << "/*" << std::setw(4) << test_counter << "*/ " << teststr << " \\\n";
                        test_index.emplace(teststr, test_counter);
                        listbuffer << std::setw(4) << test_counter << ',';
                        ++test_counter;
                    }
                    else
                        listbuffer << std::setw(4) << i->second << ',';
                    ++list_width_counter;
                    ++list_test_count;
                }
                listbuffer << "};\n";
            }
            //listbuffer << "template const unsigned short RegressionTests<" << type << ">::Tests[];\n";

            tests_per_defs[defines] += listbuffer.str();
            tests_per_defs_count[defines] += list_test_count;
        }
        std::ostringstream defs;
        std::unordered_map<unsigned, std::string> per_mask;
        for(const auto& section: tests_per_defs)
        {
            const std::string& define    = section.first;

            if(!define.empty())
                defs << "\n#ifdef " << define << "\n";

            unsigned n = tests_per_defs_count.find(define)->second;
            defs <<
                "template<>\n"
                "struct RegressionTests<" << GetCpptypeForDefines(define) << ">\n"
                "{\n"
                "    static const unsigned short Tests[" << n << "];\n"
                "};\n";

            if(!define.empty())
                defs << "#endif /*" << define << " */\n";

            unsigned maskpow2 = GetMaskForDefines(define);
            unsigned mask=1;
            while(maskpow2 && !(maskpow2&1)) { ++mask; maskpow2>>=1; }
            std::ostringstream out2;
            out2 << "#include \"testbed_autogen.hh\"\n";
            if(!define.empty())
                out2 << "\n#ifdef " << define << "\n";
            out2 << section.second;
            if(!define.empty())
                out2 << "#endif /*" << define << " */\n";
            per_mask[mask] += out2.str();
        }
        out << defs.str();

        for(auto& m: per_mask)
            std::ofstream("tests/testbed_testlist" + std::to_string(m.first) + ".cc")
                << m.second;
        for(auto& p: test_prefixes)
            defs_list << "#define " << p.second << " \"" << p.first << "\"\n";
        test_list << "/*end of list */\n";
        //for(auto& p: test_prefixes)
        //    test_list << "#undef " << p.second << '\n';
        return defs_list.str() + test_list.str();
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

    void CompileTest(std::istream& inf,
                     const std::string& testname,
                     unsigned&          user_param_count)
    {
        TestData test;
        test.TestName = testname;

        std::unordered_map<std::string, std::string> var_trans;

        unsigned datatype_limit_mask = ~0u;

        auto getline = [&]()
        {
            std::string in_line;
            std::getline(inf, in_line);
            bool good = inf.good();
            if(good)
            {
                // Trip spaces from both ends
                std::size_t remove_spaces_begin = 0, retain_end = in_line.size();
                while(remove_spaces_begin < in_line.size() &&
                    (in_line[remove_spaces_begin]==' '
                  || in_line[remove_spaces_begin]=='\t'
                  || in_line[remove_spaces_begin]=='\r'
                  || in_line[remove_spaces_begin]=='\n'))
                {
                    ++remove_spaces_begin;
                }
                while(retain_end > remove_spaces_begin &&
                    (in_line[retain_end-1]==' '
                  || in_line[retain_end-1]=='\t'
                  || in_line[retain_end-1]=='\r'
                  || in_line[retain_end-1]=='\n'))
                {
                    --retain_end;
                }
                in_line.erase(retain_end);
                in_line.erase(0, remove_spaces_begin);
            }
            return std::make_pair(good, in_line);
        };

        unsigned linenumber = 0;
        for(;;)
        {
            auto got = getline();
            if(!got.first) break;
            std::string in_line = std::move(got.second);
            ++linenumber;

            if(in_line.empty()) continue;
            if(in_line.back() == '\\') // Backslash continuation?
            {
                in_line.pop_back();
                // Continue on another line
                for(;;)
                {
                    got = getline();
                    if(!got.first) break;
                    std::string in_line2 = std::move(got.second);
                    ++linenumber;
                    if(in_line2.empty() || in_line2.back() != '\\')
                    {
                        in_line += std::move(in_line2);
                        break;
                    }
                    in_line2.pop_back();
                    in_line += std::move(in_line2);
                }
            }
            const char* line = in_line.c_str();
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
                            test.DataTypes.insert(std::move(cpptype));

                            valuepos = space;
                        }
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
                        all_functions.emplace(testname,
                            FunctionInfo{datatype_limit_mask, std::move(code), std::move(used_constants)}
                                              );
                    }
                    break;
            }
        }

        for(auto type: test.DataTypes)
            tests[type].push_back(test);
    }
}

int main(int argc, char* argv[])
{
    std::vector<std::string> files, ignore_patterns, strip_prefix;

    for(int a=1; a<argc; ++a)
    {
        if(std::strcmp(argv[a], "--ignore") == 0)
        {
            if(++a == argc)
            {
                std::cerr << "make_tests: Expected ignore-pattern after --ignore\n";
                return 1;
            }
            ignore_patterns.push_back(argv[a]);
            continue;
        }
        else if(std::strcmp(argv[a], "--strip_prefix") == 0
             || std::strcmp(argv[a], "--strip-prefix") == 0
               )
        {
            if(++a == argc)
            {
                std::cerr << "make_tests: Expected strip-prefix-pattern after --strip-prefix\n";
                return 1;
            }
            strip_prefix.push_back(argv[a]);
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

    unsigned user_param_count{};

    for(const auto& filename: files)
    {
        std::ifstream inf(filename);
        std::string testname = filename;
        for(auto& p: strip_prefix)
            if(testname.compare(0, p.size(), p) == 0)
                testname.erase(0, p.size());

        CompileTest(inf, testname, user_param_count);
    }

    std::unordered_map<std::string, unsigned> test_index;
    for(auto& coll: tests)
        for(auto& test: coll.second)
            test_index.emplace(test.TestName, test_index.size());

    auto gen_funcs = [&](
        std::ostream& out,
        unsigned m,
        std::string (*gen)(unsigned, const std::string&, const FunctionInfo&),
        void        (*makecase)(std::ostream&,
                                const std::string& /*caseno*/, const std::string& /* code */)
                        )
    {
        constexpr unsigned customtest_index = ~0u;
        // Use sorted container here for more beautiful generated code
        std::map<std::string/*code*/, std::unordered_set<unsigned/*case*/>> cases;
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
        FunctionInfo{
            ~0u,
            "OptimizerTests::evaluateFunction(vars)",
            {}}
    );

    static const char lesser_opt_begin[] = ""
    //    "#pragma GCC push_options\n"
        "#pragma GCC optimize   (\"O0,no-ipa-pta,no-ivopts\")\n"
    ;
    static const char lesser_opt_end[] = ""
    //    "#pragma GCC pop_options\n"
    ;
    static const char unreachable[] = R"(
    unreachable_helper();
    return Value_t{};
)";

    std::ofstream outStream("tests/testbed_autogen.hh");
    outStream << R"(
#include <type_traits>
#include "extrasrc/fpaux.hh"
#include "tests/testbed_defs.hh"

template<typename Value_t>
struct fp_type_mask : public std::integral_constant<unsigned, 0)";
for(unsigned m=1; m<=highest_mask; m<<=1)
    outStream << "\n    | (" << m << "*int(" << GetTestCodeForMask(m, "Value_t") << "))";
outStream << R"(> {};

template<typename Value_t, unsigned n = fp_type_mask<Value_t>::value>
struct const_container {};

#if defined(__cpp_if_constexpr) && __cpp_if_constexpr >= 201606L

template<typename Value_t, unsigned type_mask = fp_type_mask<Value_t>::value>
Value_t evaluate_test(unsigned which, const Value_t* vars);

#else // No "if constexpr"

template<typename Value_t, unsigned type_mask = fp_type_mask<Value_t>::value>
struct evaluator { };
)";
    for(unsigned n=1, m=1; m<=highest_mask; m<<=1, ++n)
        outStream << R"(
template<typename Value_t> struct evaluator<Value_t,)" << m << R"(>
    { static Value_t calc(unsigned which, const Value_t* vars); };
)";
    outStream << R"(

template<typename Value_t>
inline Value_t evaluate_test(unsigned which, const Value_t* vars)
{
    return evaluator<Value_t>::calc(which, vars);
}
#endif
)";

    if(true) // scope for testbed_alltests.cc
    {
        std::string test_list = ListTests(outStream, "FP_LIST_ALL_TESTS");
        std::ofstream("tests/testbed_alltests.cc") << R"(
#include "testbed_autogen.hh"
const TestType AllTests[] =
{
#define q(c) (c=='T'||c=='Y')
#define o(opt,testname, min,max,step, funcstring, paramstr) \
    { testname,#funcstring,paramstr, #min,#max,#step, #opt[4]-'0', q(#opt[0]),q(#opt[1]),q(#opt[2]),q(#opt[3]) },
)" << test_list << R"(FP_LIST_ALL_TESTS(o)
#undef o
#undef q
};
)";
    } // testbed_alltests.cc

    if(true) // scope for testbed_cpptest.hh
    {
        std::ofstream out2("tests/testbed_cpptest.hh");
        // This file does not need any #includes.
        gen_funcs(out2, 0, [](unsigned, const std::string&, const FunctionInfo& info)
        {
            return info.code;
        }, [](std::ostream& out, const std::string& caseno, const std::string& code)
        {
            std::ostringstream temp;
            temp << 'T' << caseno;
            out << "#define " << std::setw(5) << temp.str()
                << "(_,__)_ " << code << " __\n";
        });

        out2 << R"(
    #define FP_LIST_ALL_CONST()";
        for(unsigned m=1; m<=highest_mask; m*=2)
        {
            if(m>1) out2 << ',';
            out2 << 'o' << m;
        }
        out2 << ") \\\n";
        std::unordered_map<std::string, std::pair<unsigned,std::string>> all_const;
        for(const auto& f: all_functions)
            for(auto& c: f.second.used_constants)
                all_const.emplace(c.first, std::make_pair(0,c.second)).first->second.first |= f.second.type_limit_mask;
        for(auto& c: all_const)
        {
            for(unsigned m=1; m<=highest_mask; m*=2)
                if(c.second.first & m)
                    out2 << " o" << m << "(" << std::setw(8) << c.first << ',' << std::setw(7) << c.second.second << ")";
            out2 << " \\\n";
        }
        for(unsigned m=1; m<=highest_mask; m*=2)
        {
            bool complex = false;
            #define o(code,mask,typename,def,lit) \
                if(mask == m && std::string(#code)[0] == 'c') \
                    complex = true;
            DEFINE_TYPES(o)
            #undef o
            if(complex)
                out2 << " o" << m << "(i, Value_t(0,1)+(Value_t)0) \\\n";
        }
        out2 << "/* end of list */\n";
    } // testbed_cpptest.hh

    for(unsigned n=1, m=1; m<=highest_mask; m<<=1, ++n)
    {
        std::ofstream out2("tests/testbed_const" + std::to_string(n) + ".hh");
        // This file does not need any #includes.
        out2 << "#define _(name,lit)\n";
        std::string literal_maker = "static_assert(!\"I don't know how to make literals\");";
        std::string if_rule = "#if(0";
    #define o(code,mask,typename,def,lit) if(m == mask) { if_rule += "||defined(" #def ")"; literal_maker = lit; }
        DEFINE_TYPES(o)
    #undef o
        if_rule += ")";
        out2 << "#define o(name,lit) " << literal_maker << "\n" << if_rule;
        out2 << R"(
template<typename Value_t> struct const_container<Value_t,)" << m << "> { FP_LIST_ALL_CONST(";
        for(unsigned n=1; n<=highest_mask; n<<=1)
        {
            if(n>1) out2 << ',';
            out2 << ((n==m) ? 'o' : '_');
        }
        out2 << ") };\n"
              "#endif\n"
              "#undef o\n"
              "#undef _\n\n";
    } // testbed_const*.hh

    if(true)
    {
        std::ofstream out2("tests/testbed_evaluate0.cc");

        out2 << lesser_opt_begin << R"(
#include "testbed_autogen.hh"
#include "testbed_comp.hh"
#include "testbed_cpptest.hh"
)";
        for(unsigned n=1, m=1; m<=highest_mask; m<<=1, ++n)
            out2 << R"(#include "testbed_const)" << n << R"(.hh"
)";
        out2 << R"(
#if defined(__cpp_if_constexpr) && __cpp_if_constexpr >= 201606L
template<typename Value_t, unsigned type_mask>
Value_t evaluate_test(unsigned which, const Value_t* vars)
{
    static const_container<Value_t> c;
    [[maybe_unused]] Value_t uparam[)" << user_param_count << R"(];
    using namespace FUNCTIONPARSERTYPES;
    switch(which)
    {
        #define M(n,m) T##n(case n:if constexpr(type_mask&m)return,;else break;)
)";
        gen_funcs(out2, 0, [](unsigned, const std::string&, const FunctionInfo& info)
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
        out2 << R"(
        #undef M
default: break;
    })" << unreachable << R"(
}
)";
#define o(code,mask,typename,def,lit) \
        out2 << \
            "#ifdef " #def "\n" \
            "  template " << #typename << " evaluate_test<" #typename ">(unsigned,const " #typename " *);\n" \
            "#endif\n";
        DEFINE_TYPES(o)
#undef o
        out2 << R"(
#endif
)" << lesser_opt_end;
    } //testbed_evaluate0.cc

    for(unsigned n=1, m=1; m<=highest_mask; m<<=1, ++n)
    {
        std::ofstream out2("tests/testbed_evaluate" + std::to_string(n) + ".cc");

        out2 << lesser_opt_begin << R"(
#include "testbed_autogen.hh"
#include "testbed_comp.hh"
#include "testbed_cpptest.hh"
#include "testbed_const)" << n << R"(.hh"

#if !(defined(__cpp_if_constexpr) && __cpp_if_constexpr >= 201606L) // No "if constexpr"
#define M(n) T##n(case n:return,;)
#if(0)";
#define o(code,mask,typename,def,lit) if(m == mask) out2 << "||defined(" #def ")";
        DEFINE_TYPES(o)
#undef o
        out2 << R"()
template<typename Value_t>
Value_t evaluator<Value_t,)" << m << R"(>::calc(unsigned which, const Value_t* vars)
{
    static const_container<Value_t> c;
    [[maybe_unused]] Value_t uparam[)" << user_param_count << R"(];
    using namespace FUNCTIONPARSERTYPES;
    switch(which) {
)";
        gen_funcs(out2, m, [](unsigned m, const std::string&, const FunctionInfo& info)
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
        out2 << R"(
default: break;
})" << unreachable << R"(}
#undef M
)";
#define o(code,mask,typename,def,lit) \
        if(m == mask) \
            out2 << "template " #typename " evaluator<" #typename << "," << mask \
                 << ">::calc(unsigned,const " #typename " *);\n";
        DEFINE_TYPES(o)
#undef o
        out2 << R"(
#endif
#endif
)" << lesser_opt_end;
    } // testbed_evaluate*.cc

    return 0;
}
