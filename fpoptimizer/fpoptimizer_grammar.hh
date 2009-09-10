#ifndef FPOPT_NAN_CONST

#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#define FPOPT_NAN_CONST (-1712345.25) /* Would use 0.0 / 0.0 here, but some compilers don't accept it. */

namespace FPoptimizer_CodeTree
{
    class CodeTree;
}

namespace FPoptimizer_Grammar
{
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

    /* The param_opcode field of the ParamSpec has the following
     * possible values (from enum SpecialOpcode):
     *   NumConstant:
     *      this describes a specific constant value (constvalue)
     *      that must be matched / synthesized.
     *   ImmedHolder:
     *      this describes any constant value
     *      that must be matched / synthesized.
     *      In matching, all ImmedHolders having the same ID (index)
     *      must evaluate to the same constant value.
     *      In synthesizing, the constant value matched by
     *      an ImmedHolder with this ID must be synthesized.
     *   NamedHolder:
     *      this describes any node
     *      that must be matched / synthesized.
     *      "index" is the ID of the NamedHolder:
     *      In matching, all NamedHolders having the same ID
     *      must match the identical node.
     *      In synthesizing, the node matched by
     *      a NamedHolder with this ID must be synthesized.
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
     *      The subtree is described in subfunc_opcode,param_begin..+param_count.
     *    GroupFunction:
     *      this describes a constant value
     *      that must be matched / synthesized.
     *      The subfunc_opcode field (from fparser's OPCODE enum)
     *      describes the mathematical operation performed on the
     *      nodes described in param_begin..+param_count.
     *      For example, cAdd indicates that the constant values
     *      of the given parameters are to be added up.
     *      All the parameters must be of a type that
     *      evaluates into a constant value.
     */
    enum SpecialOpcode
    {
        NumConstant,        // Holds a particular value (syntax-time constant)
        ImmedHolder,        // Holds a particular immed
        NamedHolder,        // Holds a particular named param (of any kind)
        SubFunction,        // Holds an opcode and the params
        RestHolder,         // Holds anything else
        GroupFunction       // For parse-time functions
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

#ifdef __GNUC__
# define PACKED_GRAMMAR_ATTRIBUTE __attribute__((packed))
#else
# define PACKED_GRAMMAR_ATTRIBUTE
#endif

    enum { PARAM_INDEX_BITS = 9 };

    /* A ParamSpec object describes
     * either a parameter (leaf, node) that must be matched,
     * or a parameter (leaf, node) that must be synthesized.
     */
    typedef std::pair<SpecialOpcode, const void*> ParamSpec;
    ParamSpec ParamSpec_Extract(unsigned paramlist, unsigned index);
    bool ParamSpec_Compare(const void* a, const void* b, SpecialOpcode type);

    struct ParamSpec_ImmedHolder
    {
        unsigned index       : 8; // holder ID
        unsigned constraints : 8; // constraints
    } PACKED_GRAMMAR_ATTRIBUTE;

    struct ParamSpec_NumConstant
    {
        double constvalue;        // the value
    } PACKED_GRAMMAR_ATTRIBUTE;

    struct ParamSpec_NamedHolder
    {
        unsigned index       : 8; // holder ID
        unsigned constraints : 8; // constraints
    } PACKED_GRAMMAR_ATTRIBUTE;

    struct ParamSpec_RestHolder
    {
        unsigned index       : 8; // holder ID
    } PACKED_GRAMMAR_ATTRIBUTE;

    struct ParamSpec_SubFunctionData
    {
        /* Expected parameters (leaves) of the tree: */
        unsigned param_count         : 2;
        unsigned param_list          : 30;

        /* The opcode that the tree must have when SubFunction */
        FUNCTIONPARSERTYPES::OPCODE subfunc_opcode : 6;

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
        ParamMatchingType match_type : 2; /* When SubFunction */
    } PACKED_GRAMMAR_ATTRIBUTE; // size: 2+30+6+2=40 bits=5 bytes

    struct ParamSpec_SubFunction
    {
        ParamSpec_SubFunctionData data;
        unsigned constraints : 8; // constraints
    } PACKED_GRAMMAR_ATTRIBUTE;

    struct ParamSpec_GroupFunction
    {
        unsigned param_count         : 2;
        unsigned param_list          : 30;
        /* The opcode that is used for calculating the GroupFunction */
        FUNCTIONPARSERTYPES::OPCODE subfunc_opcode : 8;
        unsigned constraints                       : 8; // constraints
    } PACKED_GRAMMAR_ATTRIBUTE;

    /* Theoretical minimal sizes in each param_opcode cases:
     * Assume param_opcode needs 3 bits.
     *    NumConstant:   3 + 64              (or 3+4 if just use index to clist[])
     *    ImmedHolder:   3 + 7 + 2           (7 for constraints, 2 for immed index)
     *    NamedHolder:   3 + 7 + 2           (same as above)
     *    RestHolder:    3 + 2               (2 for restholder index)
     *    SubFunction:   3 + 7 + 2 + 2 + 3*9 = 41
     *    GroupFunction: 3 +         2 + 3*9 = 32
     */

    /* A rule describes a pattern for matching
     * and the method how to reconstruct the
     * matched node(s) in the tree.
     */
    struct Rule
    {
        /* If the rule matched, this field describes how to perform
         * the replacement.
         *   When type==ProduceNewTree,
         *       the source tree is replaced entirely with
         *       the new tree described at repl_param_begin[0].
         *   When type==ReplaceParams,
         *       the matching leaves in the source tree are removed
         *       and new leaves are constructedfrom the trees
         *       described at repl_param_begin[0..repl_param_count].
         *       Other leaves remain intact.
         */
        RuleType  ruletype         : 3;

        /* The replacement parameters (if NewTree, begin[0] represents the new tree) */
        unsigned  repl_param_count : 2; /* Assumed to be 1 when type == ProduceNewTree */
        unsigned  repl_param_list  : 27;

        /* The function that we must match. Always a SubFunction. */
        ParamSpec_SubFunctionData match_tree;

        /* For optimization: Number of minimum parameters (leaves)
         * that the source tree must contain for this rule to match.
         */
        unsigned  n_minimum_params : 8;
    } PACKED_GRAMMAR_ATTRIBUTE; // size: 3+2+27 + 40 + 8 = 80 bits = 10 bytes

    /* Grammar is a set of rules for tree substitutions. */
    struct Grammar
    {
        /* The rules of this grammar */
        const Rule*    rule_begin;
        unsigned       rule_count;
    };

    bool ApplyGrammar(const Grammar& grammar,
                      FPoptimizer_CodeTree::CodeTree& tree);

    /* GrammarPack is the structure which contains all
     * the information regarding the optimization rules.
     */
    extern const struct GrammarPack
    {
        Grammar               glist[4];
    } pack;
}

#endif
