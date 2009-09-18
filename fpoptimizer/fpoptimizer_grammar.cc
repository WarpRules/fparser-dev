#include "fparser.hh"
#include "fptypes.hh"
#include "fpoptimizer_grammar.hh"

using namespace FUNCTIONPARSERTYPES;

namespace FPoptimizer_Grammar
{
    bool ParamSpec_Compare(const void* aa, const void* bb, SpecialOpcode type)
    {
        switch(type)
        {
            case ImmedHolder:
            {
                ParamSpec_ImmedHolder& a = *(ParamSpec_ImmedHolder*) aa;
                ParamSpec_ImmedHolder& b = *(ParamSpec_ImmedHolder*) bb;
                return a.constraints == b.constraints
                    && a.index       == b.index;
            }
            case NumConstant:
            {
                ParamSpec_NumConstant& a = *(ParamSpec_NumConstant*) aa;
                ParamSpec_NumConstant& b = *(ParamSpec_NumConstant*) bb;
                return FloatEqual(a.constvalue, b.constvalue);
            }
            case NamedHolder:
            {
                ParamSpec_NamedHolder& a = *(ParamSpec_NamedHolder*) aa;
                ParamSpec_NamedHolder& b = *(ParamSpec_NamedHolder*) bb;
                return a.constraints == b.constraints
                    && a.index       == b.index;
            }
            case RestHolder:
            {
                ParamSpec_RestHolder& a = *(ParamSpec_RestHolder*) aa;
                ParamSpec_RestHolder& b = *(ParamSpec_RestHolder*) bb;
                return a.index       == b.index;
            }
            case SubFunction:
            {
                ParamSpec_SubFunction& a = *(ParamSpec_SubFunction*) aa;
                ParamSpec_SubFunction& b = *(ParamSpec_SubFunction*) bb;
                return a.constraints    == b.constraints
                    && a.data.subfunc_opcode == b.data.subfunc_opcode
                    && a.data.match_type     == b.data.match_type
                    && a.data.param_count    == b.data.param_count
                    && a.data.param_list     == b.data.param_list;
            }
            case GroupFunction:
            {
                ParamSpec_GroupFunction& a = *(ParamSpec_GroupFunction*) aa;
                ParamSpec_GroupFunction& b = *(ParamSpec_GroupFunction*) bb;
                return a.constraints    == b.constraints
                    && a.subfunc_opcode == b.subfunc_opcode
                    && a.param_count    == b.param_count
                    && a.param_list     == b.param_list;
            }
        }
        return true;
    }

    unsigned ParamSpec_GetDepCode(const ParamSpec& b)
    {
        switch(b.first)
        {
            case ImmedHolder:
            {
                const ParamSpec_ImmedHolder* s = (const ParamSpec_ImmedHolder*) b.second;
                return s->depcode;
            }
            case NamedHolder:
            {
                const ParamSpec_NamedHolder* s = (const ParamSpec_NamedHolder*) b.second;
                return s->depcode;
            }
            case SubFunction:
            {
                const ParamSpec_SubFunction* s = (const ParamSpec_SubFunction*) b.second;
                return s->depcode;
            }
            case GroupFunction:
            {
                const ParamSpec_GroupFunction* s = (const ParamSpec_GroupFunction*) &b;
                return s->depcode;
            }
            default: break;
        }
        return 0;
    }
}
