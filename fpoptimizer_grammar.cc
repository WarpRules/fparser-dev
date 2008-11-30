#include "fpoptimizer_grammar.hh"

namespace FPoptimizer_Grammar
{
    ParamSpec::ParamSpec(FunctionType* f)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(Function), Func(f), Params(), Name()
          {
          }
    
    ParamSpec::ParamSpec(double d)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(NumConstant), ConstantValue(d), Params(), Name() { }
    
    ParamSpec::ParamSpec(const std::string& n)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(NamedHolder), Params(), Name(n) { }
    
    ParamSpec::ParamSpec(OpcodeType o, const std::vector<ParamSpec*>& p)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(o), Params(p), Name() { }
    
    ParamSpec::ParamSpec(unsigned i, double)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(ImmedHolder), Index(i), Params(), Name() { }
    
    ParamSpec::ParamSpec(unsigned i, void*)
        : Negated(), Transformation(None),  MinimumRepeat(1), AnyRepetition(false),
          Opcode(RestHolder), Index(i), Params(), Name() { }

    bool ParamSpec::operator== (const ParamSpec& b) const
    {
        if(Negated != b.Negated) return false;
        if(Transformation != b.Transformation) return false;
        if(MinimumRepeat != b.MinimumRepeat) return false;
        if(AnyRepetition != b.AnyRepetition) return false;
        if(Opcode != b.Opcode) return false;
        switch(Opcode)
        {
            case NumConstant:
                return ConstantValue == b.ConstantValue;
            case ImmedHolder:
            case RestHolder:
                return Index == b.Index;
            case NamedHolder:
                return Name == b.Name;
            case Function:
                return *Func == *b.Func;
            default:
                if(Params.size() != b.Params.size()) return false;
                for(size_t a=0; a<Params.size(); ++a)
                    if(!(*Params[a] == *b.Params[a]))
                        return false;
                break;
        }
        return true;
    }

    bool ParamSpec::operator< (const ParamSpec& b) const
    {
        if(Negated != b.Negated) return Negated < b.Negated;
        if(Transformation != b.Transformation) return Transformation < b.Transformation;
        if(MinimumRepeat != b.MinimumRepeat) return MinimumRepeat < b.MinimumRepeat;
        if(AnyRepetition != b.AnyRepetition) return AnyRepetition < b.AnyRepetition;
        if(Opcode != b.Opcode) return Opcode < b.Opcode;
        switch(Opcode)
        {
            case NumConstant:
                return ConstantValue < b.ConstantValue;
            case ImmedHolder:
            case RestHolder:
                return Index < b.Index;
            case NamedHolder:
                return Name < b.Name;
            case Function:
                return *Func < *b.Func;
            default:
                if(Params.size() != b.Params.size()) return Params.size() > b.Params.size();
                for(size_t a=0; a<Params.size(); ++a)
                    if(!(*Params[a] == *b.Params[a]))
                        return *Params[a] < *b.Params[a];
                break;
        }
        return false;
    }

    bool MatchedParams::operator== (const MatchedParams& b) const
    {
        if(Type != b.Type) return false;
        if(Params.size() != b.Params.size()) return false;
        for(size_t a=0; a<Params.size(); ++a)
            if(!(*Params[a] == *b.Params[a]))
                return false;
        return true;
    }

    bool MatchedParams::operator< (const MatchedParams& b) const
    {
        if(Type !=  b.Type) return Type;
        if(Params.size() != b.Params.size()) return Params.size() > b.Params.size();
        for(size_t a=0; a < Params.size(); ++a)
            if(!(*Params[a] == *b.Params[a]))
                return *Params[a] < *b.Params[a];
        return false;
    }
    

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
          Params(), Name(),
          EvalValue(0)
    {
        const ParamSpec_Const& pitem = pack.plist[offs];
        switch(Opcode)
        {
            case NumConstant:
                ConstantValue = pack.clist[pitem.index];
                break;
            case ImmedHolder:
                Index = pitem.index;
                break;
            case NamedHolder:
                Name.assign(pack.nlist[pitem.index], pitem.count);
                break;
            case RestHolder:
                Index = pitem.index;
                break;
            case Function:
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
