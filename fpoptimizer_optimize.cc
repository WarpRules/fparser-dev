#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_codetree.hh"

#include <algorithm>

namespace FPoptimizer_Grammar
{
    struct OpcodeRuleCompare
    {
        bool operator() (unsigned opcode, const RuleType_Const& rule) const
        {
            return opcode < pack.flist[rule.input_index].opcode;
        }
        bool operator() (const RuleType_Const& rule, unsigned opcode) const
        {
            return pack.flist[rule.input_index].opcode < opcode;
        }
    };

    bool Grammar_Const::ApplyTo(
        std::set<uint_fast64_t>& optimized_children,
        FPoptimizer_CodeTree::CodeTree& tree,
        bool child_triggered) const
    {
        bool changed_once = false;

        if(optimized_children.find(tree.Hash) == optimized_children.end())
        {
            for(;;)
            {
                bool changed = false;

                if(!child_triggered)
                {
                    /* First optimize all children */
                    for(size_t a=0; a<tree.Params.size(); ++a)
                    {
                        if( ApplyTo( optimized_children, *tree.Params[a].param ) )
                        {
                            changed = true;
                        }
                    }
                }

                /* Figure out which rules _may_ match this tree */
                typedef const RuleType_Const* ruleit;

                std::pair<ruleit, ruleit> range
                    = std::equal_range(pack.rlist + index,
                                       pack.rlist + index + count,
                                       tree.Opcode,
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

                if(!changed) break;

                // If we had a change at this point, mark up that we had one, and try again
                changed_once = true;
            }

            optimized_children.insert(tree.Hash);
        }

        /* If any changes whatsoever were done, recurse the optimization to parents */
        if((child_triggered || changed_once) && tree.Parent)
        {
            ApplyTo( optimized_children, *tree.Parent, true );
            /* As this step may cause the tree we were passed to actually not exist,
             * don't touch the tree after this.
             *
             * FIXME: Is it even safe at all?
             */
        }

        return changed_once;
    }

    /* Store information about a potential match,
     * in order to iterate through candidates
     */
    struct MatchedParams_Const::CodeTreeMatch
    {
        //
    };
    
    bool RuleType_Const::ApplyTo(
        FPoptimizer_CodeTree::CodeTree& tree) const
    {
        const FunctionType_Const&  input  = pack.flist[input_index];
        const MatchedParams_Const& params = pack.mlist[input.index];
        const MatchedParams_Const& repl   = pack.mlist[repl_index];
        
        // Simplest verifications first
        if(input.opcode != tree.Opcode) return false;

        MatchedParams_Const::CodeTreeMatch matchrec;
        
        if(params.Match(tree, matchrec))
        {
            switch(type)
            {
                case ReplaceParams:
                    repl.ReplaceParams(tree, params, matchrec);
                    return true;
                case ProduceNewTree:
                    repl.ReplaceTree(tree,   params, matchrec);
                    return true;
            }
        }
        return false;
    }

    bool MatchedParams_Const::Match(
        FPoptimizer_CodeTree::CodeTree& tree,
        MatchedParams_Const::CodeTreeMatch& match) const
    {
        return false;
    }

    void MatchedParams_Const::ReplaceParams(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams_Const& matcher,
        MatchedParams_Const::CodeTreeMatch& match) const
    {
        // Replace the 0-level params indicated in "match" with the ones we have
    }

    void MatchedParams_Const::ReplaceTree(
        FPoptimizer_CodeTree::CodeTree& tree,
        const MatchedParams_Const& matcher,
        CodeTreeMatch& match) const
    {
        // Replace the entire tree with one indicated by our Params[0]
        // Note: The tree is still constructed using the holders indicated in "match".
    }
}
