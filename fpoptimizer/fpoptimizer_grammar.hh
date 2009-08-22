#define FPOPT_NAN_CONST (-1712345.25) /* Would use 0.0 / 0.0 here, but some compilers don't accept it. */

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
        Invert,  // 1/x
        NotThe   // !x
    };

    enum ImmedConstraint_Value
    {
        ValueMask = 0x07,
        Value_AnyNum     = 0x0, // any value
        Value_EvenInt    = 0x1, // any even integer (0,2,4, etc)
        Value_OddInt     = 0x2, // any odd integer (1,3, etc)
        Value_IsInteger  = 0x3, // any integer-value (excludes e.g. 0.2)
        Value_NonInteger = 0x4  // any non-integer (excludes e.g. 1 or 5)
    };
    enum ImmedConstraint_Sign
    {
        SignMask  = 0x18,
        Sign_AnySign     = 0x00, // - or +
        Sign_Positive    = 0x08, // positive only
        Sign_Negative    = 0x10  // negative only
    };
    enum ImmedConstraint_Oneness
    {
        OnenessMask   = 0x60,
        Oneness_Any      = 0x00,
        Oneness_One      = 0x20, // +1 or -1
        Oneness_NotOne   = 0x40  // anything but +1 or -1
    };

    enum SpecialOpcode
    {
        NumConstant = 0xFFFB, // Holds a particular value (syntax-time constant)
        ImmedHolder,          // Holds a particular immed
        NamedHolder,          // Holds a particular named param (of any kind)
        SubFunction,          // Holds an opcode and the params
        RestHolder            // Holds anything else
      //GroupFunction         // For parse-time functions
    };

    enum ParamMatchingType
    {
        PositionalParams, // this set of params in this order
        SelectedParams,   // this set of params in any order
        AnyParams         // these params are included
    };

    enum RuleType
    {
        ProduceNewTree, // replace self with the first (and only) from replaced_param
        ReplaceParams   // replace indicate params with replaced_params
    };

    enum SignBalanceType
    {
        BalanceDontCare,
        BalanceMoreNeg,
        BalanceMorePos,
        BalanceEqual
    };

    struct MatchResultType
    {
        bool found:16;
        bool has_more:16;

        MatchResultType(bool f,bool m) : found(f),has_more(m) { }
    };
    static const MatchResultType
        NoMatch(false,false),       // No match, don't try to increment match_index
        TryMore(false,true),        // No match, but try to increment match_index
        FoundSomeMatch(true,true),  // Found match, but we may have more
        FoundLastMatch(true,false); // Found match, don't have more

    // For iterating through match candidates
    template<typename Payload>
    struct MatchPositionSpec
    {
        unsigned roundno;
        bool     done;
        Payload  data;
        MatchPositionSpec() : roundno(0), done(false), data() { }
    };

    /***/

#ifdef __GNUC__
# define PACKED_GRAMMAR_ATTRIBUTE __attribute__((packed))
#else
# define PACKED_GRAMMAR_ATTRIBUTE
#endif

    struct MatchedParams
    {
        ParamMatchingType type    : 6;
        SignBalanceType   balance : 2;
        // count,index to plist[]
        unsigned         count : 8;
        unsigned         index : 16;

        struct CodeTreeMatch;

        MatchResultType
            Match(FPoptimizer_CodeTree::CodeTree& tree,
                  CodeTreeMatch& match,
                  unsigned long match_index,
                  bool recursion) const;

        void ReplaceParams(FPoptimizer_CodeTree::CodeTree& tree,
                           const MatchedParams& matcher, CodeTreeMatch& match) const;

        void ReplaceTree(FPoptimizer_CodeTree::CodeTree& tree,
                         const MatchedParams& matcher, CodeTreeMatch& match) const;

        void SynthesizeTree(
            FPoptimizer_CodeTree::CodeTree& tree,
            const MatchedParams& matcher,
            MatchedParams::CodeTreeMatch& match) const;
    } PACKED_GRAMMAR_ATTRIBUTE;

    struct ParamSpec
    {
        OpcodeType opcode : 16;
        bool     sign     : 1;
        TransformationType
           transformation  : 3;
        unsigned minrepeat : 3;
        bool     anyrepeat : 1;

        // For NumConstant:   index to clist[]
        // For ImmedHolder:   index is the slot, count is an ImmedConstraint
        // For RestHolder:    index is the slot
        // For NamedHolder:   index is the slot, count is an ImmedConstraint
        // For SubFunction:   index to flist[]
        // For anything else
        //  =  GroupFunction: index,count to plist[]
        unsigned count : 8;
        unsigned index : 16;

        MatchResultType Match(
            FPoptimizer_CodeTree::CodeTree& tree,
            MatchedParams::CodeTreeMatch& match,
            TransformationType transf,
            unsigned long match_index) const;

        bool GetConst(
            const MatchedParams::CodeTreeMatch& match,
            double& result) const;

        void SynthesizeTree(
            FPoptimizer_CodeTree::CodeTree& tree,
            const MatchedParams& matcher,
            MatchedParams::CodeTreeMatch& match) const;
    } PACKED_GRAMMAR_ATTRIBUTE;
    struct Function
    {
        OpcodeType opcode : 16;
        // index to mlist[]
        unsigned   index  : 16;

        MatchResultType
            Match(FPoptimizer_CodeTree::CodeTree& tree,
                  MatchedParams::CodeTreeMatch& match,
                  unsigned long match_index) const;
    } PACKED_GRAMMAR_ATTRIBUTE;
    struct Rule
    {
        unsigned  n_minimum_params : 8;
        RuleType  type             : 8;
        // index to mlist[]
        unsigned  repl_index       : 16;

        Function  func;

        bool ApplyTo(FPoptimizer_CodeTree::CodeTree& tree) const;
    } PACKED_GRAMMAR_ATTRIBUTE;
    struct Grammar
    {
        // count,index to rlist[]
        unsigned index : 16;
        unsigned count : 16;

        bool ApplyTo(FPoptimizer_CodeTree::CodeTree& tree,
                     bool recursion=false) const;
    } PACKED_GRAMMAR_ATTRIBUTE;

    extern const struct GrammarPack
    {
        const double*         clist;
        const ParamSpec*      plist;
        const MatchedParams*  mlist;
        const Function*       flist;
        const Rule*           rlist;
        Grammar               glist[4];
    } pack;
}
