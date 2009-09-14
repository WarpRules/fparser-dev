#define FPOPT_NAN_CONST (-1712345.25) /* Would use 0.0 / 0.0 here, but some compilers don't accept it. */

namespace FPoptimizer_CodeTree
{
    class CodeTree;
}

namespace FPoptimizer_Grammar
{
    typedef unsigned OpcodeType;

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
        Sign_Negative    = 0x10, // negative only
        Sign_NoIdea      = 0x18  // where sign cannot be guessed
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
        /* On the value of NumConstant:
         * The value must be different from any of
         * the values in FUNCTIONPARSERTYPES::OPCODE,
         * and it must be small enough that the last
         * value within SpecialOpcode fits within the
         * bits allocated for opcode in ParamSpec.
         * That is, as of this writing,
         * 64 < NumConstant <= (0x100-5).
         * This rule is verified by fpoptimizer_grammar_gen.y
         * using a few assert()s in the beginning of its main().
         */
        NumConstant = 0xFB, // Holds a particular value (syntax-time constant)
        ImmedHolder,        // Holds a particular immed
        NamedHolder,        // Holds a particular named param (of any kind)
        SubFunction,        // Holds an opcode and the params
        RestHolder          // Holds anything else
      //GroupFunction       // For parse-time functions
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

    struct MatchResultType
    {
        //bool found:16;
        //bool has_more:16;
        // msvc doesn't like the above
        bool found;
        bool has_more;

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

    /* A MatchedParams object describes
     * either how to match the parameters (leaves, nodes) of the tree
     * or how to synthesize them.
     */
    struct MatchedParams
    {
        /* When matching, type describes the method of matching.
         *
         *               Sample input tree:      (cOr 2 3)  (cOr 2 4) (cOr 3 2) (cOr 4 2 3) (cOr 2)
         * Possible methods:
         *    PositionalParams, e.g. (cOr [2 3]):  match     no match  no match  no match   no match
         *      The nodes described here are
         *      to be matched, in this order.
         *    SelectedParams,   e.g. (cOr {2 3}):  match     no match   match    no match   no match
         *      The nodes described here are
         *      to be matched, in any order.
         *    AnyParams,        e.g. (cOr 2 3  ):  match     no match   match     match     no match
         *      At least the nodes described here
         *      are to be matched, in any order.
         * When synthesizing, the type is ignored.
         */
        ParamMatchingType type : 2; // needs 2

        /* ParamSpec objects from pack.plist[index] .. pack.plist[index+count]
         * are matched / synthesized.
         */
        unsigned         count : 8;   // needs 2
        unsigned         index : 16;  // needs 10

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

    /* A ParamSpec object describes
     * either a parameter (leaf, node) that must be matched,
     * or a parameter (leaf, node) that must be synthesized.
     */
    struct ParamSpec
    {
        /* The opcode field of the ParamSpec has the following
         * possible values (from enum SpecialOpcode):
         *   NumConstant:
         *      this describes a specific constant value
         *      that must be matched / synthesized.
         *      The value is found in pack.clist[index]
         *   ImmedHolder:
         *      this describes any constant value
         *      that must be matched / synthesized.
         *      "index" is the ID of the ImmedHolder.
         *      In matching, all ImmedHolders having the same ID
         *      must evaluate to the same constant value.
         *      In synthesizing, the constant value matched by
         *      an ImmedHolder with this ID must be synthesized.
         *      "count" is a bitmask describing the constraints
         *      specific to this immediate value, interpreted
         *      according to ImmedConstraint rules.
         *   NamedHolder:
         *      this describes any node
         *      that must be matched / synthesized.
         *      "index" is the ID of the NamedHolder:
         *      In matching, all NamedHolders having the same ID
         *      must match the identical node.
         *      In synthesizing, the node matched by
         *      a NamedHolder with this ID must be synthesized.
         *      "count" is a bitmask describing the constraints
         *      specific to this node, interpreted according
         *      to ImmedConstraint rules.
         *   RestHolder:
         *      this describes a set of node
         *      that must be matched / synthesized.
         *      "index" is the ID of the RestHolder:
         *      In matching, all RestHolders having the same ID
         *      must match the same set of nodes.
         *      In synthesizing, all nodes matched by
         *      the RestHolder with this ID must be synthesized.
         *    SubFunction:
         *      this describes a subtree
         *      that must be matched / synthesized.
         *      The subtree is described in pack.flist[index].
         *      "count" is a bitmask describing the constraints
         *      specific to this subtree, interpreted according
         *      to ImmedConstraint rules.
         *    Anything else (GroupFunction):
         *      this describes a constant value
         *      that must be matched / synthesized.
         *      The opcode field (from fparser's OPCODE enum)
         *      describes the mathematical operation performed on the
         *      nodes described in pack.plist[index]..pack.plist[index+count] .
         *      For example, cAdd indicates that the constant values
         *      of the given parameters are to be added up.
         *      All the parameters must be of a type that
         *      evaluates into a constant value.
         */
        OpcodeType opcode : 8; // max bits required: 7 (note SpecialOpcode's limits)

        /* index and count have different meanings depending on the opcode.
         * See above for the possible interpretations. */
        unsigned count :  7; // max bits required: 7 (because of ImmedConstraint)
        unsigned index : 16; // max bits required: 9

        MatchResultType Match(
            FPoptimizer_CodeTree::CodeTree& tree,
            MatchedParams::CodeTreeMatch& match,
            unsigned long match_index) const;

        bool GetConst(
            const MatchedParams::CodeTreeMatch& match,
            double& result) const;

        void SynthesizeTree(
            FPoptimizer_CodeTree::CodeTree& tree,
            const MatchedParams& matcher,
            MatchedParams::CodeTreeMatch& match) const;
    } PACKED_GRAMMAR_ATTRIBUTE;

    /* A Function object describes
     * either a tree that must be matched,
     * or a tree that must be synthesized.
     *
     * - With Rule, it describes the tree that must be matched.
     * - With ParamSpec (when param.opcode=SubFunction),
     *                  it describes a subtree that must be matched,
     *                  or a subtree that must be generated, depending
     *                  on whether it is used in matching or synthesizing
     *                  context.
     */
    struct Function
    {
        /* The opcode that the tree must have. */
        OpcodeType opcode : 16;
        /* pack.mlist[index] describes the expected
         * parameters (leaves) of the tree. */
        unsigned   index  : 16;

        MatchResultType
            Match(FPoptimizer_CodeTree::CodeTree& tree,
                  MatchedParams::CodeTreeMatch& match,
                  unsigned long match_index) const;
    } PACKED_GRAMMAR_ATTRIBUTE;

    /* A rule describes a pattern for matching
     * and the method how to reconstruct the
     * matched node(s) in the tree.
     */
    struct Rule
    {
        /* For optimization: Number of minimum parameters (leaves)
         * that the source tree must contain for this rule to match.
         */
        unsigned  n_minimum_params : 8;

        /* If the rule matched, this field describes how to perform
         * the replacement.
         *   When type==ProduceNewTree,
         *       the source tree is replaced entirely with
         *       the new tree described at repl_index.
         *   When type==ReplaceParams,
         *       the matching leaves in the source tree
         *       are removed, and new leaves are constructed
         *       from the trees described at repl_index.
         *       Other leaves remain intact.
         */
        RuleType  type             : 8;

        /* The replacement tree is described in pack.mlist[repl_index]. */
        /* If type == ProduceNewTree, only the param index is used from matched_params */
        /* If type == ReplaceParams,  index and count are used */
        unsigned  repl_index       : 16;

        /* func describes the exact method how the tree should be matched.
         * If func does not match, this rule is not used; if it matches,
         * the rule is used and the tree is replaced according to the rules
         * indicated above.
         */
        Function  func;

        bool ApplyTo(FPoptimizer_CodeTree::CodeTree& tree) const;
    } PACKED_GRAMMAR_ATTRIBUTE;

    /* Grammar is a set of rules for tree substitutions. */
    struct Grammar
    {
        /* The rules of this grammar begin
         * at pack.rlist[index] and end
         * at pack.rlist[index+count].
         */
        unsigned index : 16;
        unsigned count : 16;

        /* ApplyTo tries to apply any/all rules within this
         * grammar to the given tree. Return value true indicates
         * that something was replaced, and that the grammar should
         * be applied again.
         * The recursion parameter is only used for diagnostics;
         * the default value false is appropriate.
         */
        bool ApplyTo(FPoptimizer_CodeTree::CodeTree& tree,
                     bool recursion=false) const;
    } PACKED_GRAMMAR_ATTRIBUTE;

    /* GrammarPack is the structure which contains all
     * the information regarding the optimization rules.
     */
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
