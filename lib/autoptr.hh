#ifndef FPOptimizerAutoPtrHH
#define FPOptimizerAutoPtrHH

#include <algorithm>

template<typename Ref>
class FPOPT_autoptr
{
public:
    FPOPT_autoptr()              : p(nullptr)      { }
    FPOPT_autoptr(Ref*        b) : p(b)            { Reserve(p); }
    FPOPT_autoptr(const FPOPT_autoptr& b) : p(b.p) { Reserve(p); }

    inline Ref& operator* () const { return *p; }
    inline Ref* operator->() const { return p; }
    bool isnull() const { return !p; }
    Ref* get() const { return p; }

    FPOPT_autoptr& operator= (Ref* b)
    {
        ChangeTo(b);
        return *this;
    }
    FPOPT_autoptr& operator= (const FPOPT_autoptr& b)
    {
        if(&b != this)
        {
            // If p == b.p,
            //              1. both of have a claim to p
            //              2. we add third claim (Reserve)
            //              3. we release the same (Release)
            //              4. nothing happens (p = p2). Both still have a claim to p, two in total.
            // If p != b.p,
            //              1. we have claim to p1, they have to p2
            //              2. we claim p2: p1 has now one claim, p2 has two
            //              3. we release p1: p1 is gone, p2 has two claims
            //              4. we refer to p2 (p2 has two claims, both of us)
            // In either case, this function operates properly
            ChangeTo(b.p);
        }
        return *this;
    }
    FPOPT_autoptr(FPOPT_autoptr&& b) : p(nullptr)
    {
        // Initially we have no claims, b has whatever
        // After this, we have what b had, and b has nothing
        swap(b);
    }
    FPOPT_autoptr& operator= (FPOPT_autoptr&& b)
    {
        if(this != &b)
        {
            // If p == b.p,
            //              1. both of have a claim to p
            //              2. we add third claim (Reserve)
            //              3. we release the same (Release)
            //              4. nothing happens (p = p2)
            //              5. b releases (p has one claim, ours only)
            // If p != b.p,
            //              1. we have claim to p1, they have to p2
            //              2. we claim p2: p1 has now one claim, p2 has two
            //              3. we release p1: p1 is gone, p2 has two claims
            //              4. we refer to p2
            //              5. b releases (p2 has one claim, ours only)
            // In either case, this function operates properly
            ChangeTo(b.p);
            Release(b.p);
        }
        return *this;
    }

    ~FPOPT_autoptr()
    {
        Release(p);
    }

    void swap(FPOPT_autoptr<Ref>& b) { std::swap(p, b.p); }

private:
    inline static void Reserve(Ref* p2);  // Does nothing if p2=null
    inline static void Release(Ref*& p2); // Note: p2 becomes nullptr
    inline void ChangeTo(Ref* p2);
private:
    Ref* p;
};

//
template<typename Ref>
inline void FPOPT_autoptr<Ref>::Release(Ref*& p2)
{
    if(p2)
    {
        --(p2->RefCount);
        if(!p2->RefCount)
        {
            delete p2;
        }
    }
    p2 = nullptr;
}
template<typename Ref>
inline void FPOPT_autoptr<Ref>::Reserve(Ref* p2)
{
    if(p2) ++(p2->RefCount);
}
template<typename Ref>
inline void FPOPT_autoptr<Ref>::ChangeTo(Ref* p2)
{
    Reserve(p2);
    Release(p);
    p = p2;
}

#endif
