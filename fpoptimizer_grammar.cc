#include "fpconfig.hh"
#include "fparser.hh"
#include "fptypes.hh"

#include "fpoptimizer_grammar.hh"

using namespace FUNCTIONPARSERTYPES;

namespace FPoptimizer_Grammar
{
    ParamSpec::ParamSpec(FunctionType* f)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(cFCall), Func(f), Params(), Name()
          {
          }
    
    ParamSpec::ParamSpec(double d)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(cImmed), ConstantValue(d), Params(), Name() { }
    
    ParamSpec::ParamSpec(const std::string& n)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(cVar), Params(), Name(n) { }
    
    ParamSpec::ParamSpec(OpcodeType o, const std::vector<ParamSpec*>& p)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(o), Params(p), Name() { }
    
    ParamSpec::ParamSpec(unsigned i, double)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(cFetch), Index(i), Params(), Name() { }
    
    ParamSpec::ParamSpec(unsigned i, void*)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(cDup), Index(i), Params(), Name() { }


    void Grammar::Read(const GrammarPack& pack, size_t offs)
    {
        rules.clear();
        
        const Grammar_Const& g = pack.glist[offs];
        for(unsigned c=0; c<g.count; ++c)
            rules.push_back(Rule(pack, g.index + c));
    }
    
    Rule::Rule(const GrammarPack& pack, size_t offs)
        : Type(pack.rlist[offs].type),
          Input(pack, pack.rlist[offs].input_index),
          Replacement(pack, pack.rlist[offs].repl_index)
    {
    }
    
    FunctionType::FunctionType(const GrammarPack& pack, size_t offs)
        : Opcode(pack.flist[offs].opcode),
          Params(pack, pack.flist[offs].index)
    {
    }
    
    MatchedParams::MatchedParams(const GrammarPack& pack, size_t offs)
        : Type(pack.mlist[offs].type),
          Params()
    {
        const MatchedParams_Const& m = pack.mlist[offs];
        for(size_t a=0; a<m.count; ++a)
        {
            Params.push_back(new ParamSpec(pack, m.index + a));
        }
    }
    
    ParamSpec::ParamSpec(const GrammarPack& pack, size_t offs)
        : Negated(pack.plist[offs].negated),
          Transformation(pack.plist[offs].transformation),
          MinimumRepeat(pack.plist[offs].minrepeat),
          AnyRepetition(pack.plist[offs].anyrepeat),
          Opcode(pack.plist[offs].opcode),
          Params(), Name()
    {
        const ParamSpec_Const& pitem = pack.plist[offs];
        switch(Opcode)
        {
            case cImmed:
                ConstantValue = pack.clist[pitem.index];
                break;
            case cFetch:
                Index = pitem.index;
                break;
            case cVar:
                Name.assign(pack.nlist[pitem.index], pitem.count);
                break;
            case cDup:
                Index = pitem.index;
                break;
            case cFCall:
                Func = new FunctionType(pack, pitem.index);
                break;
            default:
            {
                for(size_t a=0; a<pitem.count; ++a)
                    Params.push_back(new ParamSpec(pack, pitem.index+a));
            }
        }
    }
}
