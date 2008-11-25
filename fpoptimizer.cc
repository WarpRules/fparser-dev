//=============================================
// Function parser v3.0.3 optimizer by Bisqwit
//=============================================

/*
 NOTE!
 ----
 Everything that goes into the #ifndef FP_SUPPORT_OPTIMIZER part
 (ie. when FP_SUPPORT_OPTIMIZER is not defined) should be put in
 the end of fparser.cc file, not in this file.

 Everything in this file should be inside the #ifdef FP_SUPPORT_OPTIMIZER
 block (except the #include "fpconfig.hh" line).
*/


#include "fpconfig.hh"

#ifdef FP_SUPPORT_OPTIMIZER

#include "fparser.hh"
#include "fptypes.hh"
using namespace FUNCTIONPARSERTYPES;

#include <cmath>
#include <list>
#include <utility>
#include <cassert>
using namespace std;

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#define CONSTANT_E     2.71828182845904509080  // exp(1)
#define CONSTANT_PI    M_PI                    // atan2(0,-1)
#define CONSTANT_L10   2.30258509299404590109  // log(10)
#define CONSTANT_L10I  0.43429448190325176116  // 1/log(10)
#define CONSTANT_L10E  CONSTANT_L10I           // log10(e)
#define CONSTANT_L10EI CONSTANT_L10            // 1/log10(e)
#define CONSTANT_DR    (180.0 / M_PI)          // 180/pi
#define CONSTANT_RD    (M_PI / 180.0)          // pi/180

// Debugging in optimizer? Not recommended for production use.
//#define TREE_DEBUG
#ifdef TREE_DEBUG
// Debug verbosely?
#include <ostream>
#endif

#define POWI_TABLE_SIZE 256
#define POWI_WINDOW_SIZE 3
static const unsigned char powi_table[POWI_TABLE_SIZE] =
{
      0,   1,   1,   2,   2,   3,   3,   4,  /*   0 -   7 */
      4,   6,   5,   6,   6,  10,   7,   9,  /*   8 -  15 */
      8,  16,   9,  16,  10,  12,  11,  13,  /*  16 -  23 */
     12,  17,  13,  18,  14,  24,  15,  26,  /*  24 -  31 */  
     16,  17,  17,  19,  18,  33,  19,  26,  /*  32 -  39 */
     20,  25,  21,  40,  22,  27,  23,  44,  /*  40 -  47 */  
     24,  32,  25,  34,  26,  29,  27,  44,  /*  48 -  55 */
     28,  31,  29,  34,  30,  60,  31,  36,  /*  56 -  63 */   
     32,  64,  33,  34,  34,  46,  35,  37,  /*  64 -  71 */
     36,  65,  37,  50,  38,  48,  39,  69,  /*  72 -  79 */   
     40,  49,  41,  43,  42,  51,  43,  58,  /*  80 -  87 */
     44,  64,  45,  47,  46,  59,  47,  76,  /*  88 -  95 */   
     48,  65,  49,  66,  50,  67,  51,  66,  /*  96 - 103 */
     52,  70,  53,  74,  54, 104,  55,  74,  /* 104 - 111 */  
     56,  64,  57,  69,  58,  78,  59,  68,  /* 112 - 119 */
     60,  61,  61,  80,  62,  75,  63,  68,  /* 120 - 127 */  
     64,  65,  65, 128,  66, 129,  67,  90,  /* 128 - 135 */
     68,  73,  69, 131,  70,  94,  71,  88,  /* 136 - 143 */  
     72, 128,  73,  98,  74, 132,  75, 121,  /* 144 - 151 */
     76, 102,  77, 124,  78, 132,  79, 106,  /* 152 - 159 */
     80,  97,  81, 160,  82,  99,  83, 134,  /* 160 - 167 */ 
     84,  86,  85,  95,  86, 160,  87, 100,  /* 168 - 175 */
     88, 113,  89,  98,  90, 107,  91, 122,  /* 176 - 183 */ 
     92, 111,  93, 102,  94, 126,  95, 150,  /* 184 - 191 */
     96, 128,  97, 130,  98, 133,  99, 195,  /* 192 - 199 */ 
    100, 128, 101, 123, 102, 164, 103, 138,  /* 200 - 207 */
    104, 145, 105, 146, 106, 109, 107, 149,  /* 208 - 215 */
    108, 200, 109, 146, 110, 170, 111, 157,  /* 216 - 223 */
    112, 128, 113, 130, 114, 182, 115, 132,  /* 224 - 231 */
    116, 200, 117, 132, 118, 158, 119, 206,  /* 232 - 239 */ 
    120, 240, 121, 162, 122, 147, 123, 152,  /* 240 - 247 */
    124, 166, 125, 214, 126, 138, 127, 153   /* 248 - 255 */
}; /* copied from gcc */
static const int powi_cache_size = 256;

namespace {
inline double Min(double d1, double d2)
{
    return d1<d2 ? d1 : d2;
}
inline double Max(double d1, double d2)
{
    return d1>d2 ? d1 : d2;
}

class compres
{
    // states: 0=false, 1=true, 2=unknown
public:
    compres(bool b) : state(b) {}
    compres(char v) : state(v) {}
    // is it?
    operator bool() const { return state != 0; }
    // is it not?
    bool operator! () const { return state != 1; }
    bool operator==(bool b) const { return state != !b; }
    bool operator!=(bool b) const { return state != b; }
private:
    char state;
};

const compres maybe = (char)2;

struct CodeTree;

class SubTree
{
    CodeTree *tree;
    bool sign;  // Only possible when parent is cAdd or cMul

    inline void flipsign() { sign = !sign; }
public:
    SubTree();
    SubTree(double value);
    SubTree(const SubTree &b);
    SubTree(const CodeTree &b);

    ~SubTree();
    const SubTree &operator= (const SubTree &b);
    const SubTree &operator= (const CodeTree &b);

    bool getsign() const { return sign; }

    const CodeTree* operator-> () const { return tree; }
    const CodeTree& operator* () const { return *tree; }
    struct CodeTree* operator-> () { return tree; }
    struct CodeTree& operator* () { return *tree; }

    bool operator< (const SubTree& b) const;
    bool operator== (const SubTree& b) const;
    void Negate(); // Note: Parent must be cAdd
    void Invert(); // Note: Parent must be cMul

    void CheckConstNeg();
    void CheckConstInv();
};

bool IsNegate(const SubTree &p1, const SubTree &p2);
bool IsInverse(const SubTree &p1, const SubTree &p2);

typedef list<SubTree> paramlist;

#ifdef TREE_DEBUG
struct CodeTreeData;
std::ostream& operator << (std::ostream& str, const CodeTreeData& tree);
std::ostream& operator << (std::ostream& str, const CodeTree& tree);
#endif

struct CodeTreeData
{
    paramlist args;

private:
    unsigned op;       // Operation
    double value;      // In case of cImmed
    unsigned var;      // In case of cVar
    unsigned funcno;   // In case of cFCall, cPCall

public:
    CodeTreeData() : op(cAdd) {}
    ~CodeTreeData() {}

    void SetOp(unsigned newop)     { op=newop; }
    void SetFuncNo(unsigned newno) { funcno=newno; }

    inline unsigned GetOp() const { return op; }
    inline double GetImmed() const { return value; }
    inline unsigned GetVar() const { return var; }
    inline unsigned GetFuncNo() const { return funcno; }

    bool IsFunc() const  { return op == cFCall || op == cPCall; }
    bool IsImmed() const { return op == cImmed; }
    bool IsVar() const   { return op == cVar; }

    bool IsLongIntegerImmed() const
    {
        // Returns true if the immed can be converted losslessly into (long).
        if(!IsImmed()) return false;
        return GetImmed() == (double)GetLongIntegerImmed();
    }
    inline long GetLongIntegerImmed() const { return (long)GetImmed(); }

    void AddParam(const SubTree &p)
    {
        args.push_back(p);
    }
    void SetVar(unsigned v)
    {
        args.clear();
        op  = cVar;
        var = v;
    }
    void SetImmed(double v)
    {
        args.clear();
        op       = cImmed;
        value    = orig = v;
        inverted = negated = false;
    }
    void NegateImmed()
    {
        negated = !negated;
        UpdateValue();
    }
    void InvertImmed()
    {
        inverted = !inverted;
        UpdateValue();
    }

    bool IsOriginal() const { return !(IsInverted() || IsNegated()); }
    bool IsInverted() const { return inverted; }
    bool IsNegated() const { return negated; }
    bool IsInvertedOriginal() const { return IsInverted() && !IsNegated(); }
    bool IsNegatedOriginal() const { return !IsInverted() && IsNegated(); }

private:
    void UpdateValue()
    {
        value = orig;
        if(IsInverted()) { value = 1.0 / value;
                           // FIXME: potential divide by zero.
                         }
        if(IsNegated()) value = -value;
    }

    double orig;
    bool inverted;
    bool negated;
protected:
    // Ensure we don't accidentally copy this
    void operator=(const CodeTreeData &b);
};


class CodeTreeDataPtr
{
    typedef pair<CodeTreeData, unsigned> p_t;
    typedef p_t* pp;
    mutable pp p;

    void Alloc()   const { ++p->second; }
    void Dealloc() const { if(!--p->second) delete p; p = 0; }

    void PrepareForWrite()
    {
        // We're ready if we're the only owner.
        if(p->second == 1) return;

        // Then make a clone.
        p_t *newtree = new p_t(p->first, 1);
        // Forget the old
        Dealloc();
        // Keep the new
        p = newtree;
    }

public:
    CodeTreeDataPtr() : p(new p_t) { p->second = 1; }
    CodeTreeDataPtr(const CodeTreeDataPtr &b): p(b.p) { Alloc(); }
    ~CodeTreeDataPtr() { Dealloc(); }
    const CodeTreeDataPtr &operator= (const CodeTreeDataPtr &b)
    {
        b.Alloc();
        Dealloc();
        p = b.p;
        return *this;
    }
    const CodeTreeData *operator-> () const { return &p->first; }
    const CodeTreeData &operator*  () const { return p->first; }
    CodeTreeData *operator-> () { PrepareForWrite(); return &p->first; }
    CodeTreeData &operator*  () { PrepareForWrite(); return p->first; }

    void Shock();
};


#define CHECKCONSTNEG(item, op) \
    ((op)==cMul) \
       ? (item).CheckConstInv() \
       : (item).CheckConstNeg()

struct CodeTree
{
    CodeTreeDataPtr data;

private:
    typedef paramlist::iterator pit;
    typedef paramlist::const_iterator pcit;

    /*
    template<unsigned v> inline void chk() const
    {
    }
    */

public:
    const pcit GetBegin() const { return data->args.begin(); }
    const pcit GetEnd()   const { return data->args.end(); }
    const pit GetBegin() { return data->args.begin(); }
    const pit GetEnd()   { return data->args.end(); }
    const SubTree& getp0() const { /*chk<1>();*/pcit tmp=GetBegin();               return *tmp; }
    const SubTree& getp1() const { /*chk<2>();*/pcit tmp=GetBegin(); ++tmp;        return *tmp; }
    const SubTree& getp2() const { /*chk<3>();*/pcit tmp=GetBegin(); ++tmp; ++tmp; return *tmp; }
    unsigned GetArgCount() const { return data->args.size(); }
    void Erase(const pit p)      { data->args.erase(p); }

    SubTree& getp0() { /*chk<1>();*/pit tmp=GetBegin();               return *tmp; }
    SubTree& getp1() { /*chk<2>();*/pit tmp=GetBegin(); ++tmp;        return *tmp; }
    SubTree& getp2() { /*chk<3>();*/pit tmp=GetBegin(); ++tmp; ++tmp; return *tmp; }

    // set
    void SetImmed(double v) { data->SetImmed(v); }
    void SetOp(unsigned op) { data->SetOp(op); }
    void SetVar(unsigned v) { data->SetVar(v); }
    // get
    double GetImmed() const { return data->GetImmed(); }
    long GetLongIntegerImmed() const { return data->GetLongIntegerImmed(); }
    unsigned GetVar() const { return data->GetVar(); }
    unsigned GetOp() const  { return data->GetOp(); }
    // test
    bool IsImmed() const { return data->IsImmed(); }
    bool IsLongIntegerImmed() const { return data->IsLongIntegerImmed(); }
    bool IsVar()   const { return data->IsVar(); }
    // act
    void AddParam(const SubTree &p) { data->AddParam(p); }
    void NegateImmed() { data->NegateImmed(); } // don't use when op!=cImmed
    void InvertImmed() { data->InvertImmed(); } // don't use when op!=cImmed

    compres NonZero() const { if(!IsImmed()) return maybe;
                              return GetImmed() != 0.0; }
    compres IsPositive() const { if(!IsImmed()) return maybe;
                                 return GetImmed() > 0.0; }

private:
    struct ConstList
    {
        double voidvalue;
        list<pit> cp;
        double value;
        unsigned size() const { return cp.size(); }
    };
    struct ConstList BuildConstList(bool has_voidvalue = true);
    void KillConst(const ConstList &cl)
    {
        for(list<pit>::const_iterator i=cl.cp.begin(); i!=cl.cp.end(); ++i)
            Erase(*i);
    }
    void FinishConst(const ConstList &cl, bool has_voidvalue)
    {
        if(has_voidvalue)
        {
            if(cl.value != cl.voidvalue)
            {
                if(cl.size() == 1) return; // nothing to do
                AddParam(cl.value); // add the resulting constant
            }
        }
        else
        {
            if(cl.size() <= 0) return; // nothing to do
            AddParam(cl.value); // add the resulting constant
        }
        KillConst(cl); // remove all listed constants
    }

public:
    CodeTree() {}
    CodeTree(double v) { SetImmed(v); }

    CodeTree(unsigned op, const SubTree &p)
    {
        SetOp(op);
        AddParam(p);
    }
    CodeTree(unsigned op, const SubTree &p1, const SubTree &p2)
    {
        SetOp(op);
        AddParam(p1);
        AddParam(p2);
    }

    bool operator== (const CodeTree& b) const;
    bool operator< (const CodeTree& b) const;

private:
    bool IsSortable() const
    {
        switch(GetOp())
        {
            case cAdd:  case cMul:
            case cEqual:
            case cAnd: case cOr:
            case cMax: case cMin:
                return true;
            default:
                return false;
        }
    }
    void SortIfPossible()
    {
        if(IsSortable())
        {
            data->args.sort();
        }
    }

    void ReplaceWithConst(double value)
    {
        SetImmed(value);

        /* REMEMBER TO CALL CheckConstInv / CheckConstNeg
         * FOR PARENT SubTree, OR MAYHEM HAPPENS
         */
    }

    void ReplaceWith(const CodeTree &b)
    {
        // If b is child of *this, mayhem
        // happens. So we first make a clone
        // and then proceed with copy.
        CodeTreeDataPtr tmp = b.data;
        tmp.Shock();
        data = tmp;
    }

    void ReplaceWith(unsigned op, const SubTree &p)
    {
        ReplaceWith(CodeTree(op, p));
    }

    void ReplaceWith(unsigned op, const SubTree &p1, const SubTree &p2)
    {
        ReplaceWith(CodeTree(op, p1, p2));
    }

    void OptimizeConflict()
    {
        // This optimization does this: x-x = 0, x/x = 1, a+b-a = b.

        if(GetOp() == cAdd || GetOp() == cMul)
        {
        Redo:
            pit a, b;
            for(a=GetBegin(); a!=GetEnd(); ++a)
            {
                for(b=a; ++b != GetEnd(); )
                {
                    const SubTree &p1 = *a;
                    const SubTree &p2 = *b;

                    if(GetOp() == cMul ? IsInverse(p1,p2)
                                       : IsNegate(p1,p2))
                    {
                        // These parameters complement each others out
                        Erase(b);
                        Erase(a);
                        goto Redo;
                    }
                }
            }
        }
        OptimizeRedundant();
    }

    void OptimizeRedundant()
    {
        // This optimization does this: min()=0, max()=0, add()=0, mul()=1

        if(!GetArgCount())
        {
            if(GetOp() == cAdd || GetOp() == cMin || GetOp() == cMax)
                ReplaceWithConst(0);
            else if(GetOp() == cMul)
                ReplaceWithConst(1);
            return;
        }

        // And this: mul(x) = x, min(x) = x, max(x) = x, add(x) = x

        if(GetArgCount() == 1)
        {
            if(GetOp() == cMul || GetOp() == cAdd || GetOp() == cMin || GetOp() == cMax)
                if(!getp0().getsign())
                    ReplaceWith(*getp0());
        }

        OptimizeDoubleNegations();
    }

    void OptimizeDoubleNegations()
    {
        if(GetOp() == cAdd)
        {
            // Eschew double negations

            // If any of the elements is !cAdd of cMul
            // and has a numeric constant, negate
            // the constant and negate sign.

            for(pit a=GetBegin(); a!=GetEnd(); ++a)
            {
                SubTree &pa = *a;
                if(pa.getsign()
                && pa->GetOp() == cMul)
                {
                    // Before doing the check, ensure that
                    // we are not changing a x*1 into x*-1.
                    pa->OptimizeConstantMath1();

                    // OptimizeConstantMath1() may have changed
                    // the type of the expression (but not its sign),
                    // so verify that we're still talking about a cMul group.
                    if(pa->GetOp() == cMul)
                    {
                        CodeTree &p = *pa;
                        for(pit b=p.GetBegin();
                                b!=p.GetEnd(); ++b)
                        {
                            SubTree &pb = *b;
                            if(pb->IsImmed())
                            {
                                pb.Negate();
                                pa.Negate();
                                break;
                            }
                        }
                    }
                }
            }
        }

        if(GetOp() == cMul)
        {
            // If any of the elements is !cMul of cPow
            // and has a numeric exponent, negate
            // the exponent and negate sign.

            for(pit a=GetBegin(); a!=GetEnd(); ++a)
            {
                SubTree &pa = *a;
                if(pa.getsign() && pa->GetOp() == cPow)
                {
                    CodeTree &p = *pa;
                    if(p.getp1()->IsImmed())
                    {
                        // negate ok for pow when op=cImmed
                        p.getp1().Negate();
                        pa.Negate();
                    }
                }
            }
        }
    }

    void OptimizeConstantMath1()
    {
        // This optimization does three things:
        //      - For adding groups:
        //          Constants are added together.
        //      - For multiplying groups:
        //          Constants are multiplied together.
        //      - For function calls:
        //          If all parameters are constants,
        //          the call is replaced with constant value.

        // First, do this:
        OptimizeAddMulFlat();

#ifdef TREE_DEBUG
        cout << "BEFORE MATH1    :" << *this << endl;
#endif
        switch(GetOp())
        {
            case cAdd:
            {
                ConstList cl = BuildConstList();
                FinishConst(cl, true);
                break;
            }
            case cMul:
            {
                ConstList cl = BuildConstList();

                if(cl.value == 0.0) ReplaceWithConst(0.0);
                else FinishConst(cl, true);

                break;
            }
            case cMin:
            case cMax:
            {
                ConstList cl = BuildConstList(false);
                FinishConst(cl, false); // No "default" value
                break;
            }

            #define ConstantUnaryFun(token, fun) \
                case token: { const SubTree &p0 = getp0(); \
                    if(p0->IsImmed()) ReplaceWithConst(fun(p0->GetImmed())); \
                    break; }
            #define ConstantBinaryFun(token, fun) \
                case token: { const SubTree &p0 = getp0(); \
                              const SubTree &p1 = getp1(); \
                    if(p0->IsImmed() && \
                       p1->IsImmed()) ReplaceWithConst(fun(p0->GetImmed(), p1->GetImmed())); \
                    break; }

            // FIXME: potential invalid parameters for functions
            //        can cause exceptions here

            ConstantUnaryFun(cAbs,   fabs);
            ConstantUnaryFun(cAcos,  acos);
            ConstantUnaryFun(cAsin,  asin);
            ConstantUnaryFun(cAtan,  atan);
            ConstantUnaryFun(cCeil,  ceil);
            ConstantUnaryFun(cCos,   cos);
            ConstantUnaryFun(cCosh,  cosh);
            ConstantUnaryFun(cFloor, floor);
            ConstantUnaryFun(cLog,   log);
            ConstantUnaryFun(cSin,   sin);
            ConstantUnaryFun(cSinh,  sinh);
            ConstantUnaryFun(cTan,   tan);
            ConstantUnaryFun(cTanh,  tanh);
            ConstantBinaryFun(cAtan2, atan2);
            ConstantBinaryFun(cMod,   fmod); // not a func, but belongs here too
            ConstantBinaryFun(cPow,   pow);

            case cNeg:
            case cSub:
            case cDiv:
                /* Unreached (nonexistent operator)
                 * TODO: internal error here?
                 */
                break;

            case cCot:
            case cCsc:
            case cSec:
            case cDeg:
            case cRad:
            case cLog10:
            case cSqrt:
            case cExp:
                /* Unreached (nonexistent function)
                 * TODO: internal error here?
                 */
                 break;
        }
#ifdef TREE_DEBUG
        cout << "AFTER MATH1 A   :" << *this << endl;
#endif

        OptimizeConflict();

#ifdef TREE_DEBUG
        cout << "AFTER MATH1 B   :" << *this << endl;
#endif
    }

    void OptimizeAddMulFlat()
    {
        // This optimization flattens the topography of the tree.
        //   Examples:
        //       x + (y+z) = x+y+z
        //       x * (y/z) = x*y/z
        //       x / (y/z) = x/y*z
        //       min(x, min(y,z)) = min(x,y,z)
        //       max(x, max(y,z)) = max(x,y,z)

        if(GetOp() == cAdd
        || GetOp() == cMul
        || GetOp() == cMin
        || GetOp() == cMax)
        {
            // If children are same type as parent add them here
            for(pit b, a=GetBegin(); a!=GetEnd(); a=b)
            {
                const SubTree &pa = *a;  b=a; ++b;
                if(pa->GetOp() != GetOp()) continue;

                // Child is same type
                for(pcit c=pa->GetBegin();
                         c!=pa->GetEnd();
                         ++c)
                {
                    const SubTree &pb = *c;
                    if(pa.getsign())
                    {
                        // +a -(+b +c)
                        // means b and c will be negated

                        SubTree tmp = pb;
                        if(GetOp() == cMul)
                            tmp.Invert();
                        else
                            tmp.Negate();
                        AddParam(tmp);
                    }
                    else
                        AddParam(pb);
                }
                Erase(a);

                // Note: OptimizeConstantMath1() would be a good thing to call next.
            }
        }
    }

    void OptimizeLinearCombine()
    {
        // This optimization does the following:
        //
        //   x*x*x*x -> x^4
        //   x+x+x+x -> x*4
        //   x*x     -> x^2
        //   x/z/z   ->
        //
        //   min(x,x,y) -> min(x,y)
        //   max(x,x,y) -> max(x,y)
        //   max(x,x)   -> max(x)

        // Remove conflicts first, so we don't have to worry about signs.
        OptimizeConflict();

        bool didchanges = false;
        bool FactorIsImportant = true;
        unsigned ReplacementOpcode = GetOp();

        switch(GetOp())
        {
        case cAdd:
            FactorIsImportant = true;
            ReplacementOpcode = cMul;
            goto Redo;
        case cMul:
            FactorIsImportant = true;
            ReplacementOpcode = cPow;
            goto Redo;
        case cMin:
            FactorIsImportant = false;
            goto Redo;
        case cMax:
            FactorIsImportant = false;
            goto Redo;
        default: break;
        Redo:
            for(pit a=GetBegin(); a!=GetEnd(); ++a)
            {
                const SubTree &pa = *a;

                list<pit> poslist;

                for(pit b=a; ++b!=GetEnd(); )
                {
                    const SubTree &pb = *b;
                    if(*pa == *pb)
                        poslist.push_back(b);
                }

                unsigned min = 2;
                if(poslist.size() >= min)
                {
                    SubTree arvo = pa;
                    bool negate = arvo.getsign();

                    double factor = poslist.size() + 1;

                    if(negate)
                    {
                        arvo.Negate();
                        factor = -factor;
                    }

                    if(FactorIsImportant)
                    {
                        CodeTree tmp;
                        tmp.SetOp(ReplacementOpcode);
                        tmp.AddParam(arvo);
                        tmp.AddParam(factor);

                        list<pit>::const_iterator j;
                        for(j=poslist.begin(); j!=poslist.end(); ++j)
                            Erase(*j);
                        poslist.clear();
                        *a = tmp;
                    }
                    else
                    {
                        list<pit>::const_iterator j;
                        bool first=true;
                        for(j=poslist.begin(); j!=poslist.end(); ++j, first=false)
                            if(!first)
                                Erase(*j);
                    }
                    didchanges = true;
                    goto Redo;
                }
            }
        }

        if(didchanges)
        {
            // As a result, there might be need for this:
            OptimizeAddMulFlat();
            // And this:
            OptimizeRedundant();
        }
    }

    void OptimizeLogarithm()
    {
        /*
            This is basic logarithm math:
              pow(X,Y)/log(Y) = X
              log(X)/log(Y) = logY(X)
              log(X^Y)      = log(X)*Y
              log(X*Y)      = log(X)+log(Y)
              exp(log(X)*Y) = X^Y

            This function does these optimizations:
               pow(const_E, log(x))   = x
               pow(const_E, log(x)*y) = x^y
               pow(10,      log(x)*const_L10I*y) = x^y
               pow(z,       log(x)/log(z)*y)     = x^y

            And this:
               log(x^z)             = z * log(x)
            Which automatically causes these too:
               log(pow(const_E, x))         = x
               log(pow(y,       x))         = x * log(y)
               log(pow(pow(const_E, y), x)) = x*y

            And it does this too:
               log(x) + log(y) + log(z) = log(x * y * z)
               log(x * exp(y)) = log(x) + y

        */

        // Must be already in exponential form.

        // Optimize exponents before doing something.
        OptimizeExponents();

        if(GetOp() == cLog)
        {
            // We should have one parameter for log() function.
            // If we don't, we're screwed.

            const SubTree &p = getp0();

            if(p->GetOp() == cPow)
            {
                // Found log(x^y)
                SubTree p0 = p->getp0(); // x
                SubTree p1 = p->getp1(); // y

                // Build the new logarithm.
                CodeTree tmp(GetOp(), p0);  // log(x)

                // Become log(x) * y
                ReplaceWith(cMul, tmp, p1);
            }
            else if(p->GetOp() == cMul)
            {
                // Redefine &p nonconst
                SubTree &p = getp0();

                p->OptimizeAddMulFlat();
                p->OptimizeExponents();

                p.CheckConstInv();

                list<SubTree> adds;

                for(pit b, a = p->GetBegin();
                           a != p->GetEnd(); a=b)
                {
                    SubTree &pa = *a;  b=a; ++b;
                    if(pa->GetOp() == cPow
                    && pa->getp0()->IsImmed()
                    && pa->getp0()->GetImmed() == CONSTANT_E)
                    {
                        adds.push_back(pa->getp1());
                        p->Erase(a);
                        continue;
                    }
                }
                if(adds.size())
                {
                    CodeTree tmp(cAdd, *this);

                    list<SubTree>::const_iterator i;
                    for(i=adds.begin(); i!=adds.end(); ++i)
                        tmp.AddParam(*i);

                    ReplaceWith(tmp);
                }
            }
        }
        if(GetOp() == cAdd)
        {
            // Check which ones are logs.
            list<pit> poslist;

            for(pit a=GetBegin(); a!=GetEnd(); ++a)
            {
                const SubTree &pa = *a;
                if(pa->GetOp() == cLog)
                    poslist.push_back(a);
            }

            if(poslist.size() >= 2)
            {
                CodeTree tmp(cMul, 1.0); // eek

                list<pit>::const_iterator j;
                for(j=poslist.begin(); j!=poslist.end(); ++j)
                {
                    const SubTree &pb = **j;
                    // Take all of its children
                    for(pcit b=pb->GetBegin();
                             b!=pb->GetEnd();
                             ++b)
                    {
                        SubTree tmp2 = *b;
                        if(pb.getsign()) tmp2.Negate();
                        tmp.AddParam(tmp2);
                    }
                    Erase(*j);
                }
                poslist.clear();

                AddParam(CodeTree(cLog, tmp));
            }
            // Done, hopefully
        }
        if(GetOp() == cPow)
        {
            const SubTree &p0 = getp0();
            SubTree &p1 = getp1();

            if(p0->IsImmed() && p0->GetImmed() == CONSTANT_E
            && p1->GetOp() == cLog)
            {
                // pow(const_E, log(x)) = x
                ReplaceWith(*(p1->getp0()));
            }
            else if(p1->GetOp() == cMul)
            {
                //bool didsomething = true;

                pit poslogpos; bool foundposlog = false;
                pit neglogpos; bool foundneglog = false;

                ConstList cl = p1->BuildConstList();

                for(pit a=p1->GetBegin(); a!=p1->GetEnd(); ++a)
                {
                    const SubTree &pa = *a;
                    if(pa->GetOp() == cLog)
                    {
                        if(!pa.getsign())
                        {
                            foundposlog = true;
                            poslogpos   = a;
                        }
                        else if(*p0 == *(pa->getp0()))
                        {
                            foundneglog = true;
                            neglogpos   = a;
                        }
                    }
                }

                if(p0->IsImmed()
                && p0->GetImmed() == 10.0
                && cl.value == CONSTANT_L10I
                && foundposlog)
                {
                    SubTree base = (*poslogpos)->getp0();
                    p1->KillConst(cl);
                    p1->Erase(poslogpos);
                    p1->OptimizeRedundant();
                    SubTree mul = p1;

                    ReplaceWith(cPow, base, mul);

                    // FIXME: what optimizations should be done now?
                    return;
                }

                // Put back the constant
                FinishConst(cl, true);

                if(p0->IsImmed()
                && p0->GetImmed() == CONSTANT_E
                && foundposlog)
                {
                    SubTree base = (*poslogpos)->getp0();
                    p1->Erase(poslogpos);

                    p1->OptimizeRedundant();
                    SubTree mul = p1;

                    ReplaceWith(cPow, base, mul);

                    // FIXME: what optimizations should be done now?
                    return;
                }

                if(foundposlog
                && foundneglog
                && *((*neglogpos)->getp0()) == *p0)
                {
                    SubTree base = (*poslogpos)->getp0();
                    p1->Erase(poslogpos);
                    p1->Erase(neglogpos);

                    p1->OptimizeRedundant();
                    SubTree mul = p1;

                    ReplaceWith(cPow, base, mul);

                    // FIXME: what optimizations should be done now?
                    return;
                }
            }
        }
    }

    void OptimizeFunctionCalls()
    {
        /* Goals: sin(asin(x)) = x
         *        cos(acos(x)) = x
         *        tan(atan(x)) = x
         * NOTE:
         *   Do NOT do these:
         *     asin(sin(x))
         *     acos(cos(x))
         *     atan(tan(x))
         *   Because someone might want to wrap the angle.
         */
        // FIXME: TODO
    }

    void OptimizePowMulAdd()
    {
        // x^3 * x -> x^4
        // x*3 + x -> x*4
        // FIXME: Do those

        // x^1 -> x
        if(GetOp() == cPow)
        {
            const SubTree &base     = getp0();
            const SubTree &exponent = getp1();

            if(exponent->IsImmed())
            {
                if(exponent->GetImmed() == 1.0)
                    ReplaceWith(*base);
                else if(exponent->GetImmed() == 0.0
                     && base->NonZero())
                    ReplaceWithConst(1.0);
            }
        }
    }

    void OptimizeExponents()
    {
        /* Goals:
         *     (x^y)^z   -> x^(y*z)
         *     x^y * x^z -> x^(y+z)
         */
        // First move to exponential form.
        OptimizeLinearCombine();

        bool didchanges = false;

    Redo:
        if(GetOp() == cPow)
        {
            // (x^y)^z   -> x^(y*z)

            const SubTree &p0 = getp0();
            const SubTree &p1 = getp1();
            if(p0->GetOp() == cPow)
            {
                CodeTree tmp(cMul, p0->getp1(), p1);
                tmp.Optimize();

                ReplaceWith(cPow, p0->getp0(), tmp);

                didchanges = true;
                goto Redo;
            }
        }
        if(GetOp() == cMul)
        {
            // x^y * x^z -> x^(y+z)

            for(pit a=GetBegin(); a!=GetEnd(); ++a)
            {
                const SubTree &pa = *a;

                if(pa->GetOp() != cPow) continue;

                list<pit> poslist;

                for(pit b=a; ++b != GetEnd(); )
                {
                    const SubTree &pb = *b;
                    if(pb->GetOp() == cPow
                    && *(pa->getp0())
                    == *(pb->getp0()))
                    {
                        poslist.push_back(b);
                    }
                }

                if(poslist.size() >= 1)
                {
                    poslist.push_back(a);

                    CodeTree base = *(pa->getp0());

                    CodeTree exponent(cAdd, 0.0); //eek

                    // Collect all exponents to cAdd
                    list<pit>::const_iterator i;
                    for(i=poslist.begin(); i!=poslist.end(); ++i)
                    {
                        const SubTree &pb = **i;

                        SubTree tmp2 = pb->getp1();
                        if(pb.getsign()) tmp2.Invert();

                        exponent.AddParam(tmp2);
                    }

                    exponent.Optimize();

                    CodeTree result(cPow, base, exponent);

                    for(i=poslist.begin(); i!=poslist.end(); ++i)
                        Erase(*i);
                    poslist.clear();

                    AddParam(result); // We're cMul, remember

                    didchanges = true;
                    goto Redo;
                }
            }
        }

        OptimizePowMulAdd();

        if(didchanges)
        {
            // As a result, there might be need for this:
            OptimizeConflict();
        }
    }

    void OptimizeLinearExplode()
    {
        // x^2 -> x*x
        // But only if x is just a simple thing

        // Won't work on anything else.
        if(GetOp() != cPow) return;

        // TODO TODO TODO
    }

    void OptimizePascal()
    {
#if 0    // Too big, too specific, etc

        // Won't work on anything else.
        if(GetOp() != cAdd) return;

        // Must be done after OptimizeLinearCombine();

        // Don't need pascal triangle
        // Coefficient for x^a * y^b * z^c = 3! / (a! * b! * c!)

        // We are greedy and want other than just binomials
        // FIXME

        // note: partial ones are also nice
        //     x*x + x*y + y*y
        //   = (x+y)^2 - x*y
        //
        //     x x * x y * + y y * +
        // ->  x y + dup * x y * -
#endif
    }

public:

    void Optimize();

    void Assemble(vector<unsigned> &byteCode,
                  vector<double>   &immed,
                  size_t& stacktop_cur,
                  size_t& stacktop_max) const;
    void AssembleSequence(
                  const SubTree& tree, long count,
                  double if_zero_constant,
                  unsigned if_negative_opcode,
                  unsigned cumulation_opcode,
                  vector<unsigned> &byteCode,
                  vector<double>   &immed,
                  size_t& stacktop_cur,
                  size_t& stacktop_max) const;

    typedef std::pair<size_t, // Stack offset where it is found
                      int     // How many times will it still be needed?
                     > Subdivide_result;

    Subdivide_result AssembleSequence_Subdivide(
                  long count,
                  int cache[powi_cache_size], int cache_needed[powi_cache_size],
                  unsigned cumulation_opcode,
                  vector<unsigned> &byteCode,
                  vector<double>   &immed,
                  size_t& stacktop_cur,
                  size_t& stacktop_max) const;

    Subdivide_result Subdivide_MakeResult(
                  const Subdivide_result& a,
                  const Subdivide_result& b,

                  unsigned cumulation_opcode,
                  vector<unsigned> &byteCode,
                  vector<double>   &immed,
                  size_t& stacktop_cur,
                  size_t& stacktop_max) const;

    void FinalOptimize()
    {
        // First optimize each parameter.
        for(pit a=GetBegin(); a!=GetEnd(); ++a)
            (*a)->FinalOptimize();

        /* These things are to be done:
         *
         * x * CONSTANT_DR        -> cDeg(x)
         * x * CONSTANT_RD        -> cRad(x)
         * pow(x, 0.5)            -> sqrt(x)
         * log(x) * CONSTANT_L10I -> log10(x)
         * pow(CONSTANT_E, x)     -> exp(x)
         * inv(sin(x))            -> csc(x)
         * inv(cos(x))            -> sec(x)
         * inv(tan(x))            -> cot(x)
         */


        if(GetOp() == cPow)
        {
            const SubTree &p0 = getp0();
            const SubTree &p1 = getp1();
            if(p0->GetOp()    == cImmed
            && p0->GetImmed() == CONSTANT_E)
            {
                ReplaceWith(cExp, p1);
            }
            else if(p1->GetOp()    == cImmed
                 && p1->GetImmed() == 0.5)
            {
                ReplaceWith(cSqrt, p0);
            }
        }
        if(GetOp() == cMul)
        {
            if(GetArgCount() == 1 && getp0().getsign())
            {
                /***/if(getp0()->GetOp() == cSin)ReplaceWith(cCsc, getp0()->getp0());
                else if(getp0()->GetOp() == cCos)ReplaceWith(cSec, getp0()->getp0());
                else if(getp0()->GetOp() == cTan)ReplaceWith(cCot, getp0()->getp0());
            }
        }
        // Separate "if", because op may have just changed
        if(GetOp() == cMul)
        {
            /* NOTE: Not gcov'd yet */

            CodeTree *found_log = 0;

            ConstList cl = BuildConstList();

            for(pit a=GetBegin(); a!=GetEnd(); ++a)
            {
                SubTree &pa = *a;
                if(pa->GetOp() == cLog && !pa.getsign())
                    found_log = &*pa;
            }
            if(cl.value == CONSTANT_L10I && found_log)
            {
                // Change the log() to log10()
                found_log->SetOp(cLog10);
                // And forget the constant
                KillConst(cl);
            }
            else if(cl.value == CONSTANT_DR)
            {
                KillConst(cl);
                OptimizeRedundant();
#ifdef TREE_DEBUG
    cout << "PRE_REP         :" << (*this) << endl;
#endif
                ReplaceWith(cDeg, *this);
#ifdef TREE_DEBUG
    cout << "POST_REP        :" << (*this) << endl;
#endif
            }
            else if(cl.value == CONSTANT_RD)
            {
                KillConst(cl);
                OptimizeRedundant();
                ReplaceWith(cRad, *this);
            }
            else FinishConst(cl, true);
        }

        SortIfPossible();
    }
};

#ifdef TREE_DEBUG
std::ostream& operator << (std::ostream& str, const CodeTree& tree)
{
    const CodeTreeData& data = *tree.data;
    switch( (FUNCTIONPARSERTYPES::OPCODE) data.GetOp())
    {
        case cImmed: str << data.GetImmed(); return str;
        case cVar:   str << "Var" << data.GetVar(); return str;
        case cFCall: str << "FCall(Func" << data.GetFuncNo() << ")"; break;
        case cPCall: str << "PCall(Func" << data.GetFuncNo() << ")"; break;

        case cAbs: str << "cAbs"; break;
        case cAcos: str << "cAcos"; break;
#ifndef FP_NO_ASINH
        case cAcosh: str << "cAcosh"; break;
#endif
        case cAsin: str << "cAsin"; break;
#ifndef FP_NO_ASINH
        case cAsinh: str << "cAsinh"; break;
#endif
        case cAtan: str << "cAtan"; break;
        case cAtan2: str << "cAtan2"; break;
#ifndef FP_NO_ASINH
        case cAtanh: str << "cAtanh"; break;
#endif
        case cCeil: str << "cCeil"; break;
        case cCos: str << "cCos"; break;
        case cCosh: str << "cCosh"; break;
        case cCot: str << "cCot"; break;
        case cCsc: str << "cCsc"; break;
#ifndef FP_DISABLE_EVAL
        case cEval: str << "cEval"; break;
#endif
        case cExp: str << "cExp"; break;
        case cFloor: str << "cFloor"; break;
        case cIf: str << "cIf"; break;
        case cInt: str << "cInt"; break;
        case cLog: str << "cLog"; break;
        case cLog10: str << "cLog10"; break;
        case cMax: str << "cMax"; break;
        case cMin: str << "cMin"; break;
        case cSec: str << "cSec"; break;
        case cSin: str << "cSin"; break;
        case cSinh: str << "cSinh"; break;
        case cSqrt: str << "cSqrt"; break;
        case cTan: str << "cTan"; break;
        case cTanh: str << "cTanh"; break;

        // These do not need any ordering:
        case cJump: str << "cJump"; break;
        case cNeg: str << "cNeg"; break;
        case cAdd: str << "cAdd"; break;
        case cSub: str << "cSub"; break;
        case cMul: str << "cMul"; break;
        case cDiv: str << "cDiv"; break;
        case cMod: str << "cMod"; break;
        case cPow: str << "cPow"; break;
        case cEqual: str << "cEqual"; break;
        case cNEqual: str << "cNEqual"; break;
        case cLess: str << "cLess"; break;
        case cLessOrEq: str << "cLessOrEq"; break;
        case cGreater: str << "cGreater"; break;
        case cGreaterOrEq: str << "cGreaterOrEq"; break;
        case cNot: str << "cNot"; break;
        case cAnd: str << "cAnd"; break;
        case cOr: str << "cOr"; break;
        case cDeg: str << "cDeg"; break;
        case cRad: str << "cRad"; break;
        case cDup: str << "cDup"; break;
        case cInv: str << "cInv"; break;
        case cPop: str << "cPop"; break;
        case cFetch: str << "cFetch"; break;
        case VarBegin: str << "VarBegin"; break;
    }
    str << '(';

    bool first = true;
    for(paramlist::const_iterator
        i = data.args.begin(); i != data.args.end(); ++i)
    {
        if(first) first=false; else str << ", ";
        const SubTree& sub = *i;
        if(sub.getsign()) str << '!';
        str << *sub;
    }
    str << ')';

    return str;
}
#endif

void CodeTreeDataPtr::Shock()
{
 /*
    PrepareForWrite();
    paramlist &p2 = (*this)->args;
    for(paramlist::iterator i=p2.begin(); i!=p2.end(); ++i)
    {
        (*i)->data.Shock();
    }
 */
}

CodeTree::ConstList CodeTree::BuildConstList(bool has_voidvalue)
{
    ConstList result;
    result.value     =
    result.voidvalue = GetOp()==cMul ? 1.0 : 0.0;

    bool first = true;

    list<pit> &cp = result.cp;
    for(pit b, a=GetBegin(); a!=GetEnd(); a=b)
    {
        SubTree &pa = *a;  b=a; ++b;
        if(!pa->IsImmed()) continue;

        double thisvalue = pa->GetImmed();
        if(has_voidvalue && thisvalue == result.voidvalue)
        {
            // This value is no good, forget it
            Erase(a);
            continue;
        }
        switch(GetOp())
        {
            case cMul:
                result.value *= thisvalue;
                break;
            case cAdd:
                result.value += thisvalue;
                break;
            case cMin:
                if(first || thisvalue < result.value) result.value = thisvalue;
                break;
            case cMax:
                if(first || thisvalue > result.value) result.value = thisvalue;
                break;
            default: break; /* Unreached */
        }
        cp.push_back(a);
        first = false;
    }
    if(GetOp() == cMul)
    {
        /*
          If one of the values is -1 and it's not the only value,
          then some other value will be negated.
        */
        for(bool done=false; cp.size() > 1 && !done; )
        {
            done = true;
            for(list<pit>::iterator b,a=cp.begin(); a!=cp.end(); a=b)
            {
                b=a; ++b;
                if((**a)->GetImmed() == -1.0)
                {
                    Erase(*a);
                    cp.erase(a);

                    // take randomly something
                    (**cp.begin())->data->NegateImmed();
                    if(cp.size() < 2)break;
                    done = false;
                }
            }
        }
    }
    return result;
}

void CodeTree::Assemble
   (vector<unsigned> &byteCode,
    vector<double>   &immed,
    size_t& stacktop_cur,
    size_t& stacktop_max) const
{
    #define AddCmd(op) byteCode.push_back((op))
    #define AddConst(v) do { \
        byteCode.push_back(cImmed); \
        immed.push_back((v)); \
    } while(0)
    #define SimuPush(n) stacktop_cur += (n)
    #define SimuPop(n) do { \
        if(stacktop_cur > stacktop_max) stacktop_max = stacktop_cur; \
        stacktop_cur -= (n); \
    } while(0)
    #define SimuDupPushFrom(n) do { \
        if((n) == stacktop_cur-1) AddCmd(cDup); \
        else { AddCmd(cFetch); AddCmd((n)); } \
        SimuPush(1); \
    } while(0)

    if(IsVar())
    {
        SimuPush(1);
        AddCmd(GetVar());
        return;
    }
    if(IsImmed())
    {
        SimuPush(1);
        AddConst(GetImmed());
        return;
    }

    switch(GetOp())
    {
        case cAdd:
        case cMul:
        case cMin:
        case cMax:
        {
            unsigned opcount = 0;

            /* TODO: If the cMul contains an immed,
                     refrain from processing the immed,
                     and use AssembleSequence() to
                     generate a sequence of cDup & cFetch & cAdd
             */

            for(pcit a=GetBegin(); a!=GetEnd(); ++a)
            {
                const SubTree &pa = *a;

                if(opcount < 2) ++opcount;

                bool pnega = pa.getsign();

                bool done = false;
                if(pa->IsImmed())
                {
                    if(GetOp() == cMul
                    && pa->data->IsInverted()
                    && (pnega || opcount==2)
                      )
                    {
                        CodeTree tmp = *pa;
                        tmp.data->InvertImmed();
                        tmp.Assemble(byteCode, immed,
                                     stacktop_cur, stacktop_max);
                        pnega = !pnega;
                        done = true;
                    }
                    else if(GetOp() == cAdd
                    && (pa->data->IsNegatedOriginal()
                //     || pa->GetImmed() < 0
                       )
                    && (pnega || opcount==2)
                           )
                    {
                        CodeTree tmp = *pa;
                        tmp.data->NegateImmed();
                        tmp.Assemble(byteCode, immed,
                                     stacktop_cur, stacktop_max);
                        pnega = !pnega;
                        done = true;
                    }
                }
                if(!done)
                    pa->Assemble(byteCode, immed, stacktop_cur, stacktop_max);

                if(opcount == 2)
                {
                    unsigned tmpop = GetOp();
                    if(pnega) // negation in non-first operand
                    {
                        tmpop = (tmpop == cMul) ? cDiv : cSub;
                    }
                    SimuPop(1);
                    AddCmd(tmpop);
                }
                else if(pnega) // negation in the first operand
                {
                    if(GetOp() == cMul) AddCmd(cInv);
                    else AddCmd(cNeg);
                }
            }
            break;
        }
        case cPow:
        {
            const SubTree& p0 = getp0();
            const SubTree& p1 = getp1();

            if(p1->IsLongIntegerImmed())
            {
                /* Optimize integer exponents */
                AssembleSequence(
                    p0, p1->GetLongIntegerImmed(),
                    1.0,   /* in case the exponent is 0 */
                    cInv,  /* in case the exponent is negative */
                    cMul,  /* cumulation operand */
                    byteCode,immed,stacktop_cur,stacktop_max
                );
            }
            else
            {
                p0->Assemble(byteCode, immed, stacktop_cur, stacktop_max);
                p1->Assemble(byteCode, immed, stacktop_cur, stacktop_max);
                AddCmd(GetOp());
                SimuPop(1);
            }
            break;
        }
        case cIf:
        {
            // If the parameter amount is != 3, we're screwed.
            getp0()->Assemble(byteCode, immed, stacktop_cur, stacktop_max); // expression
            SimuPop(1);

            unsigned ofs = byteCode.size();
            AddCmd(cIf);
            AddCmd(0); // code index
            AddCmd(0); // immed index

            getp1()->Assemble(byteCode, immed, stacktop_cur, stacktop_max); // true branch
            SimuPop(1);

            byteCode[ofs+1] = byteCode.size()+2;
            byteCode[ofs+2] = immed.size();

            ofs = byteCode.size();
            AddCmd(cJump);
            AddCmd(0); // code index
            AddCmd(0); // immed index

            getp2()->Assemble(byteCode, immed, stacktop_cur, stacktop_max); // false branch
            SimuPop(1);

            byteCode[ofs+1] = byteCode.size()-1;
            byteCode[ofs+2] = immed.size();

            SimuPush(1);

            break;
        }
        case cFCall:
        {
            // If the parameter count is invalid, we're screwed.
            size_t was_stacktop = stacktop_cur;
            for(pcit a=GetBegin(); a!=GetEnd(); ++a)
            {
                const SubTree &pa = *a;
                pa->Assemble(byteCode, immed, stacktop_cur, stacktop_max);
            }
            AddCmd(GetOp());
            AddCmd(data->GetFuncNo());
            SimuPop(stacktop_cur - was_stacktop - 1);
            break;
        }
        case cPCall:
        {
            // If the parameter count is invalid, we're screwed.
            size_t was_stacktop = stacktop_cur;
            for(pcit a=GetBegin(); a!=GetEnd(); ++a)
            {
                const SubTree &pa = *a;
                pa->Assemble(byteCode, immed, stacktop_cur, stacktop_max);
            }
            AddCmd(GetOp());
            AddCmd(data->GetFuncNo());
            SimuPop(stacktop_cur - was_stacktop - 1);
            break;
        }
        default:
        {
            // If the parameter count is invalid, we're screwed.
            size_t was_stacktop = stacktop_cur;
            for(pcit a=GetBegin(); a!=GetEnd(); ++a)
            {
                const SubTree &pa = *a;
                pa->Assemble(byteCode, immed, stacktop_cur, stacktop_max);
            }
            AddCmd(GetOp());
            SimuPop(stacktop_cur - was_stacktop - 1);
            break;
        }
    }
}

static void PlanNtimesCache
    (long count,
     int cache[powi_cache_size],
     int cache_needed[powi_cache_size],
     int need_count = 1)
{
    if(count < 1) return;

    if(count < powi_cache_size)
    {
        /*fprintf(stderr, "%ld will be needed %d times more\n", count, need_count);*/
        cache_needed[count] += need_count;
        if(cache[count]) return;
    }

    if(count < POWI_TABLE_SIZE)
    {
        long half = powi_table[count];
        bool by_itself = half*2 == count;

        PlanNtimesCache(half,       cache, cache_needed, by_itself ? 2 : 1);
        if(!by_itself)
            PlanNtimesCache(count-half, cache, cache_needed);
    }
    else if(count & 1)
    {
        int digit = count & ((1 << POWI_WINDOW_SIZE) - 1);
        bool by_itself = digit*2 == count;

        PlanNtimesCache(digit, cache, cache_needed, by_itself ? 2 : 1);

        if(!by_itself) PlanNtimesCache(count - digit, cache, cache_needed);
    }
    else
        PlanNtimesCache(count / 2, cache, cache_needed, 2);

    if(count < powi_cache_size)
        cache[count] = 1; // This value has been generated
}

void CodeTree::AssembleSequence(
    const SubTree& tree, long count,
    double if_zero_constant,
    unsigned if_negative_opcode,
    unsigned cumulation_opcode,
    vector<unsigned> &byteCode,
    vector<double>   &immed,
    size_t& stacktop_cur,
    size_t& stacktop_max) const
{
    if(count == 0)
    {
        SimuPush(1);
        AddConst(if_zero_constant);
    }
    else
    {
        tree->Assemble(byteCode, immed, stacktop_cur, stacktop_max);
        if(count < 0)
        {
            AddCmd(if_negative_opcode);
            count = -count;
        }

        /* To prevent calculating the same factors over and over again,
         * we use a cache. */
        int cache[powi_cache_size], cache_needed[powi_cache_size];

        /* Assume we have no factors in the cache */
        for(int n=0; n<powi_cache_size; ++n) { cache[n] = 0; cache_needed[n] = 0; }


        /* Decide which factors we would need multiple times.
         * Output:
         *   cache[]        = these factors were generated
         *   cache_needed[] = number of times these factors were desired
         */
        cache[1] = 1; // We have this value already.
        PlanNtimesCache(count, cache, cache_needed);

        cache[1] = stacktop_cur-1;
        for(int n=2; n<powi_cache_size; ++n)
            cache[n] = -1; /* Stack location for each component */

        size_t stacktop_desired = stacktop_cur;

        // Cache all the required components
        for(int n=2; n<powi_cache_size; ++n)
            if(cache_needed[n] > 0)
            {
                /*fprintf(stderr, "Will need %d, %d times, caching...\n", n, cache_needed[n]);*/
                Subdivide_result res = AssembleSequence_Subdivide(
                    n, cache, cache_needed, cumulation_opcode,
                    byteCode, immed, stacktop_cur, stacktop_max);
                /*fprintf(stderr, "Cache[%d] = %u,%d\n",
                    n, (unsigned)res.first, res.second);*/
                cache[n] = res.first;
            }

        /*fprintf(stderr, "Calculating result for %ld...\n", count);*/
        Subdivide_result res = AssembleSequence_Subdivide(
            count, cache, cache_needed, cumulation_opcode,
            byteCode,immed, stacktop_cur, stacktop_max);

        size_t n_excess = stacktop_cur - stacktop_desired;
        if(n_excess > 0 || res.first != stacktop_desired-1)
        {
            // Remove the cache values
            AddCmd(cPop);
            AddCmd(stacktop_desired-1);
            AddCmd(res.first);
            SimuPop(n_excess);
        }
    }
}

CodeTree::Subdivide_result CodeTree::AssembleSequence_Subdivide(
    long count,
    int cache[powi_cache_size], int cache_needed[powi_cache_size],
    unsigned cumulation_opcode,
    vector<unsigned> &byteCode,
    vector<double>   &immed,
    size_t& stacktop_cur,
    size_t& stacktop_max) const
{
    if(count < powi_cache_size)
    {
        if(cache[count] >= 0)
        {
            // found from the cache
            /*fprintf(stderr, "* I found %ld from cache (%u,%d)\n",
                count, (unsigned)cache[count], cache_needed[count]);*/
            return Subdivide_result(cache[count], --cache_needed[count]);
        }
    }
    
    if(count < POWI_TABLE_SIZE)
    {
        long half = powi_table[count];

        /*fprintf(stderr, "* I want %ld, my plan is %ld + %ld\n", count, half, count-half);*/

        Subdivide_result half_res = AssembleSequence_Subdivide(
            half, cache, cache_needed, cumulation_opcode,
            byteCode,immed, stacktop_cur,stacktop_max);

        if(half*2 == count)
        {
            // self-cumulate the subdivide result
            return Subdivide_MakeResult(
                half_res,
                half_res,
                cumulation_opcode,
                byteCode,immed, stacktop_cur,stacktop_max);
        }
        else
        {
            Subdivide_result otherhalf_res = AssembleSequence_Subdivide(
                count-half, cache, cache_needed, cumulation_opcode,
                byteCode,immed, stacktop_cur,stacktop_max);

            return Subdivide_MakeResult(
                half_res,
                otherhalf_res,
                cumulation_opcode,
                byteCode,immed, stacktop_cur,stacktop_max);
        }
    }
    else if(count & 1)
    {
        long digit = count & ((1 << POWI_WINDOW_SIZE) - 1);

        /*fprintf(stderr, "* I want %ld, my plan is %ld + %ld\n", count, digit, count-digit);*/

        Subdivide_result digit_res = AssembleSequence_Subdivide(
            digit, cache, cache_needed, cumulation_opcode,
            byteCode,immed, stacktop_cur,stacktop_max);

        if(digit*2 == count)
        {
            return Subdivide_MakeResult(
                digit_res,
                digit_res,
                cumulation_opcode,
                byteCode,immed, stacktop_cur,stacktop_max);
        }
        else
        {
            Subdivide_result other_res = AssembleSequence_Subdivide(
                count-digit, cache, cache_needed, cumulation_opcode,
                byteCode,immed, stacktop_cur,stacktop_max);

            return Subdivide_MakeResult(
                digit_res,
                other_res,
                cumulation_opcode,
                byteCode,immed, stacktop_cur,stacktop_max);
        }
    }
    else
    {
        /*fprintf(stderr, "* I want %ld, my plan is %ld + %ld\n", count, count/2, count/2);*/

        Subdivide_result half_res = AssembleSequence_Subdivide(
            count/2, cache, cache_needed, cumulation_opcode,
            byteCode,immed, stacktop_cur,stacktop_max);

        // self-cumulate the subdivide result
        return Subdivide_MakeResult(
            half_res,
            half_res,
            cumulation_opcode,
            byteCode,immed, stacktop_cur,stacktop_max);
    }
}

CodeTree::Subdivide_result CodeTree::Subdivide_MakeResult(
    const Subdivide_result& a,
    const Subdivide_result& b,
    unsigned cumulation_opcode,
    vector<unsigned> &byteCode,
    vector<double>   &immed,
    size_t& stacktop_cur,
    size_t& stacktop_max) const
{
    /*fprintf(stderr, "== making result for %u:%d and %u:%d, stacktop=%u\n",
        (unsigned)a.first, a.second,
        (unsigned)b.first, b.second,
        (unsigned)stacktop_cur);*/

    // Figure out whether we can trample a and b
    int a_needed = a.second;
    int b_needed = b.second;
    // If they're the same slot, tax them twice.
    if(a.first == b.first) { a_needed -= 1; b_needed -= 1; }

    size_t apos = a.first, bpos = b.first;

    #define DUP_BOTH() do { \
        if(apos < bpos) { size_t tmp=apos; apos=bpos; bpos=tmp; } \
        /*fprintf(stderr, "-> dup(%u) dup(%u) op\n", (unsigned)apos, (unsigned)bpos);*/ \
        SimuDupPushFrom(apos); \
        SimuDupPushFrom(apos==bpos ? stacktop_cur-1 : bpos); } while(0)
    #define DUP_ONE(p) do { \
        /*fprintf(stderr, "-> dup(%u) op\n", (unsigned)p);*/ \
        SimuDupPushFrom(p); \
    } while(0)

    if(a_needed > 0 && b_needed > 0)
    {
        // If they must both be preserved, make duplicates
        // First push the one that is at the larger stack
        // address. This increases the odds of possibly using cDup.
        DUP_BOTH();

        //SCENARIO 1:
        // Input:  x B A x x
        // Temp:   x B A x x A B
        // Output: x B A x x R
        //SCENARIO 2:
        // Input:  x A B x x
        // Temp:   x A B x x B A
        // Output: x A B x x R

        // Add them together.
        AddCmd(cumulation_opcode);
        SimuPop(1);
        // The return value will not need to be preserved.
        return Subdivide_result(stacktop_cur-1, 0);
    }
    // So, either one could be trampled over

    if(a_needed > 0)
    {
        // A must be preserved, but B can be trampled over

        // SCENARIO 1:
        //  Input:  x B x x A
        //   Temp:  x B x x A A B   (dup both, later first)
        //  Output: x B x x A R
        // SCENARIO 2:
        //  Input:  x A x x B
        //   Temp:  x A x x B A
        //  Output: x A x x R
        // SCENARIO 3:
        //  Input:  x x x B A
        //   Temp:  x x x B A A B   (dup both, later first)
        //  Output: x x x B A R
        // SCENARIO 4:
        //  Input:  x x x A B
        //   Temp:  x x x A B A
        //  Output: x x x A R
        // SCENARIO 5:
        //  Input:  x A B x x
        //   Temp:  x A B x x A B   (dup both, later first)
        //  Output: x A B x x R

        // if B is not at the top, dup both.
        if(bpos != stacktop_cur-1)
            DUP_BOTH();    // dup both
        else
            DUP_ONE(apos); // just dup A

        AddCmd(cumulation_opcode);
        SimuPop(1);
        return Subdivide_result(stacktop_cur-1, 0);
    }
    else if(b_needed > 0)
    {
        // B must be preserved, but A can be trampled over
        // This is a mirror image of the a_needed>0 case, so I'll cut the chase
        if(apos != stacktop_cur-1)
            DUP_BOTH();
        else
            DUP_ONE(bpos);

        AddCmd(cumulation_opcode);
        SimuPop(1);
        return Subdivide_result(stacktop_cur-1, 0);
    }
    else
    {
        // Both can be trampled over.
        // SCENARIO 1:
        //  Input:  x B x x A
        //   Temp:  x B x x A B
        //  Output: x B x x R
        // SCENARIO 2:
        //  Input:  x A x x B
        //   Temp:  x A x x B A
        //  Output: x A x x R
        // SCENARIO 3:
        //  Input:  x x x B A
        //  Output: x x x R
        // SCENARIO 4:
        //  Input:  x x x A B
        //  Output: x x x R
        // SCENARIO 5:
        //  Input:  x A B x x
        //   Temp:  x A B x x A B   (dup both, later first)
        //  Output: x A B x x R
        // SCENARIO 6:
        //  Input:  x x x C
        //   Temp:  x x x C C   (c is both A and B)
        //  Output: x x x R

        if(apos == bpos && apos == stacktop_cur-1)
            DUP_ONE(apos); // scenario 6
        else if(apos == stacktop_cur-1 && bpos == stacktop_cur-2)
            /*fprintf(stderr, "-> op\n")*/; // scenario 3
        else if(apos == stacktop_cur-2 && bpos == stacktop_cur-1)
            /*fprintf(stderr, "-> op\n")*/; // scenario 4
        else if(apos == stacktop_cur-1)
            DUP_ONE(bpos); // scenario 1
        else if(bpos == stacktop_cur-1)
            DUP_ONE(apos); // scenario 2
        else
            DUP_BOTH(); // scenario 5

        AddCmd(cumulation_opcode);
        SimuPop(1);
        return Subdivide_result(stacktop_cur-1, 0);
    }
}

void CodeTree::Optimize()
{
    // Phase:
    //   Phase 0: Do local optimizations.
    //   Phase 1: Optimize each.
    //   Phase 2: Do local optimizations again.

    for(unsigned phase=0; phase<=2; ++phase)
    {
        if(phase == 1)
        {
            // Optimize each parameter.
            for(pit a=GetBegin(); a!=GetEnd(); ++a)
            {
                (*a)->Optimize();
                CHECKCONSTNEG(*a, GetOp());
            }
            continue;
        }
        if(phase == 0 || phase == 2)
        {
            // Do local optimizations.

            OptimizeConstantMath1();
            OptimizeLogarithm();
            OptimizeFunctionCalls();
            OptimizeExponents();
            OptimizeLinearExplode();
            OptimizePascal();

            /* Optimization paths:

               doublenegations=
               redundant= * doublenegations
               conflict= * redundant
               addmulflat=
               constantmath1= addmulflat * conflict
               linearcombine= conflict * addmulflat¹ redundant¹
               powmuladd=
               exponents= linearcombine * powmuladd conflict¹
               logarithm= exponents *
               functioncalls= IDLE
               linearexplode= IDLE
               pascal= IDLE

               * = actions here
               ¹ = only if made changes
            */
        }
    }
}


bool CodeTree::operator== (const CodeTree& b) const
{
    if(GetOp() != b.GetOp()) return false;
    if(IsImmed()) if(GetImmed()  != b.GetImmed())  return false;
    if(IsVar())   if(GetVar()    != b.GetVar())    return false;
    if(data->IsFunc())
        if(data->GetFuncNo() != b.data->GetFuncNo()) return false;
    return data->args == b.data->args;
}

bool CodeTree::operator< (const CodeTree& b) const
{
    if(GetArgCount() != b.GetArgCount())
        return GetArgCount() > b.GetArgCount();

    if(GetOp() != b.GetOp())
    {
        // sort immeds last
        if(IsImmed() != b.IsImmed()) return IsImmed() < b.IsImmed();

        return GetOp() < b.GetOp();
    }

    if(IsImmed())
    {
        if(GetImmed() != b.GetImmed()) return GetImmed() < b.GetImmed();
    }
    if(IsVar() && GetVar() != b.GetVar())
    {
        return GetVar() < b.GetVar();
    }
    if(data->IsFunc() && data->GetFuncNo() != b.data->GetFuncNo())
    {
        return data->GetFuncNo() < b.data->GetFuncNo();
    }

    pcit i = GetBegin(), j = b.GetBegin();
    for(; i != GetEnd(); ++i, ++j)
    {
        const SubTree &pa = *i, &pb = *j;

        if(!(pa == pb))
            return pa < pb;
    }
    return false;
}


bool IsNegate(const SubTree &p1, const SubTree &p2) /*const */
{
    if(p1->IsImmed() && p2->IsImmed())
    {
        return p1->GetImmed() == -p2->GetImmed();
    }
    if(p1.getsign() == p2.getsign()) return false;
    return *p1 == *p2;
}
bool IsInverse(const SubTree &p1, const SubTree &p2) /*const*/
{
    if(p1->IsImmed() && p2->IsImmed())
    {
        // FIXME: potential divide by zero.
        return p1->GetImmed() == 1.0 / p2->GetImmed();
    }
    if(p1.getsign() == p2.getsign()) return false;
    return *p1 == *p2;
}

SubTree::SubTree() : tree(new CodeTree), sign(false)
{
}

SubTree::SubTree(const SubTree &b) : tree(new CodeTree(*b.tree)), sign(b.sign)
{
}

#define SubTreeDecl(p1, p2) \
    SubTree::SubTree p1 : tree(new CodeTree p2), sign(false) { }

SubTreeDecl( (const CodeTree &b), (b) )
SubTreeDecl( (double value),             (value) )

#undef SubTreeDecl

SubTree::~SubTree()
{
    delete tree; tree=0;
}

const SubTree &SubTree::operator= (const SubTree &b)
{
    sign = b.sign;
    CodeTree *oldtree = tree;
    tree = new CodeTree(*b.tree);
    delete oldtree;
    return *this;
}
const SubTree &SubTree::operator= (const CodeTree &b)
{
    sign = false;
    CodeTree *oldtree = tree;
    tree = new CodeTree(b);
    delete oldtree;
    return *this;
}

bool SubTree::operator< (const SubTree& b) const
{
    if(getsign() != b.getsign()) return getsign() < b.getsign();
    return *tree < *b.tree;
}
bool SubTree::operator== (const SubTree& b) const
{
    return sign == b.sign && *tree == *b.tree;
}
void SubTree::Negate() // Note: Parent must be cAdd
{
    flipsign();
    CheckConstNeg();
}
void SubTree::CheckConstNeg()
{
    if(tree->IsImmed() && getsign())
    {
        tree->NegateImmed();
        sign = false;
    }
}
void SubTree::Invert() // Note: Parent must be cMul
{
    flipsign();
    CheckConstInv();
}
void SubTree::CheckConstInv()
{
    if(tree->IsImmed() && getsign())
    {
        tree->InvertImmed();
        sign = false;
    }
}

}//namespace

void FunctionParser::MakeTree(void *r) const
{
    // Dirty hack. Should be fixed.
    CodeTree* result = static_cast<CodeTree*>(r);

    vector<CodeTree> stack(1);

    #define GROW(n) do { \
        stacktop += n; \
        if(stack.size() <= stacktop) stack.resize(stacktop+1); \
    } while(0)

    #define EAT(n, opcode) do { \
        unsigned newstacktop = stacktop-(n); \
        if((n) == 0) GROW(1); \
        stack[stacktop].SetOp((opcode)); \
        for(unsigned a=0, b=(n); a<b; ++a) \
            stack[stacktop].AddParam(stack[newstacktop+a]); \
        stack.erase(stack.begin() + newstacktop, \
                    stack.begin() + stacktop); \
        stacktop = newstacktop; GROW(1); \
    } while(0)

    #define ADDCONST(n) do { \
        stack[stacktop].SetImmed((n)); \
        GROW(1); \
    } while(0)

    unsigned stacktop=0;

    list<unsigned> labels;

    const std::vector<unsigned>& ByteCode = data->ByteCode;
    const unsigned ByteCodeSize = ByteCode.size();
    const std::vector<double>& Immed = data->Immed;

    for(unsigned IP=0, DP=0; ; ++IP)
    {
        while(labels.size() > 0
        && *labels.begin() == IP)
        {
            // The "else" of an "if" ends here
            EAT(3, cIf);
            labels.erase(labels.begin());
        }

        if(IP >= ByteCodeSize)
        {
            break;
        }

        unsigned opcode = ByteCode[IP];

        if(opcode == cIf)
        {
            IP += 2;
        }
        else if(opcode == cJump)
        {
            labels.push_front(ByteCode[IP+1]+1);
            IP += 2;
        }
        else if(opcode == cImmed)
        {
            ADDCONST(Immed[DP++]);
        }
        else if(OPCODE(opcode) < VarBegin)
        {
            switch(opcode)
            {
                // Unary operators
                case cNeg:
                {
                    EAT(1, cAdd); // Unary minus is negative adding.
                    stack[stacktop-1].getp0().Negate();
                    break;
                }
                // Binary operators
                case cSub:
                {
                    EAT(2, cAdd); // Minus is negative adding
                    stack[stacktop-1].getp1().Negate();
                    break;
                }
                case cDiv:
                {
                    EAT(2, cMul); // Divide is inverse multiply
                    stack[stacktop-1].getp1().Invert();
                    break;
                }

                // ADD ALL TWO PARAMETER NON-FUNCTIONS HERE
                case cAdd: case cMul:
                case cMod: case cPow:
                case cEqual: case cLess: case cGreater:
                case cNEqual: case cLessOrEq: case cGreaterOrEq:
                case cAnd: case cOr:
                    EAT(2, opcode);
                    break;

                // ADD ALL UNARY NON-FUNCTIONS HERE
                case cNot:
                    EAT(1, opcode);
                    break;

                case cFCall:
                {
                    unsigned index = ByteCode[++IP];
                    unsigned params = data->FuncPtrs[index].params;
                    EAT(params, opcode);
                    stack[stacktop-1].data->SetFuncNo(index);
                    break;
                }
                case cPCall:
                {
                    unsigned index = ByteCode[++IP];
                    unsigned params =
                        //data->FuncParsers[index]->data->varAmount;
                        data->FuncParsers[index].params;
                    EAT(params, opcode);
                    stack[stacktop-1].data->SetFuncNo(index);
                    break;
                }

                // Converted to cMul on fly
                case cDeg:
                    ADDCONST(CONSTANT_DR);
                    EAT(2, cMul);
                    break;

                // Converted to cMul on fly
                case cRad:
                    ADDCONST(CONSTANT_RD);
                    EAT(2, cMul);
                    break;

              case cNop: break;

                // Functions
                default:
                {
                    //assert(opcode >= cAbs);
                    unsigned funcno = opcode-cAbs;
                    assert(funcno < sizeof(Functions)/sizeof(Functions[0]));
                    const FuncDefinition& func = Functions[funcno];

                    //const FuncDefinition& func = Functions[opcode-cAbs];

                    unsigned paramcount = func.params;
#ifndef FP_DISABLE_EVAL
                    if(opcode == cEval)
                        paramcount = unsigned(data->variableRefs.size());
#endif
                    if(opcode == cSqrt)
                    {
                        // Converted on fly: sqrt(x) = x^0.5
                        opcode = cPow;
                        paramcount = 2;
                        ADDCONST(0.5);
                    }
                    if(opcode == cExp)
                    {
                        // Converted on fly: exp(x) = CONSTANT_E^x

                        opcode = cPow;
                        paramcount = 2;
                        // reverse the parameters... kludgey
                        stack[stacktop] = stack[stacktop-1];
                        stack[stacktop-1].SetImmed(CONSTANT_E);
                        GROW(1);
                    }
                    bool do_inv = false;
                    if(opcode == cCot) { do_inv = true; opcode = cTan; }
                    if(opcode == cCsc) { do_inv = true; opcode = cSin; }
                    if(opcode == cSec) { do_inv = true; opcode = cCos; }

                    bool do_log10 = false;
                    if(opcode == cLog10)
                    {
                        // Converted on fly: log10(x) = log(x) * CONSTANT_L10I
                        opcode = cLog;
                        do_log10 = true;
                    }
                    EAT(paramcount, opcode);
                    if(do_log10)
                    {
                        ADDCONST(CONSTANT_L10I);
                        EAT(2, cMul);
                    }
                    if(do_inv)
                    {
                        // Unary cMul, inverted. No need for "1.0"
                        EAT(1, cMul);
                        stack[stacktop-1].getp0().Invert();
                    }
                    break;
                }
            }
        }
        else
        {
            stack[stacktop].SetVar(opcode);
            GROW(1);
        }
    }

    if(!stacktop)
    {
        // ERROR: Stack does not have any values!
        return;
    }

    --stacktop; // Ignore the last element, it is always nop (cAdd).

    if(stacktop > 0)
    {
        // ERROR: Stack has too many values!
        return;
    }

    // Okay, the tree is now stack[0]
    *result = stack[0];
}

void FunctionParser::Optimize()
{
    if(isOptimized) return;
    CopyOnWrite();

    CodeTree tree;
    MakeTree(&tree);

#ifdef TREE_DEBUG
    cout << "BEFORE OPT      :" << tree << endl;
#endif
    // Do all sorts of optimizations
    tree.Optimize();

#ifdef TREE_DEBUG
    cout << "BEFORE FINAL    :" << tree << endl;
#endif

    // Last changes before assembly
    tree.FinalOptimize();

#ifdef TREE_DEBUG
    cout << "AFTER           :" << tree << endl;
#endif
    // Now rebuild from the tree.

    vector<unsigned> byteCode;
    vector<double> immed;

#if 0
    byteCode.resize(Comp.ByteCodeSize);
    for(unsigned a=0; a<Comp.ByteCodeSize; ++a)byteCode[a] = Comp.ByteCode[a];

    immed.resize(Comp.ImmedSize);
    for(unsigned a=0; a<Comp.ImmedSize; ++a)immed[a] = Comp.Immed[a];
#else
    byteCode.clear(); immed.clear();
    size_t stacktop_cur = 0;
    size_t stacktop_max = 0;
    tree.Assemble(byteCode, immed, stacktop_cur, stacktop_max);

    if(data->StackSize < stacktop_max)
    {
        data->StackSize = stacktop_max;
        data->Stack.resize(stacktop_max);
    }
#endif

    data->ByteCode.swap(byteCode);
    data->Immed.swap(immed);

    isOptimized = true;
}


#endif // #ifdef FP_SUPPORT_OPTIMIZER
