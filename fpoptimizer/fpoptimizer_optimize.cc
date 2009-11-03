#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_consts.hh"
#include "fpoptimizer_opcodename.hh"
#include "fpoptimizer_optimize.hh"

#include <stdio.h>

#include <algorithm>
#include <map>
#include <sstream>

using namespace FUNCTIONPARSERTYPES;
using namespace FPoptimizer_Grammar;
using namespace FPoptimizer_CodeTree;
using namespace FPoptimizer_Optimize;

namespace
{
    /* I have heard that std::equal_range() is practically worthless
     * due to the insane limitation that the two parameters for Comp() must
     * be of the same type. Hence we must reinvent the wheel and implement
     * our own here. This is practically identical to the one from
     * GNU libstdc++, except rewritten. -Bisqwit
     */
    template<typename It, typename T, typename Comp>
    std::pair<It, It>
    MyEqualRange(It first, It last, const T& val, Comp comp)
    {
        size_t len = last-first;
        while(len > 0)
        {
            size_t half = len/2;
            It middle(first); middle += half;
            if(comp(*middle, val))
            {
                first = middle;
                ++first;
                len = len - half - 1;
            }
            else if(comp(val, *middle))
            {
                len = half;
            }
            else
            {
                // The following implements this:
                // // left = lower_bound(first, middle, val, comp);
                It left(first);
              {///
                It& first2 = left;
                It last2(middle);
                size_t len2 = last2-first2;
                while(len2 > 0)
                {
                    size_t half2 = len2 / 2;
                    It middle2(first2); middle2 += half2;
                    if(comp(*middle2, val))
                    {
                        first2 = middle2;
                        ++first2;
                        len2 = len2 - half2 - 1;
                    }
                    else
                        len2 = half2;
                }
                // left = first2;  - not needed, already happens due to reference
              }///
                first += len;
                // The following implements this:
                // // right = upper_bound(++middle, first, val, comp);
                It right(++middle);
              {///
                It& first2 = right;
                It& last2 = first;
                size_t len2 = last2-first2;
                while(len2 > 0)
                {
                    size_t half2 = len2 / 2;
                    It middle2(first2); middle2 += half2;
                    if(comp(val, *middle2))
                        len2 = half2;
                    else
                    {
                        first2 = middle2;
                        ++first2;
                        len2 = len2 - half2 - 1;
                    }
                }
                // right = first2;  - not needed, already happens due to reference
              }///
                return std::pair<It,It> (left,right);
            }
        }
        return std::pair<It,It> (first,first);
    }

    /* A helper for std::equal_range */
    struct OpcodeRuleCompare
    {
        bool operator() (const CodeTree& tree,
                         unsigned rulenumber) const
        {
            /* If this function returns true, len=half.
             */
            const Rule& rule = grammar_rules[rulenumber];
            return tree.GetOpcode() < rule.match_tree.subfunc_opcode;
        }
        bool operator() (unsigned rulenumber,
                         const CodeTree& tree) const
        {
            /* If this function returns true, rule will be excluded from the equal_range
             */
            const Rule& rule = grammar_rules[rulenumber];
            return rule.match_tree.subfunc_opcode < tree.GetOpcode();
        }
    };

    /* Test and apply a rule to a given CodeTree */
    bool TestRuleAndApplyIfMatch(
        const Rule& rule,
        CodeTree& tree,
        bool from_logical_context)
    {
        MatchInfo info;

        MatchResultType found(false, MatchPositionSpecBaseP());

        if(rule.logical_context && !from_logical_context)
        {
            /* If the rule only applies in logical contexts,
             * but we do not have a logical context, fail the rule
             */
            goto fail;
        }

        /*std::cout << "TESTING: ";
        DumpMatch(rule, *tree, info, false);*/

        for(;;)
        {
        #ifdef DEBUG_SUBSTITUTIONS
            //DumpMatch(rule, tree, info, "Testing");
        #endif
            found = TestParams(rule.match_tree, tree, found.specs, info, true);
            if(found.found) break;
            if(!&*found.specs)
            {
            fail:;
                // Did not match
        #ifdef DEBUG_SUBSTITUTIONS
                DumpMatch(rule, tree, info, false);
        #endif
                return false;
            }
        }
        // Matched
    #ifdef DEBUG_SUBSTITUTIONS
        DumpMatch(rule, tree, info, true);
    #endif
        SynthesizeRule(rule, tree, info);
        return true;
    }
}

namespace FPoptimizer_Optimize
{
    /* Apply the grammar to a given CodeTree */
    bool ApplyGrammar(
        const Grammar& grammar,
        CodeTree& tree,
        bool from_logical_context)
    {
        if(tree.GetOptimizedUsing() == &grammar)
        {
#ifdef DEBUG_SUBSTITUTIONS
            std::cout << "Already optimized:  ";
            DumpTree(tree);
            std::cout << "\n" << std::flush;
#endif
            return false;
        }

        /* First optimize all children */
        if(true)
        {
            bool changed = false;

            switch(tree.GetOpcode())
            {
                case cNot:
                case cNotNot:
                case cAnd:
                case cOr:
                    for(size_t a=0; a<tree.GetParamCount(); ++a)
                        if(ApplyGrammar( grammar, tree.GetParam(a), true))
                            changed = true;
                    break;
                case cIf:
                case cAbsIf:
                    if(ApplyGrammar( grammar, tree.GetParam(0), tree.GetOpcode() == cIf))
                        changed = true;
                    for(size_t a=1; a<tree.GetParamCount(); ++a)
                        if(ApplyGrammar( grammar, tree.GetParam(a), from_logical_context))
                            changed = true;
                    break;
                default:
                    for(size_t a=0; a<tree.GetParamCount(); ++a)
                        if(ApplyGrammar( grammar, tree.GetParam(a), false))
                            changed = true;
            }

            if(changed)
            {
                // Give the parent node a rerun at optimization
                tree.Mark_Incompletely_Hashed();
                return true;
            }
        }

        /* Figure out which rules _may_ match this tree */
        typedef const unsigned char* rulenumit;

        std::pair<rulenumit, rulenumit> range =
            MyEqualRange(grammar.rule_list,
                         grammar.rule_list + grammar.rule_count,
                         tree,
                         OpcodeRuleCompare());

        if(range.first != range.second)
        {
#ifdef DEBUG_SUBSTITUTIONS
            std::vector<unsigned char> rules;
            rules.reserve(range.second - range.first);
            for(rulenumit r = range.first; r != range.second; ++r)
            {
                //if(grammar_rules[*r].match_tree.subfunc_opcode != tree.GetOpcode()) continue;
                if(IsLogisticallyPlausibleParamsMatch(grammar_rules[*r].match_tree, tree))
                    rules.push_back(*r);
            }
            range.first = &rules[0];
            range.second = &rules[rules.size()-1]+1;

            if(range.first != range.second)
            {
                std::cout << "Input (" << FP_GetOpcodeName(tree.GetOpcode())
                          << "[" << tree.GetParamCount()
                          << "]";

                unsigned first=~unsigned(0), prev=~unsigned(0);
                const char* sep = ", rules ";
                for(rulenumit r = range.first; r != range.second; ++r)
                {
                    if(first==~unsigned(0)) first=prev=*r;
                    else if(*r == prev+1) prev=*r;
                    else
                    {
                        std::cout << sep << first; sep=",";
                        if(prev != first) std::cout << '-' << prev;
                        first = prev = *r;
                    }
                }
                if(first != ~unsigned(0))
                {
                    std::cout << sep << first;
                    if(prev != first) std::cout << '-' << prev;
                }
                std::cout << ": ";
                DumpTree(tree);
                std::cout << "\n" << std::flush;
            }
#endif

            bool changed = false;

            for(rulenumit r = range.first; r != range.second; ++r)
            {
            #ifndef DEBUG_SUBSTITUTIONS
                if(!IsLogisticallyPlausibleParamsMatch(grammar_rules[*r].match_tree, tree))
                    continue;
            #endif
                if(TestRuleAndApplyIfMatch(grammar_rules[*r], tree, from_logical_context))
                {
                    changed = true;
                    break;
                }
            }

            if(changed)
            {
    #ifdef DEBUG_SUBSTITUTIONS
                std::cout << "Changed." << std::endl;
                std::cout << "Output: ";
                DumpTree(tree);
                std::cout << "\n" << std::flush;
    #endif
                // Give the parent node a rerun at optimization
                tree.Mark_Incompletely_Hashed();
                return true;
            }
        }

        // No changes, consider the tree properly optimized.
        tree.SetOptimizedUsing(&grammar);
        return false;
    }

    void ApplyGrammars(FPoptimizer_CodeTree::CodeTree& tree)
    {
    #ifdef FPOPTIMIZER_MERGED_FILE
        #define C *(const Grammar*)&
    #else
        #define C
    #endif
        #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Applying grammar_optimize_round1\n";
        #endif
        while(ApplyGrammar(C grammar_optimize_round1, tree))
            { //std::cout << "Rerunning 1\n";
                tree.FixIncompleteHashes();
            }

        #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Applying grammar_optimize_round2\n";
        #endif
        while(ApplyGrammar(C grammar_optimize_round2, tree))
            { //std::cout << "Rerunning 2\n";
                tree.FixIncompleteHashes();
            }

        #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Applying grammar_optimize_round3\n";
        #endif
        while(ApplyGrammar(C grammar_optimize_round3, tree))
            { //std::cout << "Rerunning 3\n";
                tree.FixIncompleteHashes();
            }

        #ifdef DEBUG_SUBSTITUTIONS
        std::cout << "Applying grammar_optimize_round4\n";
        #endif
        while(ApplyGrammar(C grammar_optimize_round4, tree))
            { //std::cout << "Rerunning 4\n";
                tree.FixIncompleteHashes();
            }
        #undef C
    }
}

#endif
