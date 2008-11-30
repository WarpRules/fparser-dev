#include "fpoptimizer_grammar.hh"
#include "fpoptimizer_codetree.hh"

#include <algorithm>

namespace FPoptimizer_Grammar
{
    struct OpcodeRuleCompare
    {
        bool operator() (unsigned opcode, const Rule& rule) const
        {
            return opcode < rule.Input.Opcode;
        }
        bool operator() (const Rule& rule, unsigned opcode) const
        {
            return rule.Input.Opcode < opcode;
        }
    };
    
    bool Grammar::ApplyTo(FPoptimizer_CodeTree::CodeTree& tree, bool child_triggered) const
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
                        if( ApplyTo( *tree.Params[a].param ) )
                        {
                            changed = true;
                        }
                    }
                }
                
                /* Figure out which rules _may_ match this tree */
                typedef std::vector<Rule>::const_iterator ruleit;
                std::pair<ruleit, ruleit> range = std::equal_range(rules.begin(), rules.end(), tree.Opcode,
                                                                   OpcodeRuleCompare());
                while(range.first < range.second)
                {
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
            ApplyTo(*tree.Parent, true);
            /* As this step may cause the tree we were passed to actually not exist,
             * don't touch the tree after this.
             *
             * FIXME: Is it even safe at all?
             */
        }
        
        return changed_once;
    }

    bool Rule::ApplyTo(FPoptimizer_CodeTree::CodeTree& tree) const
    {
        // Simplest verifications first
        if(Input.Opcode != tree.Opcode) return false;
        
        MatchedParams::CodeTreeMatch matchrec;
        
        if(Input.Params.Match(tree, matchrec))
        {
            switch(Type)
            {
                case ReplaceParams:
                    Replacement.ReplaceParams(tree, Input.Params, matchrec);
                    return true;
                case ProduceNewTree:
                    Replacement.ReplaceTree(tree,   Input.Params, matchrec);
                    return true;
            }
        }
        return false;
    }

    bool MatchedParams::Match(FPoptimizer_CodeTree::CodeTree& tree, CodeTreeMatch& match) const
    {
        return false;
    }
    
    void MatchedParams::ReplaceParams(FPoptimizer_CodeTree::CodeTree& tree, const MatchedParams& matcher, CodeTreeMatch& match) const
    {
        // Replace the 0-level params indicated in "match" with the ones we have
    }
    
    void MatchedParams::ReplaceTree(FPoptimizer_CodeTree::CodeTree& tree, const MatchedParams& matcher, CodeTreeMatch& match) const
    {
        // Replace the entire tree with one indicated by our Params[0]
        // Note: The tree is still constructed using the holders indicated in "match".
    }
}
