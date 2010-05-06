#include "fparser.hh"
#include "fptypes.hh"
#include "grammar.hh"
#include "optimize.hh"

#include "opcodename.hh"
using namespace FPoptimizer_Grammar;
using namespace FUNCTIONPARSERTYPES;

#include <cctype>

namespace FPoptimizer_Grammar
{
    template<typename Value_t>
    bool ParamSpec_Compare(const void* aa, const void* bb, SpecialOpcode type)
    {
        switch(type)
        {
            case ParamHolder:
            {
                ParamSpec_ParamHolder& a = *(ParamSpec_ParamHolder*) aa;
                ParamSpec_ParamHolder& b = *(ParamSpec_ParamHolder*) bb;
                return a.constraints == b.constraints
                    && a.index       == b.index
                    && a.depcode     == b.depcode;
            }
            case NumConstant:
            {
                ParamSpec_NumConstant<Value_t>& a = *(ParamSpec_NumConstant<Value_t>*) aa;
                ParamSpec_NumConstant<Value_t>& b = *(ParamSpec_NumConstant<Value_t>*) bb;
                return FloatEqual(a.constvalue, b.constvalue)
                    && a.modulo == b.modulo;
            }
            case SubFunction:
            {
                ParamSpec_SubFunction& a = *(ParamSpec_SubFunction*) aa;
                ParamSpec_SubFunction& b = *(ParamSpec_SubFunction*) bb;
                return a.constraints    == b.constraints
                    && a.data.subfunc_opcode   == b.data.subfunc_opcode
                    && a.data.match_type       == b.data.match_type
                    && a.data.param_count      == b.data.param_count
                    && a.data.param_list       == b.data.param_list
                    && a.data.restholder_index == b.data.restholder_index
                    && a.depcode               == b.depcode;
            }
        }
        return true;
    }

    unsigned ParamSpec_GetDepCode(const ParamSpec& b)
    {
        switch(b.first)
        {
            case ParamHolder:
            {
                const ParamSpec_ParamHolder* s = (const ParamSpec_ParamHolder*) b.second;
                return s->depcode;
            }
            case SubFunction:
            {
                const ParamSpec_SubFunction* s = (const ParamSpec_SubFunction*) b.second;
                return s->depcode;
            }
            default: break;
        }
        return 0;
    }

    template<typename Value_t>
    void DumpParam(const ParamSpec& parampair, std::ostream& o)
    {
        static const char ParamHolderNames[][2]  = {"%","&", "x","y","z","a","b","c"};

        //o << "/*p" << (&p-pack.plist) << "*/";
        unsigned constraints = 0;
        switch(parampair.first)
        {
            case NumConstant:
              { const ParamSpec_NumConstant<Value_t>& param = *(const ParamSpec_NumConstant<Value_t>*) parampair.second;
                o.precision(12);
                o << param.constvalue; break; }
            case ParamHolder:
              { const ParamSpec_ParamHolder& param = *(const ParamSpec_ParamHolder*) parampair.second;
                o << ParamHolderNames[param.index];
                constraints = param.constraints;
                break; }
            case SubFunction:
              { const ParamSpec_SubFunction& param = *(const ParamSpec_SubFunction*) parampair.second;
                constraints = param.constraints;
                if(param.data.match_type == GroupFunction)
                {
                    if(param.data.subfunc_opcode == cNeg)
                        { o << "-"; DumpParams<Value_t>(param.data.param_list, param.data.param_count, o); }
                    else if(param.data.subfunc_opcode == cInv)
                        { o << "/"; DumpParams<Value_t>(param.data.param_list, param.data.param_count, o); }
                    else
                    {
                        std::string opcode = FP_GetOpcodeName
                            ( (FUNCTIONPARSERTYPES::OPCODE) param.data.subfunc_opcode)
                            .substr(1);
                        for(size_t a=0; a<opcode.size(); ++a) opcode[a] = (char) std::toupper(opcode[a]);
                        o << opcode << "( ";
                        DumpParams<Value_t>(param.data.param_list, param.data.param_count, o);
                        o << " )";
                    }
                }
                else
                {
                    o << '(' << FP_GetOpcodeName
                        ( (FUNCTIONPARSERTYPES::OPCODE) param.data.subfunc_opcode)
                       << ' ';
                    if(param.data.match_type == PositionalParams) o << '[';
                    if(param.data.match_type == SelectedParams) o << '{';
                    DumpParams<Value_t>(param.data.param_list, param.data.param_count, o);
                    if(param.data.restholder_index != 0)
                        o << " <" << param.data.restholder_index << '>';
                    if(param.data.match_type == PositionalParams) o << "]";
                    if(param.data.match_type == SelectedParams) o << "}";
                    o << ')';
                }
                break; }
        }
        switch( ImmedConstraint_Value(constraints & ValueMask) )
        {
            case ValueMask: break;
            case Value_AnyNum: break;
            case Value_EvenInt:   o << "@E"; break;
            case Value_OddInt:    o << "@O"; break;
            case Value_IsInteger: o << "@I"; break;
            case Value_NonInteger:o << "@F"; break;
            case Value_Logical:   o << "@L"; break;
        }
        switch( ImmedConstraint_Sign(constraints & SignMask) )
        {
            case SignMask: break;
            case Sign_AnySign: break;
            case Sign_Positive:   o << "@P"; break;
            case Sign_Negative:   o << "@N"; break;
        }
        switch( ImmedConstraint_Oneness(constraints & OnenessMask) )
        {
            case OnenessMask: break;
            case Oneness_Any: break;
            case Oneness_One:     o << "@1"; break;
            case Oneness_NotOne:  o << "@M"; break;
        }
        switch( ImmedConstraint_Constness(constraints & ConstnessMask) )
        {
            //case ConstnessMask:
            case Constness_Const:
                if(parampair.first == ParamHolder)
                {
                    const ParamSpec_ParamHolder& param = *(const ParamSpec_ParamHolder*) parampair.second;
                    if(param.index < 2) break;
                }
                o << "@C";
                break;
            case Oneness_Any: break;
        }
    }

    template<typename Value_t>
    void DumpParams(unsigned paramlist, unsigned count, std::ostream& o)
    {
        for(unsigned a=0; a<count; ++a)
        {
            if(a > 0) o << ' ';
            const ParamSpec& param = ParamSpec_Extract<Value_t> (paramlist,a);
            DumpParam<Value_t> (param, o);
            unsigned depcode = ParamSpec_GetDepCode(param);
            if(depcode != 0)
                o << "@D" << depcode;
        }
    }
}

/* BEGIN_EXPLICIT_INSTANTATION */
namespace FPoptimizer_Grammar
{
    template void DumpParams<double>(unsigned, unsigned, std::ostream& );
    template bool ParamSpec_Compare<double>(const void*, const void*, SpecialOpcode);
#ifdef FP_SUPPORT_FLOAT_TYPE
    template void DumpParams<float>(unsigned, unsigned, std::ostream& );
    template bool ParamSpec_Compare<float>(const void*, const void*, SpecialOpcode);
#endif
#ifdef FP_SUPPORT_LONG_DOUBLE_TYPE
    template void DumpParams<long double>(unsigned, unsigned, std::ostream& );
    template bool ParamSpec_Compare<long double>(const void*, const void*, SpecialOpcode);
#endif
}
/* END_EXPLICIT_INSTANTATION */
