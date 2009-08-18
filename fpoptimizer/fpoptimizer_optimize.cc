#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_codetree.hh"
#include "fpoptimizer_consts.hh"
#include "fpoptimizer_opcodename.hh"

//#include <stdio.h>

#include <algorithm>
#include <cmath>
#include <map>
#include <assert.h>

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#ifdef FP_SUPPORT_OPTIMIZER

using namespace FUNCTIONPARSERTYPES;

//#define DEBUG_SUBSTITUTIONS

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
}

namespace FPoptimizer_CodeTree
{
    void CodeTree::ConstantFolding()
    {
        // Insert here any hardcoded constant-folding optimizations
        // that you want to be done at bytecode->codetree conversion time.
    }
}

namespace FPoptimizer_Grammar
{
    static double GetPackConst(size_t index)
    {
        double res = pack.clist[index];
    #if 0
        if(res == FPOPT_NAN_CONST)
        {
        #ifdef NAN
            return NAN;
        #else
            return 0.0; // Should be 0.0/0.0, but some compilers don't like that
        #endif
        }
    #endif
        return res;
    }

    /* A helper for std::equal_range */
    struct OpcodeRuleCompare
    {
        bool operator() (const FPoptimizer_CodeTree::CodeTree& tree, const Rule& rule) const
        {
            /* If this function returns true, len=half.
             */

            if(tree.Opcode != rule.func.opcode)
                return tree.Opcode < rule.func.opcode;

            if(tree.Params.size() < rule.n_minimum_params)
            {
                // Tree has fewer params than required?
                return true; // Failure
            }
            return false;
        }
        bool operator() (const Rule& rule, const FPoptimizer_CodeTree::CodeTree& tree) const
        {
            /* If this function returns true, rule will be excluded from the equal_range
             */

            if(rule.func.opcode != tree.Opcode)
                return rule.func.opcode < tree.Opcode;

            if(rule.n_minimum_params < tree.Params.size())
            {
                // Tree has more params than the pattern has?
                switch(pack.mlist[rule.func.index].type)
                {
                    case PositionalParams:
                    case SelectedParams:
                        return true; // Failure
                    case AnyParams:
                        return false; // Not a failure
                }
            }
            return false;
        }
    };

#ifdef DEBUG_SUBSTITUTIONS
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree);
    static const char ImmedHolderNames[2][2] = {"%","&"};
    static const char NamedHolderNames[6][2] = {"x","y","z","a","b","c"};
#endif

    /* Apply the grammar to a given CodeTree */
    bool Grammar::ApplyTo(
        FPoptimizer_CodeTree::CodeTree& tree,
        bool recursion) const
    {
        bool changed = false;

#ifdef DEBUG_SUBSTITUTIONS
        if(!recursion)
        {
            std::cout << "Input:  ";
            DumpTree(tree);
            std::cout << "\n" << std::flush;
        }
#else
        recursion=recursion;
#endif
        if(tree.OptimizedUsing != this)
        {
            /* First optimize all children */
            for(size_t a=0; a<tree.Params.size(); ++a)
            {
                if( ApplyTo( *tree.Params[a].param, true ) )
                {
                    changed = true;
                }
            }

            /* Figure out which rules _may_ match this tree */
            typedef const Rule* ruleit;

            std::pair<ruleit, ruleit> range
                = MyEqualRange(pack.rlist + index,
                               pack.rlist + index + count,
                               tree,
                               OpcodeRuleCompare());

            while(range.first < range.second)
            {
                /* Check if this rule matches */
                if(range.first->ApplyTo(tree))
                {
                    changed = true;
                    break;
                }
                ++range.first;
            }

            if(!changed)
            {
                tree.OptimizedUsing = this;
            }
        }

#ifdef DEBUG_SUBSTITUTIONS
        if(!recursion)
        {
            std::cout << "Output: ";
            DumpTree(tree);
            std::cout << "\n" << std::flush;
        }
#endif
        return changed;
    }

    /* Store information about a potential match,
     * in order to iterate through candidates
     */
    struct MatchedParams::CodeTreeMatch
    {
        // Which parameters were matched -- these will be replaced if AnyParams are used
        std::vector<size_t> param_numbers;

        // Which values were saved for ImmedHolders?
        std::map<unsigned, double> ImmedMap;
        // Which codetrees were saved for each NameHolder? And how many?
        std::map<unsigned, std::pair<fphash_t, size_t> > NamedMap;
        // Which codetrees were saved for each RestHolder?
        std::map<unsigned,
          std::vector<fphash_t> > RestMap;

        // Examples of each codetree
        std::map<fphash_t, FPoptimizer_CodeTree::CodeTreeP> trees;

        CodeTreeMatch() : param_numbers(), ImmedMap(), NamedMap(), RestMap() { }
    };

#ifdef DEBUG_SUBSTITUTIONS
    void DumpMatch(const Function& input,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const MatchedParams& replacement,
                   const MatchedParams::CodeTreeMatch& matchrec,
                   bool DidMatch=true);
    void DumpFunction(const Function& input);
    void DumpParam(const ParamSpec& p);
    void DumpParams(const MatchedParams& mitem);
#endif

    /* Apply the rule to a given CodeTree */
    bool Rule::ApplyTo(
        FPoptimizer_CodeTree::CodeTree& tree) const
    {
        const Function&      input  = func;
        const MatchedParams& repl   = pack.mlist[repl_index];

        if(input.opcode == tree.Opcode)
        {
            for(unsigned long match_index=0; ; ++match_index)
            {
                MatchedParams::CodeTreeMatch matchrec;
                MatchResultType mr =
                    pack.mlist[input.index].Match(tree, matchrec,match_index, false);
                if(!mr.found && mr.has_more) continue;
                if(!mr.found) break;

    #ifdef DEBUG_SUBSTITUTIONS
                DumpMatch(input, tree, repl, matchrec);
    #endif

                const MatchedParams& params = pack.mlist[input.index];
                switch(type)
                {
                    case ReplaceParams:
                        repl.ReplaceParams(tree, params, matchrec);
    #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "  ParmReplace: ";
                        DumpTree(tree);
                        std::cout << "\n" << std::flush;
    #endif
                        return true;
                    case ProduceNewTree:
                        repl.ReplaceTree(tree,   params, matchrec);
    #ifdef DEBUG_SUBSTITUTIONS
                        std::cout << "  TreeReplace: ";
                        DumpTree(tree);
                        std::cout << "\n" << std::flush;
    #endif
                        return true;
                }
                break; // should be unreachable
            }
        }
        #ifdef DEBUG_SUBSTITUTIONS
        // Report mismatch
        MatchedParams::CodeTreeMatch matchrec;
        DumpMatch(input, tree, repl, matchrec, false);
        #endif
        return false;
    }


    /* Match the given function to the given CodeTree.
     */
    MatchResultType Function::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match,
        unsigned long match_index) const
    {
        if(opcode != tree.Opcode) return NoMatch;
        return pack.mlist[index].Match(tree, match, match_index, true);
    }


    /* This struct is used by MatchedParams::Match() for backtracking. */
    struct ParamMatchSnapshot
    {
        MatchedParams::CodeTreeMatch snapshot;
                                    // Snapshot of the state so far
        size_t            parampos; // Which position was last chosen?
        std::vector<bool> used;     // Which params were allocated?

        size_t            matchpos;
    };

    /* Match the given list of ParamSpecs using the given ParamMatchingType
     * to the given CodeTree.
     * The CodeTree is already assumed to be a function type
     * -- i.e. it is assumed that the caller has tested the Opcode of the tree.
     */
    MatchResultType MatchedParams::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match,
        unsigned long match_index,
        bool recursion) const
    {
        /*        match_index is a feature for backtracking.
         *
         *        For example,
         *          cMul (cAdd x) (cAdd x)
         *        Applied to:
         *          (a+b)*(c+b)
         *
         *        Match (cAdd x) to (a+b) may first capture "a" into "x",
         *        and then Match(cAdd x) for (c+b) will fail,
         *        because there's no "a" there.
         *
         *        However, match_index can be used to indicate that the
         *        _second_ matching will be used, so that "b" will be
         *        captured into "x".
         */


        /* First, check if the tree has any chances of matching... */
        /* Figure out what we need. */
        struct Needs
        {
            struct Needs_Pol
            {
                int SubTrees;
                int Others;
                unsigned SubTreesDetail[VarBegin];

                Needs_Pol(): SubTrees(0), Others(0), SubTreesDetail()
                {
                }
            } polarity[2]; // 0=positive, 1=negative
            int Immeds;

            Needs(): polarity(), Immeds() { }
        } NeedList;

        // Figure out what we need
        size_t minimum_need = 0;
        for(unsigned a=0; a<count; ++a)
        {
            const ParamSpec& param = pack.plist[index+a];
            Needs::Needs_Pol& needs = NeedList.polarity[param.sign];
            switch(param.opcode)
            {
                case SubFunction:
                    needs.SubTrees += 1;
                    assert( pack.flist[param.index].opcode < VarBegin );
                    needs.SubTreesDetail[ pack.flist[param.index].opcode ] += 1;
                    ++minimum_need;
                    break;
                case NumConstant:
                case ImmedHolder:
                default: // GroupFunction:
                    NeedList.Immeds += 1;
                    ++minimum_need;
                    break;
                case NamedHolder:
                    needs.Others += param.minrepeat;
                    ++minimum_need;
                    break;
                case RestHolder:
                    break;
            }
        }
        if(tree.Params.size() < minimum_need)
        {
            // Impossible to satisfy
            return NoMatch;
        }

        // Figure out what we have (note: we already assume that the opcode of the tree matches!)
        for(size_t a=0; a<tree.Params.size(); ++a)
        {
            Needs::Needs_Pol& needs = NeedList.polarity[tree.Params[a].sign];
            unsigned opcode = tree.Params[a].param->Opcode;
            switch(opcode)
            {
                case cImmed:
                    if(NeedList.Immeds > 0) NeedList.Immeds -= 1;
                    else needs.Others -= 1;
                    break;
                case cVar:
                case cFCall:
                case cPCall:
                    needs.Others -= 1;
                    break;
                default:
                    assert( opcode < VarBegin );
                    if(needs.SubTrees > 0
                    && needs.SubTreesDetail[opcode] > 0)
                    {
                        needs.SubTrees -= 1;
                        needs.SubTreesDetail[opcode] -= 1;
                    }
                    else needs.Others -= 1;
            }
        }

        // Check whether all needs were satisfied
        if(NeedList.Immeds > 0
        || NeedList.polarity[0].SubTrees > 0
        || NeedList.polarity[0].Others > 0
        || NeedList.polarity[1].SubTrees > 0
        || NeedList.polarity[1].Others > 0)
        {
            // Something came short, impossible to satisfy.
            return NoMatch;
        }

        if(type != AnyParams)
        {
            if(NeedList.Immeds < 0
            || NeedList.polarity[0].SubTrees < 0
            || NeedList.polarity[0].Others < 0
            || NeedList.polarity[1].SubTrees < 0
            || NeedList.polarity[1].Others < 0
            || count != tree.Params.size())
            {
                // Something was too much.
                return NoMatch;
            }
        }

        TransformationType transf = None;
        switch(tree.Opcode)
        {
            case cAdd: transf = Negate; break;
            case cMul: transf = Invert; break;
            case cAnd:
            case cOr:  transf = NotThe; break;
        }

        switch(type)
        {
            case PositionalParams:
            {
                /*DumpTree(tree);
                std::cout << "<->";
                DumpParams(*this);
                std::cout << " -- ";*/

                std::vector<MatchPositionSpec<CodeTreeMatch> > specs;
                specs.reserve(count);
                //fprintf(stderr, "Enter loop %lu\n", match_index);
                for(unsigned a=0; a<count; ++a)
                {
                    specs.resize(a+1);

                PositionalParamsMatchingLoop:;
                    // Match this parameter.
                    MatchResultType mr = pack.plist[index+a].Match(
                        *tree.Params[a].param, match,
                        tree.Params[a].sign ? transf : None,
                        specs[a].roundno);

                    specs[a].done = !mr.has_more;

                    // If it was not found, backtrack...
                    if(!mr.found)
                    {
                    LoopThisRound:
                        while(specs[a].done)
                        {
                            // Backtrack
                            if(a <= 0) return NoMatch; //
                            specs.resize(a);
                            --a;
                            match = specs[a].data;
                        }
                        ++specs[a].roundno;
                        goto PositionalParamsMatchingLoop;
                    }
                    // If found...
                    if(!recursion)
                        match.param_numbers.push_back(a);
                    specs[a].data = match;

                    if(a == count-1U && match_index > 0)
                    {
                        // Skip this match
                        --match_index;
                        goto LoopThisRound;
                    }
                }
                /*std::cout << " yay?\n";*/
                // Match = no mismatch.
                bool final_try = true;
                for(unsigned a=0; a<count; ++a)
                    if(!specs[a].done) { final_try = false; break; }
                //fprintf(stderr, "Exit  loop %lu\n", match_index);
                return MatchResultType(true, !final_try);
            }
            case AnyParams:
            case SelectedParams:
            {
                const size_t n_tree_params = tree.Params.size();

                unsigned N_PositiveRestHolders = 0;
                unsigned N_NegativeRestHolders = 0;
                for(unsigned a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.opcode == RestHolder)
                    {
                        if(param.sign)
                            ++N_NegativeRestHolders;
                        else
                            ++N_PositiveRestHolders;
                    }
                }

                bool HasRestHolders = N_PositiveRestHolders || N_NegativeRestHolders;

                #ifdef DEBUG_SUBSTITUTIONS
                if((type == AnyParams) && recursion && !HasRestHolders)
                {
                    std::cout << "Recursed AnyParams with no RestHolders?\n";
                    DumpParams(*this);
                }
                #endif

                if(!HasRestHolders && recursion && count != n_tree_params)
                {
                    /*DumpTree(tree);
                    std::cout << "<->";
                    DumpParams(*this);
                    std::cout << " -- fail due to recursion&&count!=n_tree_params";*/
                    return NoMatch; // Impossible match.
                }

                /*std::cout << "Matching ";
                DumpTree(tree); std::cout << " with ";
                DumpParams(*this);
                std::cout << " , match_index=" << match_index << "\n" << std::flush;*/

                std::vector<ParamMatchSnapshot> position(count);
                std::vector<bool>               used(n_tree_params);

                unsigned p=0;

                for(; p<count; ++p)
                {
                    position[p].snapshot  = match;
                    position[p].parampos  = 0;
                    position[p].matchpos  = 0;
                    position[p].used      = used;

                    //fprintf(stderr, "posA: p=%u count=%u\n", p, count);

                backtrack:
                  {
                    if(pack.plist[index+p].opcode == RestHolder)
                    {
                        // RestHolders always match. They're filled afterwards.
                        position[p].parampos = n_tree_params;
                        position[p].matchpos = 0;
                        continue;
                    }

                    size_t whichparam = position[p].parampos;
                    size_t whichmatch = position[p].matchpos;

                    /* a          = param index in the syntax specification
                     * whichparam = param index in the tree received from parser
                     */

                    /*fprintf(stderr, "posB: p=%u, whichparam=%lu, whichmatch=%lu\n",
                        p,whichparam,whichmatch);*/
                    while(whichparam < n_tree_params)
                    {
                        if(used[whichparam])
                        {
                        NextParamNumber:
                            ++whichparam;
                            whichmatch = 0;
                            continue;
                        NextMatchNumber:
                            ++whichmatch;
                        }

                        /*std::cout << "Maybe [" << p << "]:";
                        DumpParam(pack.plist[index+p]);
                        std::cout << " <-> ";
                        if(tree.Params[whichparam].sign) std::cout << '~';
                        DumpTree(*tree.Params[whichparam].param);
                        std::cout << "...?\n" << std::flush;*/

                        MatchResultType mr = pack.plist[index+p].Match(
                            *tree.Params[whichparam].param, match,
                            tree.Params[whichparam].sign ? transf : None,
                            whichmatch);

                        /*std::cout << "In ";
                        DumpTree(tree); std::cout << std::flush;
                        fprintf(stderr, ", trying param %lu, match %lu (matchindex %lu); got %s,%s: ",
                            whichparam,whichmatch, match_index,
                            mr.found?"found":"not found",
                            mr.has_more?"more":"no more"); fflush(stderr);
                        DumpParam(pack.plist[index+p]); std::cout << "\n" << std::flush;*/

                        if(!mr.found)
                        {
                        NextParamTest:
                            if(!mr.has_more) goto NextParamNumber;
                            goto NextMatchNumber;
                        }

                        /*std::cout << "woo... " << a << ", " << b << "\n";*/
                        /* NamedHolders require a special treatment,
                         * because a repetition count may be issued
                         * for them.
                         */
                        if(pack.plist[index+p].opcode == NamedHolder)
                        {
                            // Verify the MinRepeat & AnyRepeat case
                            unsigned MinRepeat = pack.plist[index+p].minrepeat;
                            bool AnyRepeat     = pack.plist[index+p].anyrepeat;
                            unsigned HadRepeat = 1;

                            for(size_t repeat_pos = whichparam+1;
                                repeat_pos < n_tree_params && (HadRepeat < MinRepeat || AnyRepeat);
                                ++repeat_pos)
                            {
                                /*fprintf(stderr, "Req @ %lu = %d:%16lX, got @ %lu = %d:%16lX\n",
                                    whichparam, tree.Params[whichparam].sign,
                                                tree.Params[whichparam].param->Hash,
                                    repeat_pos, tree.Params[repeat_pos].sign,
                                                tree.Params[repeat_pos].param->Hash);*/

                                if(tree.Params[repeat_pos].param->Hash
                                == tree.Params[whichparam].param->Hash
                                && tree.Params[repeat_pos].sign
                                == tree.Params[whichparam].sign
                                && !used[repeat_pos])
                                {
                                    ++HadRepeat;
                                }
                            }
                            /*fprintf(stderr, "Got repeat %u, needs %u\n", HadRepeat,MinRepeat);*/
                            if(HadRepeat < MinRepeat)
                            {
                                match = position[p].snapshot;
                                used  = position[p].used;
                                goto NextParamTest; // No sufficient repeat count here
                            }

                            used[whichparam] = true;
                            if(!recursion) match.param_numbers.push_back(whichparam);

                            HadRepeat = 1;
                            for(size_t repeat_pos = whichparam+1;
                                repeat_pos < n_tree_params && (HadRepeat < MinRepeat || AnyRepeat);
                                ++repeat_pos)
                            {
                                if(tree.Params[repeat_pos].param->Hash
                                == tree.Params[whichparam].param->Hash
                                && tree.Params[repeat_pos].sign
                                == tree.Params[whichparam].sign
                                && !used[repeat_pos])
                                {
                                    ++HadRepeat;
                                    used[repeat_pos] = true;
                                    if(!recursion) match.param_numbers.push_back(repeat_pos);
                                }
                            }
                            if(AnyRepeat)
                                match.NamedMap[pack.plist[index+p].index].second = HadRepeat;
                        }
                        else
                        {
                            used[whichparam] = true;
                            if(!recursion) match.param_numbers.push_back(whichparam);
                        }
                        position[p].parampos = mr.has_more ? whichparam : (whichparam+1);
                        position[p].matchpos = mr.has_more ? (whichmatch+1) : 0;
                        goto ok;
                    }

                    /*DumpParam(param);
                    std::cout << " didn't match anything in ";
                    DumpTree(tree);
                    std::cout << "\n";*/
                  }

                    // No match for this param, try backtracking.
                DiscardedThisAttempt:
                    while(p > 0)
                    {
                        --p;
                        ParamMatchSnapshot& prevpos = position[p];
                        if(prevpos.parampos < n_tree_params)
                        {
                            // Try another combination.
                            match = prevpos.snapshot;
                            used  = prevpos.used;
                            goto backtrack;
                        }
                    }
                    // If we cannot backtrack, break. No possible match.
                    /*if(!recursion)
                        std::cout << "Drats!\n";*/
                    if(match_index == 0)
                        return NoMatch;
                    break;
                ok:;
                    /*if(!recursion)
                        std::cout << "Match for param " << a << " at " << b << std::endl;*/

                    if(p == count-1U && match_index > 0)
                    {
                        // Skip this match
                        --match_index;
                        goto DiscardedThisAttempt;
                    }
                }
                /*fprintf(stderr, "End loop, match_index=%lu\n", match_index); fflush(stderr);*/

                /* We got a match. */

                // If the rule cares about the balance of
                // negative restholdings versus positive restholdings,
                // verify them.
                if(balance != BalanceDontCare)
                {
                    unsigned n_pos_restholdings = 0;
                    unsigned n_neg_restholdings = 0;

                    for(unsigned a=0; a<count; ++a)
                    {
                        const ParamSpec& param = pack.plist[index+a];
                        if(param.opcode == RestHolder)
                        {
                            for(size_t b=0; b<n_tree_params; ++b)
                                if(tree.Params[b].sign == param.sign && !used[b])
                                {
                                    if(param.sign)
                                        n_neg_restholdings += 1;
                                    else
                                        n_pos_restholdings += 1;
                                }
                        }
                    }
                    switch(balance)
                    {
                        case BalanceMoreNeg:
                            if(n_neg_restholdings <= n_pos_restholdings) return NoMatch;
                            break;
                        case BalanceMorePos:
                            if(n_pos_restholdings <= n_neg_restholdings) return NoMatch;
                            break;
                        case BalanceEqual:
                            if(n_pos_restholdings != n_neg_restholdings) return NoMatch;
                            break;
                        case BalanceDontCare: ;
                    }
                }

                unsigned pos_rest_remain = N_PositiveRestHolders;
                unsigned neg_rest_remain = N_NegativeRestHolders;

                // Verify if we have RestHolder constraints.
                for(unsigned a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.opcode == RestHolder)
                    {
                        std::map<unsigned, std::vector<fphash_t> >::iterator
                            i = match.RestMap.lower_bound(param.index);

                        if(i != match.RestMap.end() && i->first == param.index)
                        {
                            unsigned& n_remaining_restholders_of_this_kind =
                                param.sign ? neg_rest_remain : pos_rest_remain;
                            /*fprintf(stderr, "Does restholder %u match in", param.index);
                            fflush(stderr); DumpTree(tree); std::cout << "? " << std::flush;*/

                            const std::vector<fphash_t>& RefRestList = i->second;
                            for(size_t r=0; r<RefRestList.size(); ++r)
                            {
                                for(size_t b=0; b<n_tree_params; ++b)
                                    if(tree.Params[b].sign == param.sign
                                    && !used[b]
                                    && tree.Params[b].param->Hash == RefRestList[r])
                                    {
                                        used[b] = true;
                                        goto SatisfiedRestHolder;
                                    }
                                // Unsatisfied RestHolder constraint
                                /*fprintf(stderr, "- no\n");*/
                                p=count-1;
                                goto DiscardedThisAttempt;
                            SatisfiedRestHolder:;
                            }
                            --n_remaining_restholders_of_this_kind;
                            /*fprintf(stderr, "- yes\n");*/
                        }
                    }
                }

                // Now feed any possible RestHolders the remaining parameters.
                bool more_restholder_options = false;
                for(unsigned a=0; a<count; ++a)
                {
                    const ParamSpec& param = pack.plist[index+a];
                    if(param.opcode == RestHolder)
                    {
                        std::map<unsigned, std::vector<fphash_t> >::iterator
                            i = match.RestMap.lower_bound(param.index);
                        if(i != match.RestMap.end() && i->first == param.index) continue;

                        std::vector<fphash_t>& RestList = match.RestMap[param.index]; // mark it up

                        unsigned& n_remaining_restholders_of_this_kind =
                            param.sign ? neg_rest_remain : pos_rest_remain;

                        unsigned n_remaining_params = 0;
                        for(size_t b=0; b<n_tree_params; ++b)
                            if(tree.Params[b].sign == param.sign && !used[b])
                                ++n_remaining_params;

                        /*fprintf(stderr, "[index %lu] For restholder %u, %u remains, %u remaining of kind\n",
                            match_index,
                            (unsigned)param.index, (unsigned)n_remaining_params,
                            (unsigned)n_remaining_restholders_of_this_kind);
                            fflush(stderr);*/

                        if(n_remaining_params > 0)
                        {
                            if(n_remaining_params > 8) n_remaining_params = 8;
                            unsigned n_remaining_combinations = 1 << n_remaining_params;

                            unsigned n_options = n_remaining_restholders_of_this_kind > 1
                                ? n_remaining_combinations
                                : 1;
                            size_t selection = n_remaining_combinations - 1;
                            if(n_options > 1)
                            {
                                --n_options;
                                selection = match_index % (n_options); ++selection;
                                match_index /= n_options;
                            }
                            if(selection+1 < n_options) more_restholder_options = true;

                            /*fprintf(stderr, "- selected %u/%u\n", selection, n_options); fflush(stderr);*/

                            unsigned matchbit = 1;
                            for(size_t b=0; b<n_tree_params; ++b)
                                if(tree.Params[b].sign == param.sign && !used[b])
                                {
                                    if(selection & matchbit)
                                    {
                                        /*fprintf(stderr, "- uses param %lu\n", b);*/
                                        if(!recursion)
                                            match.param_numbers.push_back(b);
                                        fphash_t hash = tree.Params[b].param->Hash;
                                        RestList.push_back(hash);
                                        match.trees.insert(
                                            std::make_pair(hash, tree.Params[b].param) );

                                        used[b] = true;
                                    }
                                    if(matchbit < 0x80U) matchbit <<= 1;
                                }
                        }
                        --n_remaining_restholders_of_this_kind;
                    }
                }
                /*std::cout << "Returning match for ";
                DumpTree(tree);
                std::cout << "\n               with ";
                DumpParams(*this); std::cout << std::flush;
                fprintf(stderr, ", %s hope for more (now %lu)\n",
                    more_restholder_options ? "with" : "without", match_index); fflush(stderr);*/
                return more_restholder_options ? FoundSomeMatch : FoundLastMatch;
            }
        }
        return NoMatch;
    }

    MatchResultType ParamSpec::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams::CodeTreeMatch& match,
        TransformationType transf,
        unsigned long match_index) const
    {
        assert(opcode != RestHolder); // RestHolders are supposed to be handler by the caller

        switch(OpcodeType(opcode))
        {
            case NumConstant:
            {
                if(!tree.IsImmed()) return NoMatch;
                double res = tree.GetImmed();
                if(transformation == Negate) res = -res;
                if(transformation == Invert) res = 1/res;
                double res2 = GetPackConst(index);
                if(transf == Negate) res2 = -res2;
                if(transf == Invert) res2 = 1/res2;
                if(transf == NotThe) res2 = res2 != 0;
                if(res != res2) return NoMatch;
                return FoundLastMatch; // Previously unknown NumConstant, good
            }
            case ImmedHolder:
            {
                if(!tree.IsImmed()) return NoMatch;
                double res = tree.GetImmed();
                if(transformation == Negate) res = -res;
                if(transformation == Invert) res = 1/res;
                std::map<unsigned, double>::iterator
                    i = match.ImmedMap.lower_bound(index);
                if(i != match.ImmedMap.end() && i->first == index)
                {
                    double res2 = i->second;
                    if(transf == Negate) res2 = -res2;
                    if(transf == Invert) res2 = 1/res2;
                    if(transf == NotThe) res2 = res2 != 0;
                    return res == res2 ? FoundLastMatch : NoMatch;
                }
                if(sign != (transf != None)) return NoMatch;

                match.ImmedMap.insert(i, std::make_pair((unsigned)index, res));
                return FoundLastMatch; // Previously unknown ImmedHolder, good
            }
            case NamedHolder:
            {
                if(sign != (transf != None)) return NoMatch;
                std::map<unsigned, std::pair<fphash_t, size_t> >::iterator
                    i = match.NamedMap.lower_bound(index);
                if(i != match.NamedMap.end() && i->first == index)
                {
                    /*fprintf(stderr, "NamedHolder found: %16lX -- tested against %16lX\n", i->second.first, tree.Hash);*/
                    return tree.Hash == i->second.first
                           ? FoundLastMatch
                           : NoMatch;
                }
                match.NamedMap.insert(i, std::make_pair(index, std::make_pair(tree.Hash, 1)));
                match.trees.insert(std::make_pair(tree.Hash, &tree));
                return FoundLastMatch; // Previously unknown NamedHolder, good
            }
            case RestHolder:
            {
                break;
            }
            case SubFunction:
            {
                if(sign != (transf != None)) return NoMatch;
                return pack.flist[index].Match(tree, match, match_index);
            }
            default:
            {
                if(!tree.IsImmed()) return NoMatch;
                double res = tree.GetImmed();
                if(transformation == Negate) res = -res;
                if(transformation == Invert) res = 1/res;
                double res2;
                if(!GetConst(match, res2)) return NoMatch;
                if(transf == Negate) res2 = -res2;
                if(transf == Invert) res2 = 1/res2;
                if(transf == NotThe) res2 = res2 != 0;
                return res == res2 ? FoundLastMatch : NoMatch;
            }
        }
        return NoMatch;
    }

    bool ParamSpec::GetConst(
        const MatchedParams::CodeTreeMatch& match,
        double& result) const
    {
        switch(OpcodeType(opcode))
        {
            case NumConstant:
                result = GetPackConst(index);
                break;
            case ImmedHolder:
            {
                std::map<unsigned, double>::const_iterator
                    i = match.ImmedMap.find(index);
                if(i == match.ImmedMap.end()) return false; // impossible
                result = i->second;
                break;
            }
            case NamedHolder:
            {
                std::map<unsigned, std::pair<fphash_t, size_t> >::const_iterator
                    i = match.NamedMap.find(index);
                if(i == match.NamedMap.end()) return false; // impossible
                result = (double) i->second.second;
                break;
            }
            case RestHolder:
            {
                // Not enumerable
                return false;
            }
            case SubFunction:
            {
                // Not enumerable
                return false;
            }
            default:
            {
                switch(OPCODE(opcode))
                {
                    case cAdd:
                        result=0;
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp;
                            if(!pack.plist[index+p].GetConst(match, tmp)) return false;
                            result += tmp;
                        }
                        break;
                    case cMul:
                        result=1;
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp;
                            if(!pack.plist[index+p].GetConst(match, tmp)) return false;
                            result *= tmp;
                        }
                        break;
                    case cMin:
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp;
                            if(!pack.plist[index+p].GetConst(match, tmp)) return false;
                            if(p == 0 || tmp < result) result = tmp;
                        }
                        break;
                    case cMax:
                        for(unsigned p=0; p<count; ++p)
                        {
                            double tmp;
                            if(!pack.plist[index+p].GetConst(match, tmp)) return false;
                            if(p == 0 || tmp > result) result = tmp;
                        }
                        break;
                    case cSin: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::sin(result); break;
                    case cCos: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::cos(result); break;
                    case cTan: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::tan(result); break;
                    case cAsin: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::asin(result); break;
                    case cAcos: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::acos(result); break;
                    case cAtan: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::atan(result); break;
                    case cSinh: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::sinh(result); break;
                    case cCosh: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::cosh(result); break;
                    case cTanh: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = std::tanh(result); break;
#ifndef FP_NO_ASINH
                    case cAsinh: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = asinh(result); break;
                    case cAcosh: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = acosh(result); break;
                    case cAtanh: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = atanh(result); break;
#endif
                    case cCeil: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::ceil(result); break;
                    case cFloor: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = std::floor(result); break;
                    case cLog: if(!pack.plist[index].GetConst(match, result))return false;
                               result = std::log(result); break;
                    case cLog2: if(!pack.plist[index].GetConst(match, result))return false;
                                result = std::log(result) * CONSTANT_L2I;
                                //result = std::log2(result);
                                break;
                    case cLog10: if(!pack.plist[index].GetConst(match, result))return false;
                                 result = std::log10(result); break;
                    case cPow:
                    {
                        if(!pack.plist[index+0].GetConst(match, result))return false;
                        double tmp;
                        if(!pack.plist[index+1].GetConst(match, tmp))return false;
                        result = std::pow(result, tmp);
                        break;
                    }
                    case cMod:
                    {
                        if(!pack.plist[index+0].GetConst(match, result))return false;
                        double tmp;
                        if(!pack.plist[index+1].GetConst(match, tmp))return false;
                        result = std::fmod(result, tmp);
                        break;
                    }
                    default:
                        return false;
                }
            }
        }
        if(transformation == Negate) result = -result;
        if(transformation == Invert) result = 1.0 / result;
        return true;
    }

    void MatchedParams::SynthesizeTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        for(unsigned a=0; a<count; ++a)
        {
            const ParamSpec& param = pack.plist[index+a];
            if(param.opcode == RestHolder)
            {
                // Add children directly to this tree
                param.SynthesizeTree(tree, matcher, match);
            }
            else
            {
                FPoptimizer_CodeTree::CodeTree* subtree = new FPoptimizer_CodeTree::CodeTree;
                param.SynthesizeTree(*subtree, matcher, match);
                subtree->Sort();
                subtree->Recalculate_Hash_NoRecursion(); // rehash this, but not the children, nor the parent
                FPoptimizer_CodeTree::CodeTree::Param p(subtree, param.sign) ;
                tree.AddParam(p);
            }
        }
    }

    void MatchedParams::ReplaceParams(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        // Replace the 0-level params indicated in "match" with the ones we have

        // First, construct the tree recursively using the "match" info
        SynthesizeTree(tree, matcher, match);

        // Remove the indicated params
        std::sort(match.param_numbers.begin(), match.param_numbers.end());
        for(size_t a=match.param_numbers.size(); a-->0; )
        {
            size_t num = match.param_numbers[a];
            tree.DelParam(num);
        }
        tree.Sort();
        tree.Rehash(true); // rehash this and its parents, but not its children
    }

    void MatchedParams::ReplaceTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        CodeTreeMatch& match) const
    {
        // Replace the entire tree with one indicated by our Params[0]
        // Note: The tree is still constructed using the holders indicated in "match".
        std::vector<FPoptimizer_CodeTree::CodeTree::Param> OldParams = tree.Params;
        tree.Params.clear();
        pack.plist[index].SynthesizeTree(tree, matcher, match);

        tree.Sort();
        tree.Rehash(true);  // rehash this and its parents, but not its children
    }

    /* Synthesizes a new tree based on the given information
     * in ParamSpec. Assume the tree is empty, don't deallocate
     * anything. Don't touch Hash, Parent.
     */
    void ParamSpec::SynthesizeTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams& matcher,
        MatchedParams::CodeTreeMatch& match) const
    {
        switch(SpecialOpcode(opcode))
        {
            case RestHolder:
            {
                std::map<unsigned, std::vector<fphash_t> >
                    ::const_iterator i = match.RestMap.find(index);

                assert(i != match.RestMap.end());

                /*std::cout << std::flush;
                fprintf(stderr, "Restmap %u, sign %d, size is %u -- params %u\n",
                    (unsigned) i->first, sign, (unsigned) i->second.size(),
                    (unsigned) tree.Params.size());*/

                for(size_t a=0; a<i->second.size(); ++a)
                {
                    fphash_t hash = i->second[a];

                    std::map<fphash_t, FPoptimizer_CodeTree::CodeTreeP>
                        ::const_iterator j = match.trees.find(hash);

                    assert(j != match.trees.end());

                    FPoptimizer_CodeTree::CodeTree* subtree = j->second->Clone();
                    FPoptimizer_CodeTree::CodeTree::Param p(subtree, sign);
                    tree.AddParam(p);
                }
                /*fprintf(stderr, "- params size became %u\n", (unsigned)tree.Params.size());
                fflush(stderr);*/
                break;
            }
            case SubFunction:
            {
                const Function& fitem = pack.flist[index];
                tree.Opcode = fitem.opcode;
                const MatchedParams& mitem = pack.mlist[fitem.index];
                mitem.SynthesizeTree(tree, matcher, match);
                break;
            }
            case NamedHolder:
                if(!anyrepeat && minrepeat == 1)
                {
                    /* Literal parameter */
                    std::map<unsigned, std::pair<fphash_t, size_t> >
                        ::const_iterator i = match.NamedMap.find(index);

                    assert(i != match.NamedMap.end());

                    fphash_t hash = i->second.first;

                    std::map<fphash_t, FPoptimizer_CodeTree::CodeTreeP>
                        ::const_iterator j = match.trees.find(hash);

                    assert(j != match.trees.end());

                    tree.Opcode = j->second->Opcode;
                    switch(tree.Opcode)
                    {
                        case cImmed: tree.Value = j->second->Value; break;
                        case cVar:   tree.Var   = j->second->Var;  break;
                        case cFCall:
                        case cPCall: tree.Funcno = j->second->Funcno; break;
                    }

                    tree.SetParams(j->second->Params);
                    break;
                }
                // passthru; x+ is synthesized as the number, not as the tree
            case NumConstant:
            case ImmedHolder:
            default:
                tree.Opcode = cImmed;
                GetConst(match, tree.Value); // note: return value is ignored
                break;
        }
    }

#ifdef DEBUG_SUBSTITUTIONS
    void DumpParam(const ParamSpec& p)
    {
        //std::cout << "/*p" << (&p-pack.plist) << "*/";

        if(p.sign) std::cout << '~';
        if(p.transformation == Negate) std::cout << '-';
        if(p.transformation == Invert) std::cout << '/';

        switch(SpecialOpcode(p.opcode))
        {
            case NumConstant: std::cout << GetPackConst(p.index); break;
            case ImmedHolder: std::cout << ImmedHolderNames[p.index]; break;
            case NamedHolder: std::cout << NamedHolderNames[p.index]; break;
            case RestHolder: std::cout << '<' << p.index << '>'; break;
            case SubFunction: DumpFunction(pack.flist[p.index]); break;
            default:
            {
                std::string opcode = FP_GetOpcodeName(p.opcode).substr(1);
                for(size_t a=0; a<opcode.size(); ++a) opcode[a] = std::toupper(opcode[a]);
                std::cout << opcode << '(';
                for(unsigned a=0; a<p.count; ++a)
                {
                    if(a > 0) std::cout << ' ';
                    DumpParam(pack.plist[p.index+a]);
                }
                std::cout << " )";
            }
        }
        if(p.anyrepeat && p.minrepeat==1) std::cout << '*';
        if(p.anyrepeat && p.minrepeat==2) std::cout << '+';
    }

    void DumpParams(const MatchedParams& mitem)
    {
        //std::cout << "/*m" << (&mitem-pack.mlist) << "*/";

        if(mitem.type == PositionalParams) std::cout << '[';
        if(mitem.type == SelectedParams) std::cout << '{';

        for(unsigned a=0; a<mitem.count; ++a)
        {
            std::cout << ' ';
            DumpParam(pack.plist[mitem.index + a]);
        }

        switch(mitem.balance)
        {
            case BalanceMorePos: std::cout << " =+"; break;
            case BalanceMoreNeg: std::cout << " =-"; break;
            case BalanceEqual:   std::cout << " =="; break;
            case BalanceDontCare: break;
        }

        if(mitem.type == PositionalParams) std::cout << " ]";
        if(mitem.type == SelectedParams) std::cout << " }";
    }

    void DumpFunction(const Function& fitem)
    {
        //std::cout << "/*f" << (&fitem-pack.flist) << "*/";

        std::cout << '(' << FP_GetOpcodeName(fitem.opcode);
        DumpParams(pack.mlist[fitem.index]);
        std::cout << ')';
    }
    void DumpTree(const FPoptimizer_CodeTree::CodeTree& tree)
    {
        //std::cout << "/*" << tree.Depth << "*/";
        const char* sep2 = "";
        //std::cout << '[' << std::hex << tree.Hash << ']' << std::dec;
        switch(tree.Opcode)
        {
            case cImmed: std::cout << tree.Value; return;
            case cVar:   std::cout << "Var" << tree.Var; return;
            case cAdd: sep2 = " +"; break;
            case cMul: sep2 = " *"; break;
            case cAnd: sep2 = " &"; break;
            case cOr: sep2 = " |"; break;
            default:
                std::cout << FP_GetOpcodeName(tree.Opcode);
                if(tree.Opcode == cFCall || tree.Opcode == cPCall)
                    std::cout << ':' << tree.Funcno;
        }
        std::cout << '(';
        if(tree.Params.size() <= 1 && *sep2) std::cout << (sep2+1) << ' ';
        for(size_t a=0; a<tree.Params.size(); ++a)
        {
            if(a > 0) std::cout << ' ';
            if(tree.Params[a].sign) std::cout << '~';

            DumpTree(*tree.Params[a].param);

            if(tree.Params[a].param->Parent != &tree)
            {
                std::cout << "(?""?""?))";
            }

            if(a+1 < tree.Params.size()) std::cout << sep2;
        }
        std::cout << ')';
    }
    void DumpMatch(const Function& input,
                   const FPoptimizer_CodeTree::CodeTree& tree,
                   const MatchedParams& replacement,
                   const MatchedParams::CodeTreeMatch& matchrec,
                   bool DidMatch)
    {
        std::cout <<
            "Found " << (DidMatch ? "match" : "mismatch") << ":\n"
            "  Pattern    : ";
        DumpFunction(input);
        std::cout << "\n"
            "  Replacement: ";
        DumpParams(replacement);
        std::cout << "\n";

        std::cout <<
            "  Tree       : ";
        DumpTree(tree);
        std::cout << "\n";

        for(std::map<unsigned, std::pair<fphash_t, size_t> >::const_iterator
            i = matchrec.NamedMap.begin(); i != matchrec.NamedMap.end(); ++i)
        {
            std::cout << "           " << NamedHolderNames[i->first] << " = ";
            DumpTree(*matchrec.trees.find(i->second.first)->second);
            std::cout << " (" << i->second.second << " matches)\n";
        }

        for(std::map<unsigned, double>::const_iterator
            i = matchrec.ImmedMap.begin(); i != matchrec.ImmedMap.end(); ++i)
        {
            std::cout << "           " << ImmedHolderNames[i->first] << " = ";
            std::cout << i->second << std::endl;
        }

        for(std::map<unsigned, std::vector<fphash_t> >::const_iterator
            i = matchrec.RestMap.begin(); i != matchrec.RestMap.end(); ++i)
        {
            for(size_t a=0; a<i->second.size(); ++a)
            {
                fphash_t hash = i->second[a];
                std::cout << "         <" << i->first << "> = ";
                DumpTree(*matchrec.trees.find(hash)->second);
                std::cout << std::endl;
            }
            if(i->second.empty())
                std::cout << "         <" << i->first << "> = <empty>\n";
        }
        std::cout << std::flush;
    }
#endif
}

#endif
