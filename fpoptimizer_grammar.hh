#include <set>
#include <stdint.h> /* for uint_fast64_t */

namespace FPoptimizer_CodeTree
{
    class CodeTree;
}

namespace FPoptimizer_Grammar
{
    typedef unsigned OpcodeType;

    enum TransformationType
    {
        None,    // default
        Negate,  // 0-x
        Invert   // 1/x
    };

    enum SpecialOpcode
    {
        NumConstant = 0xFFFB, // Holds a particular value (syntax-time constant)
        ImmedHolder,          // Holds a particular immed
        NamedHolder,          // Holds a particular named param (of any kind)
        RestHolder,           // Holds anything else
        SubFunction           // Holds an opcode and the params
      //GroupFunction         // For parse-time functions
    };

    enum ParamMatchingType
    {
        PositionalParams,
        AnyParams
    };

    enum RuleType
    {
        ProduceNewTree, // replace self with the first (and only) from replaced_param
        ReplaceParams   // replace indicate params with replaced_params
    };

    /***/

    struct MatchedParams
    {
        ParamMatchingType type : 8;
        // count,index to plist[]
        unsigned         count : 8;
        unsigned         index : 16;

        struct CodeTreeMatch;

        bool Match(FPoptimizer_CodeTree::CodeTree& tree,
                   CodeTreeMatch& match,
                   bool recursion = true) const;

        void ReplaceParams(FPoptimizer_CodeTree::CodeTree& tree,
                           const MatchedParams& matcher, CodeTreeMatch& match) const;

        void ReplaceTree(FPoptimizer_CodeTree::CodeTree& tree,
                         const MatchedParams& matcher, CodeTreeMatch& match) const;
    };

    struct ParamSpec
    {
        OpcodeType opcode : 16;
        bool     sign     : 1;
        TransformationType
           transformation  : 3;
        unsigned minrepeat : 3;
        bool     anyrepeat : 1;

        // For NumConstant:   index to clist[]
        // For ImmedHolder:   index is the slot
        // For RestHolder:    index is the slot
        // For NamedHolder:   index is the slot
        // For SubFunction:   index to flist[]
        // For anything else
        //  =  GroupFunction: index,count to plist[]
        unsigned count : 8;
        unsigned index : 16;

        bool Match(
            FPoptimizer_CodeTree::CodeTree& tree,
            MatchedParams::CodeTreeMatch& match) const;

        double GetConst(
            MatchedParams::CodeTreeMatch& match,
            bool& impossible) const;

        void SynthesizeTree(
            FPoptimizer_CodeTree::CodeTree& tree,
            const MatchedParams& matcher,
            MatchedParams::CodeTreeMatch& match) const;
    };
    struct Function
    {
        OpcodeType opcode : 16;
        // index to mlist[]
        unsigned   index  : 16;

        bool Match(FPoptimizer_CodeTree::CodeTree& tree,
                   MatchedParams::CodeTreeMatch& match) const;
    };
    struct Rule
    {
        unsigned  n_minimum_params : 8;
        RuleType  type             : 8;
        // index to mlist[]
        unsigned  repl_index       : 16;

        Function  func;

        bool ApplyTo(FPoptimizer_CodeTree::CodeTree& tree) const;
    };
    struct Grammar
    {
        // count,index to rlist[]
        unsigned index : 16;
        unsigned count : 16;

        bool ApplyTo(std::set<uint_fast64_t>& optimized_children,
                     FPoptimizer_CodeTree::CodeTree& tree,
                     bool recursion=false) const;
    };

    extern const struct GrammarPack
    {
        const double*         clist;
        const ParamSpec*      plist;
        const MatchedParams*  mlist;
        const Function*       flist;
        const Rule*           rlist;
        Grammar               glist[3];
    } pack;
}
