#include "cpp_compress.hh"

#include <vector>
#include <map>
#include <iostream>

namespace
{
    struct token
    {
        std::string value;
        unsigned    hash;
        bool        preproc;
        int balance;

        token(const std::string& v) : value(v)
        {
            Rehash();
        }

        void operator=(const std::string& v) { value=v; Rehash(); }

        bool operator==(const token& b) const
        {
            return hash == b.hash && value == b.value;
        }

        bool operator!=(const token& b) const
            { return !operator==(b); }

        void Rehash()
        {
            hash = 0;
            preproc = value[0] == '#' || value[0] == '\n';
            balance = 0;
            for(size_t a=0; a<value.size(); ++a)
            {
                hash = hash*0x8088405u + value[a];
                if(value[a]=='(') ++balance;
                else if(value[a]==')') --balance;
            }
        }
        void swap(token& b)
        {
            value.swap(b.value);
            std::swap(hash, b.hash);
            std::swap(balance, b.balance);
            std::swap(preproc, b.preproc);
        }
    };
    struct length_rec
    {
        unsigned begin_index;
        unsigned num_tokens;
        unsigned num_occurrences;
    };

    inline bool isnamechar(char c) { return std::isalnum(c) || c == '_'; }

    std::string GetSeq(std::vector<token>::const_iterator begin,
                       size_t n, bool NewLines)
    {
        bool InDefineMode = false;

        /* Resequence the input */
        std::string result;
        int quotemode = 0;
        while(n-- > 0)
        {
            const std::string& value = begin->value; ++begin;
         #if 1
            if(!result.empty() && result[result.size()-1] != '\n')
            {
                if(value[0] == '#') result += '\n';
                else if(isnamechar(value[0])
                     && isnamechar(result[result.size()-1]))
                {
                    if(!NewLines || InDefineMode)
                        result += ' ';
                    else
                        result += '\n';
                }
            }
         #else
            result += '!';
         #endif
            if(value[0] == '#')
            {
                if(value.substr(0,7) == "#define") InDefineMode = true;
                if(value.substr(0,8) == "#include") InDefineMode = true;
            }
            if(value == "\n")
            {
                if(InDefineMode) { result += "\n"; InDefineMode = false; }
                continue;
            }

            switch(quotemode)
            {
                case 0: // prev wasn't a quote
                    if(value[0] == '"'
                    && (n>0 && begin->value[0] == '"'))
                        { quotemode = 1;
                          result.append(value, 0, value.size()-1);
                          //result += value.substr(0, value.size()-1);
                          continue;
                        }
                    else
                        result += value;
                    break;
                case 1: // prev was a quote, skip this quote
                    if(n>0 && begin->value[0] == '"')
                        { //result += value.substr(1, value.size()-2);
                          result.append(value, 1, value.size()-2);
                          continue;
                        }
                    else
                        { quotemode = 0;
                          //result += value.substr(1);
                          result.append(value, 1, value.size()-1);
                        }
                    break;
            }
          #if 1
            if(NewLines && !InDefineMode)
            {
                if(value[0] == '#'
                || value[0] == '}'
                || value[0] == '"'
                  )
                {
                    result += '\n';
                }
            }
            if(n > 0 && (value == "<" || value == ">") && value == begin->value)
                result += ' '; // Avoid making < < into <<, similarly for > >
          #endif
        }
        return result;
    }

    std::vector<token>
        Tokenize(const std::string& input, bool InDefineMode = false)
    {
        std::vector<token> result;
        size_t a=0, b=input.size();
        while(a < b)
        {
            if(InDefineMode && input[a]=='\n')
            {
                InDefineMode = false;
                result.push_back( token("\n") );
                ++a;
                continue;
            }
            if(input[a]==' ' || input[a]=='\t'
            || input[a]=='\n' || input[a]=='\r') { ++a; continue; }

            if(input[a]=='/' && input[a+1]=='*')
            {
                a += 2;
                while(a < b && (input[a-2]!='*' || input[a-1]!='/')) ++a;
                continue;
            }
            if(input[a]=='/' && input[a+1]=='/')
            {
                while(a < b && input[a]!='\n') ++a;
                continue;
            }
            if(input[a]=='_' || (input[a]>='a' && input[a]<='z')
                             || (input[a]>='A' && input[a]<='Z'))
            {
                size_t name_begin = a;
                while(++a < b)
                {
                    if(isnamechar(input[a])) continue;
                    break;
                }
                std::string name(input, name_begin, a-name_begin);
                result.push_back(name);
                /* IMPORTANT NOTE: HARDCODED LIST OF ALL PREPROCESSOR
                 * MACROS THAT TAKE PARAMETERS. ANY MACROS NOT LISTED
                 * HERE WILL BE BROKEN AFTER COMPRESSION.
                 */
                if(input[a] == '('
                && (name == "P1" || name == "P2" || name == "P3"
                 || name == "P" || name == "N" || name == "S"
                 || name == "FP_TRACE_BYTECODE_OPTIMIZATION"
                 || name == "FP_TRACE_OPCODENAME"
                 || name == "FP_ReDefinePointers"
                 || name == "FPHASH_CONST" || name == "assert"
                 || name == "FPO" || name == "DUP_ONE"
                 || name == "HANDLE_UNARY_CONST_FUNC"))
                {
                    std::vector<token> remains = Tokenize(input.substr(a), InDefineMode);
                    int balance = 1;
                    size_t eat = 1;
                    for(; balance != 0; ++eat)
                        balance += remains[eat].balance;
                    result.back() = result.back().value + GetSeq(remains.begin(), eat, false);
                    result.insert(result.end(), remains.begin()+eat, remains.end());
                    a = b; // done
                }
                continue;
            }
            if(std::isdigit(input[a]) ||
               (input[a] == '.' && std::isdigit(input[a+1])))
            {
                size_t value_begin = a;
                while(++a < b)
                {
                    if((input[a]>='0' && input[a]<='9')
                    || input[a]=='.' || input[a]=='+' || input[a]=='-'
                    || input[a]=='x' || (input[a]>='a' && input[a]<='f')
                    || input[a]=='p' || (input[a]>='A' && input[a]<='F')
                    || input[a]=='u' || input[a]=='U'
                    || input[a]=='l' || input[a]=='f'
                    || input[a]=='L' || input[a]=='F') continue;
                    break;
                }
                result.push_back( std::string(input, value_begin, a-value_begin ));
                continue;
            }
            if(a+1 < b && input[a] == '>' && input[a+1] == '>')
                { result.push_back(input.substr(a, 2)); a += 2; continue; }
            if(a+1 < b && input[a] == '<' && input[a+1] == '<')
                { result.push_back(input.substr(a, 2)); a += 2; continue; }
            if(a+1 < b && input[a] == '+' && input[a+1] == '+')
                { result.push_back(input.substr(a, 2)); a += 2; continue; }
            if(a+1 < b && input[a] == '-' && input[a+1] == '-')
                { result.push_back(input.substr(a, 2)); a += 2; continue; }
            if(a+1 < b && input[a] == '&' && input[a+1] == '&')
                { result.push_back(input.substr(a, 2)); a += 2; continue; }
            if(a+1 < b && input[a] == '|' && input[a+1] == '|')
                { result.push_back(input.substr(a, 2)); a += 2; continue; }
            if(a+1 < b && (input[a] == '>' || input[a] == '<'
                        || input[a] == '!' || input[a] == '='
                        || input[a] == '+' || input[a] == '-'
                        || input[a] == '*' || input[a] == '/'
                        || input[a] == '&' || input[a] == '|'))
                if(input[a+1] == '=')
                    { result.push_back(input.substr(a, 2)); a += 2; continue; }
            if(a+1 < b && (input[a] == ':' && input[a+1] == ':'))
                    { result.push_back(input.substr(a, 2)); a += 2; continue; }
            if(input[a] == '#')
            {
                if(input.substr(a,8) == "#include")
                {
                    result.push_back( token("#include") );
                    InDefineMode = true;
                    a += 8;
                    continue;
                }
                if(input.substr(a,7) == "#define")
                {
                    size_t p = a+7;
                    while(p < b && std::isspace(input[p])) ++p; // skip blank
                    while(p < b && isnamechar(input[p])) ++p; // skip term
                    if(input[p] != '(')
                    {
                        InDefineMode = true;
                        result.push_back(input.substr(a, p-a) + " ");
                        a = p;
                        continue;
                    }
                }
                size_t preproc_begin = a;
                bool in_quotes = false;
                while(++a < b)
                {
                    if(!in_quotes && input[a]=='"')
                        { in_quotes=true; continue; }
                    if(in_quotes && input[a]=='"' && input[a-1]!='\\')
                        { in_quotes=false; continue; }
                    if(input[a]=='\\' && input[a+1]=='\n') { ++a; continue; }
                    if(input[a]=='\n') break;
                }
                std::string stmt(input, preproc_begin, a-preproc_begin);
                if(stmt.substr(0,5) != "#line")
                    result.push_back(stmt);
                result.push_back( token("\n") );
                continue;
            }
            if(input[a] == '"')
            {
                size_t string_begin = a;
                while(++a < b)
                    if(input[a]=='"' &&
                      (input[a-1] != '\\'
                     || input[a-2]=='\\')) { ++a; break; }
                result.push_back( std::string(input, string_begin, a-string_begin) );
                continue;
            }
            if(input[a] == '\'')
            {
                size_t char_begin = a; a += 3;
                if(input[a-2] == '\\') ++a;
                result.push_back( std::string(input, char_begin, a-char_begin) );
                continue;
            }
            result.push_back( std::string(input,a++,1) );
        }
        return result;
    }

    static const char cbuf[] =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
}

std::string CPPcompressor::Compress(const std::string& input)
{
    std::vector<token> tokens = Tokenize(input);
    bool tried_retokenizing = false;
    std::string result;
    for(;;)
    {
        bool compressed = false;

        static unsigned seq_count = 0;
        std::string seq_name_buf = "";
        {// scope for creating seq_name_buf
            unsigned p=seq_count++;
            seq_name_buf += "qgh"[(p/36)%3];
            seq_name_buf += cbuf[p%36]; p/=3*36; // 0-9A-Z
            for(; p!=0; p /= 63)
                seq_name_buf += cbuf[p%63];
        }
        size_t seq_name_length = seq_name_buf.size();

        /* Find a sub-sequence of tokens for which
         * the occurrence-count times total length is
         * largest and the balance of parentheses is even.
         */
        std::map<unsigned, length_rec> hash_results;
        long best_score=0;
        size_t best_score_length=0;
        unsigned best_hash=0;

        std::cerr << tokens.size() << " tokens\n";

        std::vector<bool> donttest(tokens.size(), false);
        const size_t lookahead_depth = 75;
        for(size_t a=0; a<tokens.size(); ++a)
        {
            if(donttest[a]) continue;

            //std::cerr << a << '\t' << best_score << '\t' << best_score_length << '\r' << std::flush;
            size_t cap = a+lookahead_depth;
            for(size_t b=a+1; b<tokens.size() && b<cap; ++b)
            {
                size_t max_match_len = std::min(tokens.size()-b, b-a);
                size_t match_len = 0;
                unsigned hash = 0;
                //int balance = 0;
                while(match_len < max_match_len && tokens[a+match_len] == tokens[b+match_len])
                {
                    const token& word = tokens[a+match_len];
                    if(word.preproc) break; // Cannot include preprocessing tokens in substrings
                    //balance += word.balance;
                    //if(balance < 0) break;

                    ++match_len;
                    hash = ~hash*0x8088405u + word.hash;

                    //donttest[b] = true;
                    //if(balance == 0)
                    {
                        std::map<unsigned, length_rec>::iterator i
                            = hash_results.lower_bound(hash);
                        if(i == hash_results.end() || i->first != hash)
                        {
                            length_rec rec;
                            rec.begin_index = a;
                            rec.num_tokens  = match_len;
                            rec.num_occurrences = 1;
                            hash_results.insert(i, std::make_pair(hash,rec));
                            cap = std::max(cap, b+match_len+lookahead_depth);
                        }
                        else if(i->second.begin_index == a)
                        {
                            if(std::equal(
                                tokens.begin()+a, tokens.begin()+a+match_len,
                                tokens.begin() + i->second.begin_index))
                            {
                                long string_len = GetSeq(tokens.begin()+a, match_len, false).size();
                                long n = (i->second.num_occurrences += 1);
                                long define_length = seq_name_length + 9 - long(string_len);
                                long replace_length = long(string_len) - (long(seq_name_length)+1);
                                long score = replace_length * n - define_length;
                                if(score > best_score)
                                {
                                    best_score        = score;
                                    best_score_length = string_len;
                                    best_hash         = hash;
                                }
                            }
                            cap = std::max(cap, b+match_len+lookahead_depth);
                        }
                    }
                }
            }
        }
        if(best_score > 0)
        {
            const length_rec& rec = hash_results[best_hash];
            if(rec.num_occurrences > 0)
            {
                /* Found a practical saving */
                std::vector<token> sequence
                    (tokens.begin()+rec.begin_index,
                     tokens.begin()+rec.begin_index+rec.num_tokens);
                std::cerr << "#define " << seq_name_buf << " " <<
                    GetSeq(sequence.begin(), sequence.size(), false)
                        //<< " /* " << rec.num_occurrences
                        //<< " occurrences */"
                          << std::endl;

                /* Replace all occurrences of the sequence with the sequence name */
                std::vector<bool> deletemap(tokens.size(), false);
                for(size_t a=0;//rec.begin_index+rec.num_tokens;
                           a+rec.num_tokens<=tokens.size();
                           ++a)
                {
                    if(std::equal(sequence.begin(),
                                  sequence.end(),
                                  tokens.begin()+a))
                    {
                        tokens[a] = seq_name_buf;
                        for(size_t b=1; b<rec.num_tokens; ++b)
                            deletemap[++a] = true;
                    }
                }
                size_t tgt=0, src=0;
                for(; src < tokens.size(); ++src)
                    if(!deletemap[src])
                        tokens[tgt++].swap(tokens[src]);
                tokens.erase(tokens.begin()+tgt, tokens.end());

                sequence.insert(sequence.begin(),
                    token("#define " + seq_name_buf + " "));
                sequence.push_back( token("\n") );

                tokens.insert(tokens.begin(),
                    sequence.begin(), sequence.end());

                /* Find more repetitions */
                compressed         = true;
                tried_retokenizing = false;
            }
        }
        if(!compressed)
        {
            if(tried_retokenizing) break;
            tried_retokenizing = true;

            std::cerr << "Retokenizing\n";
            result = GetSeq(tokens.begin(), tokens.size(), true);
            tokens = Tokenize(result);
        }
    }
    return result;
}
