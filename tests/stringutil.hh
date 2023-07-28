#include <string>
#include <cstring>
#include <cstring>

namespace
{
    template<typename CharT>
    [[maybe_unused]] void
    str_replace_inplace(std::basic_string<CharT>& where,
                        const std::basic_string<CharT>& search,
                        const std::basic_string<CharT>& with)
    {
        for(auto a = where.size(); (a = where.rfind(search, a)) != where.npos; )
        {
            where.replace(a, search.size(), with);
            if(a--==0) break;
        }
    }

    [[nodiscard]] [[maybe_unused]] bool WildMatch(const char *pattern, const char *what)
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

    [[nodiscard]] [[maybe_unused]] bool WildMatch_Dirmask(const char *pattern, const char *what)
    {
        const char* slashpos = std::strchr(pattern, '/');
        if(slashpos) // Pattern contains a slash?
            return WildMatch(pattern, what);
        else
        {
            // Pattern does not contain a slash. Use */pattern
            std::string temp = std::string("*/") + pattern;
            return WildMatch(temp.c_str(), what);
        }
    }

    /* Asciibetical comparator, with in-string integer values sorted naturally */
    [[nodiscard]] [[maybe_unused]] bool natcomp(const std::string& a, const std::string& b)
    {
        std::size_t ap=0, bp=0;
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
}
