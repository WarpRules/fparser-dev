#if defined(FP_USE_THREAD_SAFE_EVAL) || defined(FP_USE_THREAD_SAFE_EVAL_WITH_ALLOCA)
# include <atomic>
# include <mutex>
# define THREAD_SAFETY
#endif

//#define REUSE_ITEMS

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
#ifdef REUSE_ITEMS
  #ifdef THREAD_SAFETY
    std::list<MpfrFloatData> released_items;
  #else
    std::deque<MpfrFloatData> released_items;
  #endif
#endif
    std::shared_ptr<MpfrFloatData> mConst_0;
    std::shared_ptr<MpfrFloatData> mConst_pi;
    std::shared_ptr<MpfrFloatData> mConst_e;
    std::shared_ptr<MpfrFloatData> mConst_log2;
    std::shared_ptr<MpfrFloatData> mConst_log10;
    std::shared_ptr<MpfrFloatData> mConst_log2inv;
    std::shared_ptr<MpfrFloatData> mConst_log10inv;
    std::shared_ptr<MpfrFloatData> mConst_epsilon;

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
#ifdef REUSE_ITEMS
        for(auto& data: released_items)
            mpfr_clear(data.mFloat);
#endif
    }

    std::shared_ptr<MpfrFloatData> allocateMpfrFloatData(bool initToZero)
    {
        auto deleter = [this](MpfrFloatData* data)
        {
            releaseMpfrFloatData(data);
        };
        auto newnode = std::shared_ptr<MpfrFloatData>(new MpfrFloatData, deleter);

#ifdef REUSE_ITEMS
        if(!released_items.empty())
        {
#ifdef THREAD_SAFETY
            // Acquire the lock during std::list fetch+pop
            std::lock_guard<std::mutex> lk(lock);
            if(!released_items.empty())
#endif
            {
                auto& elem = released_items.back();
                *newnode = std::move(elem);
                released_items.pop_back();

                if(initToZero) mpfr_set_ui(newnode->mFloat, 0ul, mpfr_rnd_t::MPFR_RNDN);
                return newnode;
            }
        }
#endif

        mpfr_init2(newnode->mFloat, mDefaultPrecision);
        if(initToZero) mpfr_set_ui(newnode->mFloat, 0ul, mpfr_rnd_t::MPFR_RNDN);
        return newnode;
    }

    inline void releaseMpfrFloatData(MpfrFloatData* data)
    {
#ifdef REUSE_ITEMS
  #ifdef THREAD_SAFETY
        // Acquire the lock during std::list push
        std::lock_guard<std::mutex> lk(lock);
  #endif
        released_items.emplace_back(std::move(*data));
#else
        mpfr_clear(data->mFloat);
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
                    //for(auto& data: current_items)
                    //{
                    //    // FIXME: This call might not be atomic.
                    //    mpfr_prec_round(data.mFloat, bits, mpfr_rnd_t::MPFR_RNDN);
                    //}

                    /* Release the constants so that they will be recalculated
                     * under locking contexts.
                     */
                    if(mConst_pi)      { safely_deallocate(mConst_pi); }
                    if(mConst_e)       { safely_deallocate(mConst_e); }
                    if(mConst_log2)    { safely_deallocate(mConst_log2); }
                    if(mConst_log10)   { safely_deallocate(mConst_log10); }
                    if(mConst_log2inv) { safely_deallocate(mConst_log2inv); }
                    if(mConst_log10inv){ safely_deallocate(mConst_log10inv); }
                    if(mConst_epsilon) { safely_deallocate(mConst_epsilon); }
                }
                else
#endif
                {
                    // If mpfr is thread-safe, then these operations are safe,
                    // even if another thread is using the same element.
                    //for(auto& data: current_items)
                    //    mpfr_prec_round(data.mFloat, bits, mpfr_rnd_t::MPFR_RNDN);

                    if(mConst_pi)
                    {
                        mpfr_const_pi(mConst_pi->mFloat, mpfr_rnd_t::MPFR_RNDN);
                    }
                    if(mConst_e)
                    {
                        recalculate_e(*mConst_e);
                    }
                    if(mConst_log2)
                    {
                        mpfr_const_log2(mConst_log2->mFloat, mpfr_rnd_t::MPFR_RNDN);
                    }
                    if(mConst_log10)
                    {
                        recalculate_log10(*mConst_log10);
                    }
                    if(mConst_log2inv)
                    {
                        recalculate_log2inv(*mConst_log2inv);
                    }
                    if(mConst_log10inv)
                    {
                        recalculate_log10inv(*mConst_log10inv);
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

    std::shared_ptr<MpfrFloatData> const_0()
    {
        return make_const(mConst_0, [](MpfrFloatData& ){});
    }

    std::shared_ptr<MpfrFloatData> const_pi()
    {
        return make_const(mConst_pi, [](MpfrFloatData& data){
            mpfr_const_pi(data.mFloat, mpfr_rnd_t::MPFR_RNDN);
        });
    }

    std::shared_ptr<MpfrFloatData> const_e()
    {
        return make_const(mConst_e, [this](MpfrFloatData& data){
            recalculate_e(data);
        });
    }

    std::shared_ptr<MpfrFloatData> const_log2()
    {
        return make_const(mConst_log2, [](MpfrFloatData& data){
            mpfr_const_log2(data.mFloat, mpfr_rnd_t::MPFR_RNDN);
        });
    }

    std::shared_ptr<MpfrFloatData> const_log10()
    {
        return make_const(mConst_log10, [this](MpfrFloatData& data){
            recalculate_log10(data);
        });
    }

    std::shared_ptr<MpfrFloatData> const_log2inv()
    {
        return make_const(mConst_log2inv, [this](MpfrFloatData& data){
            recalculate_log2inv(data);
        });
    }

    std::shared_ptr<MpfrFloatData> const_log10inv()
    {
        return make_const(mConst_log10inv, [this](MpfrFloatData& data){
            recalculate_log10inv(data);
        });
    }

    std::shared_ptr<MpfrFloatData> const_epsilon()
    {
        return make_const(mConst_epsilon, [this](MpfrFloatData& data){
            recalculateEpsilon(data);
        });
    }
private:
    template<typename F>
    std::shared_ptr<MpfrFloatData> make_const(std::shared_ptr<MpfrFloatData>& pointer, F&& initializer)
    {
        if(!pointer)
        {
            auto value = allocateMpfrFloatData(true);
            initializer(*value);
            pointer = std::move(value);
        }
        return pointer;
    }

    void recalculate_e(MpfrFloatData& data)
    {
        mpfr_set_ui(data.mFloat, 1ul, mpfr_rnd_t::MPFR_RNDN);
        mpfr_exp(data.mFloat, data.mFloat, mpfr_rnd_t::MPFR_RNDN);
    }

    void recalculate_log10(MpfrFloatData& data)
    {
        mpfr_set_ui(data.mFloat, 10ul, mpfr_rnd_t::MPFR_RNDN);
        mpfr_log(data.mFloat, data.mFloat, mpfr_rnd_t::MPFR_RNDN);
    }

    void recalculate_log2inv(MpfrFloatData& data)
    {
        mpfr_set_ui(data.mFloat, 1ul, mpfr_rnd_t::MPFR_RNDN);
        mpfr_div(data.mFloat, data.mFloat, const_log2()->mFloat, mpfr_rnd_t::MPFR_RNDN);
    }

    void recalculate_log10inv(MpfrFloatData& data)
    {
        mpfr_log10(data.mFloat, const_e()->mFloat, mpfr_rnd_t::MPFR_RNDN);
    }

    void recalculateEpsilon(MpfrFloatData& data)
    {
        mpfr_set_ui(data.mFloat, 1ul, mpfr_rnd_t::MPFR_RNDN);
        mpfr_mul_2si(data.mFloat, data.mFloat, -long(mDefaultPrecision*7/8 - 1), mpfr_rnd_t::MPFR_RNDN);
    }

#ifdef THREAD_SAFETY
    void safely_deallocate(std::shared_ptr<MpfrFloatData> &ptr)
    {
        ptr = nullptr;
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

void MpfrFloat::copyIfShared()
{
    if(mData.use_count() > 1)
    {
        auto newData = mpfrFloatDataContainer().allocateMpfrFloatData(false);
        mpfr_set(newData->mFloat, mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
        std::swap(mData, newData);
        // old data goes out of scope, may get deallocated.
    }
}

void MpfrFloat::resetIfShared(bool has_value)
{
    if(!has_value) // called from constructor?
    {
        assert(mData.use_count() == 0);
        mData = mpfrFloatDataContainer().allocateMpfrFloatData(false);
    }
    else // not from constructor
    {
        if(mData.use_count() > 1)
        {
            mData = mpfrFloatDataContainer().allocateMpfrFloatData(false);
        }
    }
}

void MpfrFloat::set_0(bool/*has_value*/)
{
    mData = mpfrFloatDataContainer().const_0();
}


//===========================================================================
// Constructors, destructor, assignment
//===========================================================================
MpfrFloat::MpfrFloat(DummyType) // private constructor, kNoInitialization
{
    resetIfShared(false);
}

MpfrFloat::MpfrFloat(std::shared_ptr<MpfrFloatData>&& data) // private constructor
    : mData(std::move(data))
{
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
        mpfr_set_d(mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
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
        mpfr_set_ld(mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
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
        mpfr_set_si(mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
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
        mpfr_set_si(mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    }
}

MpfrFloat::MpfrFloat(const char* value, char** endptr):
    mData(mpfrFloatDataContainer().allocateMpfrFloatData(false))
{
    mpfr_strtofr(mData->mFloat, value, endptr, 0, mpfr_rnd_t::MPFR_RNDN);
}

MpfrFloat::~MpfrFloat()
{
}

MpfrFloat::MpfrFloat(const MpfrFloat& rhs)
    : mData(rhs.mData)
{
}

MpfrFloat::MpfrFloat(MpfrFloat&& rhs)
    : mData(std::move(rhs.mData))
{
    rhs.set_0(false);
}

MpfrFloat& MpfrFloat::operator=(const MpfrFloat& rhs)
{
    if(this != &rhs)
    {
        mData = rhs.mData;
    }
    return *this;
}

MpfrFloat& MpfrFloat::operator=(MpfrFloat&& rhs)
{
    if(this != &rhs)
    {
        mData = std::move(rhs.mData);
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
        mpfr_set_d(mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
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
        mpfr_set_ld(mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
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
        mpfr_set_si(mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
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
        mpfr_set_si(mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
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

    mpfr_set_str(mData->mFloat, value, 10, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}
*/

void MpfrFloat::parseValue(const char* value)
{
    resetIfShared(true);
    mpfr_set_str(mData->mFloat, value, 10, mpfr_rnd_t::MPFR_RNDN);
}

void MpfrFloat::parseValue(const char* value, char** endptr)
{
    resetIfShared(true);
    mpfr_strtofr(mData->mFloat, value, endptr, 0, mpfr_rnd_t::MPFR_RNDN);
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
    return mpfr_get_si(mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
}

double MpfrFloat::toDouble() const
{
    return mpfr_get_d(mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
}


//===========================================================================
// Modifying operators
//===========================================================================
MpfrFloat& MpfrFloat::operator+=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_add(mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator-=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_sub(mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator*=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_mul(mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator/=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_div(mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator%=(const MpfrFloat& rhs)
{
    copyIfShared();
    mpfr_fmod(mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator+=(double value)
{
    copyIfShared();
    mpfr_add_d(mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator-=(double value)
{
    copyIfShared();
    mpfr_sub_d(mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator*=(double value)
{
    copyIfShared();
    mpfr_mul_d(mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator/=(double value)
{
    copyIfShared();
    mpfr_div_d(mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator+=(long value)
{
    copyIfShared();
    mpfr_add_si(mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator-=(long value)
{
    copyIfShared();
    mpfr_sub_si(mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator*=(long value)
{
    copyIfShared();
    mpfr_mul_si(mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator/=(long value)
{
    copyIfShared();
    mpfr_div_si(mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return *this;
}


//===========================================================================
// Modifying functions
//===========================================================================
void MpfrFloat::negate()
{
    copyIfShared();
    mpfr_neg(mData->mFloat, mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
}

void MpfrFloat::abs()
{
    copyIfShared();
    mpfr_abs(mData->mFloat, mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
}


//===========================================================================
// Non-modifying operators
//===========================================================================
MpfrFloat MpfrFloat::operator+(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_add(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator-(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_sub(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator*(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_mul(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator/(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_div(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator%(const MpfrFloat& rhs) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_fmod(retval.mData->mFloat, mData->mFloat, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator+(double value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_add_d(retval.mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator-(double value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_sub_d(retval.mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator*(double value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_mul_d(retval.mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator/(double value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_div_d(retval.mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator+(long value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_add_si(retval.mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator-(long value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_sub_si(retval.mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator*(long value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_mul_si(retval.mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator/(long value) const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_div_si(retval.mData->mFloat, mData->mFloat, value, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::operator-() const
{
    MpfrFloat retval(kNoInitialization);
    mpfr_neg(retval.mData->mFloat, mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}



//===========================================================================
// Comparison operators
//===========================================================================
bool MpfrFloat::operator<(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) < 0;
}

bool MpfrFloat::operator<=(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) <= 0;
}

bool MpfrFloat::operator>(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) > 0;
}

bool MpfrFloat::operator>=(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) >= 0;
}

bool MpfrFloat::operator==(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) == 0;
}


bool MpfrFloat::operator!=(const MpfrFloat& rhs) const
{
    return mpfr_cmp(mData->mFloat, rhs.mData->mFloat) != 0;
}

bool MpfrFloat::operator<(long value) const
{
    return mpfr_cmp_si(mData->mFloat, value) < 0;
}

bool MpfrFloat::operator<=(long value) const
{
    return mpfr_cmp_si(mData->mFloat, value) <= 0;
}

bool MpfrFloat::operator>(long value) const
{
    return mpfr_cmp_si(mData->mFloat, value) > 0;
}

bool MpfrFloat::operator>=(long value) const
{
    return mpfr_cmp_si(mData->mFloat, value) >= 0;
}

bool MpfrFloat::operator==(long value) const
{
    return mpfr_cmp_si(mData->mFloat, value) == 0;
}

bool MpfrFloat::operator!=(long value) const
{
    return mpfr_cmp_si(mData->mFloat, value) != 0;
}


//===========================================================================
// Operator functions
//===========================================================================
MpfrFloat operator+(long lhs, const MpfrFloat& rhs)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_add_si(retval.mData->mFloat, rhs.mData->mFloat, lhs, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat operator-(long lhs, const MpfrFloat& rhs)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_si_sub(retval.mData->mFloat, lhs, rhs.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat operator*(long lhs, const MpfrFloat& rhs)
{
    return rhs * lhs;
}

MpfrFloat operator/(long lhs, const MpfrFloat& rhs)
{
    return MpfrFloat(lhs) / rhs;
}

MpfrFloat operator%(long lhs, const MpfrFloat& rhs)
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
    mpfr_log(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::log2(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_log2(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::log10(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_log10(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::exp(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_exp(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::exp2(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_exp2(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::exp10(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_exp10(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::cos(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_cos(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::sin(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_sin(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::tan(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_tan(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::sec(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_sec(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::csc(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_csc(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::cot(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_cot(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

void MpfrFloat::sincos(const MpfrFloat& value,
                       MpfrFloat& sin,
                       MpfrFloat& cos)
{
    sin.copyIfShared();
    cos.copyIfShared();
    mpfr_sin_cos
        (sin.mData->mFloat, cos.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
}

void MpfrFloat::sinhcosh(const MpfrFloat& value,
                         MpfrFloat& sinh,
                         MpfrFloat& cosh)
{
    sinh.copyIfShared();
    cosh.copyIfShared();
    mpfr_sinh_cosh
        (sinh.mData->mFloat, cosh.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
}

MpfrFloat MpfrFloat::acos(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_acos(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::asin(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_asin(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::atan(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_atan(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::atan2(const MpfrFloat& value1, const MpfrFloat& value2)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_atan2(retval.mData->mFloat,
               value1.mData->mFloat, value2.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::hypot(const MpfrFloat& value1, const MpfrFloat& value2)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_hypot(retval.mData->mFloat,
               value1.mData->mFloat, value2.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::cosh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_cosh(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::sinh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_sinh(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::tanh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_tanh(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::acosh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_acosh(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::asinh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_asinh(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::atanh(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_atanh(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::sqrt(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_sqrt(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::cbrt(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_cbrt(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::root(const MpfrFloat& value, unsigned long root)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_rootn_ui(retval.mData->mFloat, value.mData->mFloat, root, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::pow(const MpfrFloat& value1, const MpfrFloat& value2)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_pow(retval.mData->mFloat,
             value1.mData->mFloat, value2.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::pow(const MpfrFloat& value, long exponent)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_pow_si(retval.mData->mFloat, value.mData->mFloat, exponent, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::abs(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_abs(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::dim(const MpfrFloat& value1, const MpfrFloat& value2)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_dim(retval.mData->mFloat,
             value1.mData->mFloat, value2.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
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

MpfrFloat MpfrFloat::rsqrt(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_rec_sqrt(retval.mData->mFloat, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::inv(const MpfrFloat& value)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_ui_div(retval.mData->mFloat, 1ul, value.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::fma(const MpfrFloat& value1, const MpfrFloat& value2,
                         const MpfrFloat& value3)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_fma(retval.mData->mFloat,
             value1.mData->mFloat, value2.mData->mFloat,
             value3.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::fms(const MpfrFloat& value1, const MpfrFloat& value2,
                         const MpfrFloat& value3)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_fms(retval.mData->mFloat,
             value1.mData->mFloat, value2.mData->mFloat,
             value3.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::fmma(const MpfrFloat& value1, const MpfrFloat& value2,
                          const MpfrFloat& value3, const MpfrFloat& value4)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_fmma(retval.mData->mFloat,
              value1.mData->mFloat, value2.mData->mFloat,
              value3.mData->mFloat, value4.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::fmms(const MpfrFloat& value1, const MpfrFloat& value2,
                          const MpfrFloat& value3, const MpfrFloat& value4)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_fmms(retval.mData->mFloat,
              value1.mData->mFloat, value2.mData->mFloat,
              value3.mData->mFloat, value4.mData->mFloat, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::parseString(const char* str, char** endptr)
{
    MpfrFloat retval(MpfrFloat::kNoInitialization);
    mpfr_strtofr(retval.mData->mFloat, str, endptr, 0, mpfr_rnd_t::MPFR_RNDN);
    return retval;
}

MpfrFloat MpfrFloat::const_pi()
{
    // Calls private MpfrFloat constructor with MpfrFloatData pointer.
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

MpfrFloat MpfrFloat::const_log10()
{
    return mpfrFloatDataContainer().const_log10();
}

MpfrFloat MpfrFloat::const_log2inv()
{
    return mpfrFloatDataContainer().const_log2inv();
}

MpfrFloat MpfrFloat::const_log10inv()
{
    return mpfrFloatDataContainer().const_log10inv();
}

MpfrFloat MpfrFloat::someEpsilon()
{
    return mpfrFloatDataContainer().const_epsilon();
}

// Explicit instantiation
template void MpfrFloat::template get_raw_mpfr_data<mpfr_t>(mpfr_t&);
