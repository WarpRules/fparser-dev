#if defined(FP_USE_THREAD_SAFE_EVAL) || defined(FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA)
# include <atomic>
# include <mutex>
# define THREAD_SAFETY
#endif

#include "MpfrFloat.hh"
#include <stdio.h>
#include <mpfr.h>
#ifdef THREAD_SAFETY
# include <list>
#else
# include <deque>
#endif
#include <vector>
#include <cstring>
#include <cassert>


//===========================================================================
// Auxiliary structs
//===========================================================================
struct MpfrFloat::MpfrFloatData
{
#ifdef THREAD_SAFETY
    std::atomic<unsigned> mRefCount{1};
    std::atomic<MpfrFloatData*> nextFreeNode{nullptr};
#else
    unsigned mRefCount{1};
    MpfrFloatData* nextFreeNode{nullptr};
#endif

    mpfr_t mFloat;

    MpfrFloatData() {}
};

class MpfrFloat::MpfrFloatDataContainer
{
#ifdef THREAD_SAFETY
    std::atomic<unsigned long> mDefaultPrecision{256ul};
#else
    unsigned long mDefaultPrecision{256ul};
#endif

#ifdef THREAD_SAFETY
    std::list<MpfrFloatData> mData;
    std::atomic<MpfrFloatData*> mFirstFreeNode{nullptr};
#else
    std::deque<MpfrFloatData> mData;
    MpfrFloatData* mFirstFreeNode{nullptr};
#endif

    MpfrFloatData *mConst_0 = nullptr;
    MpfrFloatData *mConst_pi = nullptr;
    MpfrFloatData *mConst_e = nullptr;
    MpfrFloatData *mConst_log2 = nullptr;
    MpfrFloatData *mConst_epsilon = nullptr;

#ifdef THREAD_SAFETY
    bool mpfr_is_thread_safe;
    std::mutex lock;
#endif

 public:
    MpfrFloatDataContainer()
#ifdef THREAD_SAFETY
        : mpfr_is_thread_safe( mpfr_buildopt_tls_p() )
#endif
    {
    }

    ~MpfrFloatDataContainer()
    {
        for(auto& data: mData)
            mpfr_clear(data.mFloat);
    }

    MpfrFloatData* allocateMpfrFloatData(bool initToZero)
    {
        if(mFirstFreeNode)
        {
#ifndef THREAD_SAFETY
            MpfrFloatData* firstFree = mFirstFreeNode;
#else
            MpfrFloatData* firstFree = mFirstFreeNode.exchange(nullptr);
            if(firstFree)
#endif
            {
                MpfrFloatData* node = firstFree;

                // TODO: What if someone already set mFirstFreeNode to non-null?
                if(mFirstFreeNode.exchange(node->nextFreeNode) != nullptr)
                {
                    // TODO: Deal with error situation
                }

                // Note: node->nextFreeNode does not need to be changed.
                if(initToZero) mpfr_set_si(node->mFloat, 0, GMP_RNDN);
                ++(node->mRefCount);
                return node;
            }
        }

#ifdef THREAD_SAFETY
        // Acquire the lock during std::list insertion
        std::lock_guard<std::mutex> lk(lock);
#endif
        auto node = &*mData.emplace(mData.end());
        mpfr_init2(node->mFloat, mDefaultPrecision);
        if(initToZero) mpfr_set_si(node->mFloat, 0, GMP_RNDN);
        return node;
    }

    void releaseMpfrFloatData(MpfrFloatData* data)
    {
        // Note: may be called from locked context, in setDefaultPrecision--safely_deallocate
#ifdef THREAD_SAFETY
        if(data->mRefCount.fetch_sub(1) == 1)
        {
            data->nextFreeNode = nullptr;
            data->nextFreeNode = mFirstFreeNode.exchange(data);
        }
#else
        if(--(data->mRefCount) == 0)
        {
            data->nextFreeNode = mFirstFreeNode;
            mFirstFreeNode = data;
        }
#endif
    }

    void setDefaultPrecision(unsigned long bits)
    {
        if(bits != mDefaultPrecision)
        {
#ifdef THREAD_SAFETY
            std::lock_guard<std::mutex> lk(lock);
            if(bits != mDefaultPrecision)
#endif
            {
                mDefaultPrecision = bits;
#ifdef THREAD_SAFETY
                if(!mpfr_is_thread_safe)
                {
                    for(auto& data: mData)
                    {
                        /* TODO: Figure out a way to do this in a thread-safe manner */
                        mpfr_prec_round(data.mFloat, bits, GMP_RNDN);
                    }

                    /* Release the constants so that they will be recalculated
                     * under locking contexts.
                     */
                    if(mConst_pi)      { safely_deallocate(mConst_pi); }
                    if(mConst_e)       { safely_deallocate(mConst_e); }
                    if(mConst_log2)    { safely_deallocate(mConst_log2); }
                    if(mConst_epsilon) { safely_deallocate(mConst_epsilon); }
                }
                else
#endif
                {
                    // If mpfr is thread-safe, then these operations are safe to do
                    // while another thread is perhaps using the element at the same time.
                    for(auto& data: mData)
                        mpfr_prec_round(data.mFloat, bits, GMP_RNDN);

                    if(mConst_pi)
                    {
                        mpfr_const_pi(mConst_pi->mFloat, GMP_RNDN);
                    }
                    if(mConst_e)
                    {
                        recalculate_e(*mConst_e);
                    }
                    if(mConst_log2)
                    {
                        mpfr_const_log2(mConst_log2->mFloat, GMP_RNDN);
                    }
                    if(mConst_epsilon)
                    {
                        recalculateEpsilon(*mConst_epsilon);
                    }
                }
            }
        }
    }

    unsigned long getDefaultPrecision() const
    {
        return mDefaultPrecision;
    }

    MpfrFloatData* const_0()
    {
        return make_const(mConst_0, [](MpfrFloatData& ){});
    }

    MpfrFloat const_pi()
    {
        return make_const(mConst_pi, [](MpfrFloatData& data){
            mpfr_const_pi(data.mFloat, GMP_RNDN);
        });
    }

    MpfrFloat const_e()
    {
        return make_const(mConst_e, [this](MpfrFloatData& data){
            recalculate_e(data);
        });
    }

    MpfrFloat const_log2()
    {
        return make_const(mConst_log2, [](MpfrFloatData& data){
            mpfr_const_log2(data.mFloat, GMP_RNDN);
        });
    }

    MpfrFloat const_epsilon()
    {
        return make_const(mConst_epsilon, [this](MpfrFloatData& data){
            recalculateEpsilon(data);
        });
    }
private:
    template<typename F>
    inline MpfrFloatData* make_const(MpfrFloatData*& pointer, F&& initializer)
    {
        if(!pointer)
        {
            MpfrFloatData* value = allocateMpfrFloatData(true);
#ifdef THREAD_SAFETY
            std::lock_guard<std::mutex> lk(lock);
            if(pointer)
            {
                releaseMpfrFloatData(value);
            }
            else
#endif
            {
                initializer(*value);
                pointer = value;
            }
        }
        return pointer;
    }

    void recalculate_e(MpfrFloatData& data)
    {
        // TODO: Make this sequence atomic (needed in setDefaultPrecision)
        mpfr_set_si(data.mFloat, 1, GMP_RNDN);
        mpfr_exp(data.mFloat, data.mFloat, GMP_RNDN);
    }

    void recalculateEpsilon(MpfrFloatData& data)
    {
        // TODO: Make this sequence atomic (needed in setDefaultPrecision)
        mpfr_set_si(data.mFloat, 1, GMP_RNDN);
        mpfr_div_2ui(data.mFloat, data.mFloat, mDefaultPrecision*7/8 - 1, GMP_RNDN);
    }

#ifdef THREAD_SAFETY
    void safely_deallocate(MpfrFloatData *&ptr)
    {
        MpfrFloatData* copy = ptr;
        ptr = nullptr;
        releaseMpfrFloatData(copy);
    }
#endif
};


//===========================================================================
// Shared data
//===========================================================================
// This should ensure that the container is not accessed by any MpfrFloat
// instance before it has been constructed or after it has been destroyed
// (which might otherwise happen if MpfrFloat is instantiated globally.)
MpfrFloat::MpfrFloatDataContainer& MpfrFloat::mpfrFloatDataContainer()
{
    static MpfrFloat::MpfrFloatDataContainer container;
    return container;
}


//===========================================================================
// Auxiliary functions
//===========================================================================
void MpfrFloat::setDefaultMantissaBits(unsigned long bits)
{
    mpfrFloatDataContainer().setDefaultPrecision(bits);
}

unsigned long MpfrFloat::getCurrentDefaultMantissaBits()
{
    return mpfrFloatDataContainer().getDefaultPrecision();
}

inline void MpfrFloat::copyIfShared()
{
    if(mData->mRefCount > 1)
    {
        MpfrFloatData* oldData = mData;
        mData = mpfrFloatDataContainer().allocateMpfrFloatData(false);
        mpfr_set(mData->mFloat, oldData->mFloat, GMP_RNDN);
        // Release the ref _after_ a copy has been made successfully
        --(oldData->mRefCount);
    }
}

inline void MpfrFloat::resetIfShared(bool has_value)
{
    if(!has_value) // called from constructor?
    {
        mData = mpfrFloatDataContainer().allocateMpfrFloatData(false);
    }
    else // not from constructor
    {
        if(mData->mRefCount > 1)
        {
            // TODO: Is this thread-safe?
            --(mData->mRefCount);
            mData = mpfrFloatDataContainer().allocateMpfrFloatData(false);
        }
    }
}

inline void MpfrFloat::set_0(bool has_value)
{
    MpfrFloatData* newData = mpfrFloatDataContainer().const_0();
    ++(newData->mRefCount);

    if(!has_value) // called from constructor?
    {
        mData = newData;
    }
    else // not from constructor
    {
        // Change to new data
        MpfrFloatData* oldData = mData;
        mData = newData;
        // Release old data
        mpfrFloatDataContainer().releaseMpfrFloatData(oldData);
    }
}


//===========================================================================
// Constructors, destructor, assignment
//===========================================================================
MpfrFloat::MpfrFloat(DummyType)
{
    resetIfShared(false);
}

MpfrFloat::MpfrFloat(MpfrFloatData* data):
    mData(data)
{
    assert(data != nullptr);
    ++(mData->mRefCount);
}

MpfrFloat::MpfrFloat()
{
    set_0(false);
}

MpfrFloat::MpfrFloat(double value)
{
    if(value == 0.0)
    {
        set_0(false);
    }
    else
    {
        resetIfShared(false);
        mpfr_set_d(mData->mFloat, value, GMP_RNDN);
    }
}

MpfrFloat::MpfrFloat(long double value)
{
    if(value == 0.0L)
    {
        set_0(false);
    }
    else
    {
        resetIfShared(false);
        mpfr_set_ld(mData->mFloat, value, GMP_RNDN);
    }
}

MpfrFloat::MpfrFloat(long value)
{
    if(value == 0)
    {
        set_0(false);
    }
    else
    {
        resetIfShared(false);
        mpfr_set_si(mData->mFloat, value, GMP_RNDN);
    }
}

MpfrFloat::MpfrFloat(int value)
{
    if(value == 0)
    {
        set_0(false);
    }
    else
    {
        resetIfShared(false);
        mpfr_set_si(mData->mFloat, value, GMP_RNDN);
    }
}

MpfrFloat::MpfrFloat(const char* value, char** endptr):
    mData(mpfrFloatDataContainer().allocateMpfrFloatData(false))
{
    mpfr_strtofr(mData->mFloat, value, endptr, 0, GMP_RNDN);
}

MpfrFloat::~MpfrFloat()
{
    mpfrFloatDataContainer().releaseMpfrFloatData(mData);
}

MpfrFloat::MpfrFloat(const MpfrFloat& rhs):
    mData(rhs.mData)
{
    ++(mData->mRefCount);
}

MpfrFloat::MpfrFloat(MpfrFloat&& rhs): mData(nullptr)
{
    set_0(false);
    std::swap(mData, rhs.mData);
}

MpfrFloat& MpfrFloat::operator=(const MpfrFloat& rhs)
{
    if(mData != rhs.mData)
    {
        mpfrFloatDataContainer().releaseMpfrFloatData(mData);
        mData = rhs.mData;
        ++(mData->mRefCount);
    }
    return *this;
}

MpfrFloat& MpfrFloat::operator=(MpfrFloat&& rhs)
{
    if(this != &rhs)
    {
        set_0(true);
        std::swap(mData, rhs.mData);
    }
    return *this;
}

MpfrFloat& MpfrFloat::operator=(double value)
{
    if(value == 0.0)
    {
        set_0(true);
    }
    else
    {
        resetIfShared(true);
        mpfr_set_d(mData->mFloat, value, GMP_RNDN);
    }
    return *this;
}

MpfrFloat& MpfrFloat::operator=(long double value)
{
    if(value == 0.0L)
    {
        set_0(true);
    }
    else
    {
        resetIfShared(true);
        mpfr_set_ld(mData->mFloat, value, GMP_RNDN);
    }
    return *this;
}

MpfrFloat& MpfrFloat::operator=(long value)
{
    if(value == 0)
    {
        set_0(true);
    }
    else
    {
        resetIfShared(true);
        mpfr_set_si(mData->mFloat, value, GMP_RNDN);
    }
    return *this;
}

MpfrFloat& MpfrFloat::operator=(int value)
{
    if(value == 0)
    {
        set_0(true);
    }
    else
    {
        resetIfShared(true);
        mpfr_set_si(mData->mFloat, value, GMP_RNDN);
    }
    return *this;
}

/*
MpfrFloat& MpfrFloat::operator=(const char* value)
{
    if(mData->mRefCount > 1)
    {
        --(mData->mRefCount);
        mData = mpfrFloatDataContainer().allocateMpfrFloatData(false);
    }

    mpfr_set_str(mData->mFloat, value, 10, GMP_RNDN);
    return *this;
}
*/

void MpfrFloat::parseValue(const char* value)
{
    resetIfShared(true);
    mpfr_set_str(mData->mFloat, value, 10, GMP_RNDN);
}

void MpfrFloat::parseValue(const char* value, char** endptr)
{
    resetIfShared(true);
    mpfr_strtofr(mData->mFloat, value, endptr, 0, GMP_RNDN);
}


//===========================================================================
// Data getters
//===========================================================================
template<>
void MpfrFloat::get_raw_mpfr_data<mpfr_t>(mpfr_t& dest_mpfr_t)
{
    std::memcpy(&dest_mpfr_t, mData->mFloat, sizeof(mpfr_t));
}

const char* MpfrFloat::getAsString(unsigned precision) const
{
#if(MPFR_VERSION_MAJOR < 2 || (MPFR_VERSION_MAJOR == 2 && MPFR_VERSION_MINOR < 4))
    static const char* const retval =
        "[mpfr_snprintf() is not supported in mpfr versions prior to 2.4]";
    return retval;
#else
    static thread_local std::vector<char> str;
    str.resize(precision+30);
    mpfr_snprintf(&(str[0]), precision+30, "%.*RNg", precision, mData->mFloat);
    return &(str[0]);
#endif
}

bool MpfrFloat::isInteger() const
{
    return mpfr_integer_p(mData->mFloat) != 0;
}

long MpfrFloat::toInt() const
{
    return mpfr_get_si(mData->mFloat, GMP_RNDN);
}

double MpfrFloat::toDouble() const
{
    return mpfr_get_d(mData->mFloat, GMP_RNDN);
}


//===========================================================================
// Modifying operators
//===========================================================================
MpfrFloat& MpfrFloat::operator+=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_add(mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator+=(double value)
{
    copyIfShared();
    mpfr_add_d(mData->mFloat, mData->mFloat, value, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator-=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_sub(mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator-=(double value)
{
    copyIfShared();
    mpfr_sub_d(mData->mFloat, mData->mFloat, value, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator*=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_mul(mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator*=(double value)
{
    copyIfShared();
    mpfr_mul_d(mData->mFloat, mData->mFloat, value, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator/=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_div(mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator/=(double value)
{
    copyIfShared();
    mpfr_div_d(mData->mFloat, mData->mFloat, value, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator%=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_fmod(mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return *this;
}


//===========================================================================
// Modifying functions
//===========================================================================
void MpfrFloat::negate()
{
    copyIfShared();
    mpfr_neg(mData->mFloat, mData->mFloat, GMP_RNDN);
}

void MpfrFloat::abs()
{
    copyIfShared();
    mpfr_abs(mData->mFloat, mData->mFloat, GMP_RNDN);
}


//===========================================================================
// Non-modifying operators
//===========================================================================
MpfrFloat MpfrFloat::operator+(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_add(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator+(double value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_add_d(retval.mData->mFloat, mData->mFloat, value, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator-(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_sub(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator-(double value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_sub_d(retval.mData->mFloat, mData->mFloat, value, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator*(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_mul(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator*(double value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_mul_d(retval.mData->mFloat, mData->mFloat, value, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator/(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_div(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator/(double value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_div_d(retval.mData->mFloat, mData->mFloat, value, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator%(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_fmod(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator-() const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_neg(retval.mData->mFloat, mData->mFloat, GMP_RNDN);
    return retval;
}



//===========================================================================
// Comparison operators
//===========================================================================
bool MpfrFloat::operator<(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) < 0;
}

bool MpfrFloat::operator<(double value) const
{
    return mpfr_cmp_d(mData->mFloat, value) < 0;
}

bool MpfrFloat::operator<=(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) <= 0;
}

bool MpfrFloat::operator<=(double value) const
{
    return mpfr_cmp_d(mData->mFloat, value) <= 0;
}

bool MpfrFloat::operator>(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) > 0;
}

bool MpfrFloat::operator>(double value) const
{
    return mpfr_cmp_d(mData->mFloat, value) > 0;
}

bool MpfrFloat::operator>=(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) >= 0;
}

bool MpfrFloat::operator>=(double value) const
{
    return mpfr_cmp_d(mData->mFloat, value) >= 0;
}

bool MpfrFloat::operator==(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) == 0;
}

bool MpfrFloat::operator==(double value) const
{
    return mpfr_cmp_d(mData->mFloat, value) == 0;
}

bool MpfrFloat::operator!=(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) != 0;
}

bool MpfrFloat::operator!=(double value) const
{
    return mpfr_cmp_d(mData->mFloat, value) != 0;
}


//===========================================================================
// Operator functions
//===========================================================================
MpfrFloat operator+(double lhs, const MpfrFloat& rhs)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_add_d(retval.mData->mFloat, rhs.mData->mFloat, lhs, GMP_RNDN);
    return retval;
}

MpfrFloat operator-(double lhs, const MpfrFloat& rhs)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_d_sub(retval.mData->mFloat, lhs, rhs.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat operator*(double lhs, const MpfrFloat& rhs)
{
    return rhs * lhs;
}

MpfrFloat operator/(double lhs, const MpfrFloat& rhs)
{
    return MpfrFloat(lhs) / rhs;
}

MpfrFloat operator%(double lhs, const MpfrFloat& rhs)
{
    return MpfrFloat(lhs) % rhs;
}

std::ostream& operator<<(std::ostream& os, const MpfrFloat& value)
{
    os << value.getAsString(unsigned(os.precision()));
    return os;
}

//===========================================================================
// Static functions
//===========================================================================
MpfrFloat MpfrFloat::log(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_log(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::log2(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_log2(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::log10(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_log10(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::exp(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_exp(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::exp2(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_exp2(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::exp10(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_exp10(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::cos(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_cos(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::sin(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_sin(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::tan(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_tan(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::sec(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_sec(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::csc(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_csc(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::cot(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_cot(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

void MpfrFloat::sincos(const MpfrFloat& value,
                       MpfrFloat& sin,
                       MpfrFloat& cos)
{
    sin.copyIfShared();
    cos.copyIfShared();
    mpfr_sin_cos
        (sin.mData->mFloat, cos.mData->mFloat, value.mData->mFloat, GMP_RNDN);
}

MpfrFloat MpfrFloat::acos(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_acos(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::asin(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_asin(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::atan(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_atan(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::atan2(const MpfrFloat& value1, const MpfrFloat& value2)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_atan2(retval.mData->mFloat,
               value1.mData->mFloat, value2.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::hypot(const MpfrFloat& value1, const MpfrFloat& value2)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_hypot(retval.mData->mFloat,
               value1.mData->mFloat, value2.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::cosh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_cosh(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::sinh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_sinh(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::tanh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_tanh(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::acosh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_acosh(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::asinh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_asinh(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::atanh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_atanh(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::sqrt(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_sqrt(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::cbrt(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_cbrt(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::root(const MpfrFloat& value, unsigned long root)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_rootn_ui(retval.mData->mFloat, value.mData->mFloat, root, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::pow(const MpfrFloat& value1, const MpfrFloat& value2)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_pow(retval.mData->mFloat,
             value1.mData->mFloat, value2.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::pow(const MpfrFloat& value, long exponent)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_pow_si(retval.mData->mFloat, value.mData->mFloat, exponent, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::abs(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_abs(retval.mData->mFloat, value.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::dim(const MpfrFloat& value1, const MpfrFloat& value2)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_dim(retval.mData->mFloat,
             value1.mData->mFloat, value2.mData->mFloat, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::round(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_round(retval.mData->mFloat, value.mData->mFloat);
    return retval;
}

MpfrFloat MpfrFloat::ceil(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_ceil(retval.mData->mFloat, value.mData->mFloat);
    return retval;
}

MpfrFloat MpfrFloat::floor(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_floor(retval.mData->mFloat, value.mData->mFloat);
    return retval;
}

MpfrFloat MpfrFloat::trunc(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_trunc(retval.mData->mFloat, value.mData->mFloat);
    return retval;
}

MpfrFloat MpfrFloat::parseString(const char* str, char** endptr)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_strtofr(retval.mData->mFloat, str, endptr, 0, GMP_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::const_pi()
{
    // Calls MpfrFloat constructor with MpfrFloatData*.
    // The constructor will increment the refcount.
    return mpfrFloatDataContainer().const_pi();
}

MpfrFloat MpfrFloat::const_e()
{
    return mpfrFloatDataContainer().const_e();
}

MpfrFloat MpfrFloat::const_log2()
{
    return mpfrFloatDataContainer().const_log2();
}

MpfrFloat MpfrFloat::someEpsilon()
{
    return mpfrFloatDataContainer().const_epsilon();
}

// Explicit instantiation
template void MpfrFloat::template get_raw_mpfr_data<mpfr_t>(mpfr_t&);
