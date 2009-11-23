#include "MpfrFloat.hh"
#include <stdio.h>
#include <mpfr.h>
#include <deque>
#include <vector>
#include <cstring>

//===========================================================================
// Shared data
//===========================================================================
namespace
{
    std::vector<char> gMpfrFloatString;
}

//===========================================================================
// Auxiliary structs
//===========================================================================
struct MpfrFloat::MpfrFloatData
{
    unsigned mRefCount;
    MpfrFloatData* nextFreeNode;
    mpfr_t mFloat;

    MpfrFloatData(): mRefCount(1), nextFreeNode(0) {}
};

class MpfrFloat::MpfrFloatDataContainer
{
    unsigned long mDefaultPrecision;
    std::deque<MpfrFloat::MpfrFloatData> mData;
    MpfrFloat::MpfrFloatData* mFirstFreeNode;

 public:
    MpfrFloatDataContainer(): mDefaultPrecision(256), mFirstFreeNode(0)
    {}

    ~MpfrFloatDataContainer()
    {
        for(size_t i = 0; i < mData.size(); ++i)
            mpfr_clear(mData[i].mFloat);
    }

    MpfrFloat::MpfrFloatData* allocateMpfrFloatData(bool initToZero)
    {
        if(mFirstFreeNode)
        {
            MpfrFloat::MpfrFloatData* node = mFirstFreeNode;
            mFirstFreeNode = node->nextFreeNode;
            if(initToZero) mpfr_set_si(node->mFloat, 0, GMP_RNDN);
            ++(node->mRefCount);
            return node;
        }

        mData.push_back(MpfrFloat::MpfrFloatData());
        mpfr_init2(mData.back().mFloat, mDefaultPrecision);
        if(initToZero) mpfr_set_si(mData.back().mFloat, 0, GMP_RNDN);
        return &mData.back();
    }

    void releaseMpfrFloatData(MpfrFloat::MpfrFloatData* data)
    {
        if(--(data->mRefCount) == 0)
        {
            data->nextFreeNode = mFirstFreeNode;
            mFirstFreeNode = data;
        }
    }

    void setDefaultPrecision(unsigned long bits)
    {
        if(bits != mDefaultPrecision)
        {
            mDefaultPrecision = bits;
            for(size_t i = 0; i < mData.size(); ++i)
                mpfr_set_prec(mData[i].mFloat, bits);
        }
    }
};

MpfrFloat::MpfrFloatDataContainer MpfrFloat::gMpfrFloatDataContainer;


//===========================================================================
// Auxiliary functions
//===========================================================================
void MpfrFloat::setDefaultMantissaBits(unsigned long bits)
{
    gMpfrFloatDataContainer.setDefaultPrecision(bits);
}

void MpfrFloat::get_raw_mpfr_data(void* dest_mpfr_t)
{
    std::memcpy(dest_mpfr_t, &mData->mFloat, sizeof(mpfr_t));
}

inline void MpfrFloat::copyIfShared()
{
    if(mData->mRefCount > 1)
    {
        --(mData->mRefCount);
        MpfrFloatData* oldData = mData;
        mData = gMpfrFloatDataContainer.allocateMpfrFloatData(false);
        mpfr_set(mData->mFloat, oldData->mFloat, GMP_RNDN);
    }
}


//===========================================================================
// Constructors, destructor, assignment
//===========================================================================
MpfrFloat::MpfrFloat(DummyType):
    mData(gMpfrFloatDataContainer.allocateMpfrFloatData(false))
{}

MpfrFloat::MpfrFloat():
    mData(gMpfrFloatDataContainer.allocateMpfrFloatData(true))
{}

MpfrFloat::MpfrFloat(double value):
    mData(gMpfrFloatDataContainer.allocateMpfrFloatData(false))
{
    mpfr_set_d(mData->mFloat, value, GMP_RNDN);
}

MpfrFloat::MpfrFloat(const char* value):
    mData(gMpfrFloatDataContainer.allocateMpfrFloatData(false))
{
    mpfr_set_str(mData->mFloat, value, 10, GMP_RNDN);
}

MpfrFloat::~MpfrFloat()
{
    gMpfrFloatDataContainer.releaseMpfrFloatData(mData);
}

MpfrFloat::MpfrFloat(const MpfrFloat& rhs):
    mData(rhs.mData)
{
    ++(mData->mRefCount);
}

MpfrFloat& MpfrFloat::operator=(const MpfrFloat& rhs)
{
    if(mData != rhs.mData)
    {
        gMpfrFloatDataContainer.releaseMpfrFloatData(mData);
        mData = rhs.mData;
        ++(mData->mRefCount);
    }
    return *this;
}

MpfrFloat& MpfrFloat::operator=(double value)
{
    if(mData->mRefCount > 1)
    {
        --(mData->mRefCount);
        mData = gMpfrFloatDataContainer.allocateMpfrFloatData(false);
    }

    mpfr_set_d(mData->mFloat, value, GMP_RNDN);
    return *this;
}

MpfrFloat& MpfrFloat::operator=(const char* value)
{
    if(mData->mRefCount > 1)
    {
        --(mData->mRefCount);
        mData = gMpfrFloatDataContainer.allocateMpfrFloatData(false);
    }

    mpfr_set_str(mData->mFloat, value, 10, GMP_RNDN);
    return *this;
}


//===========================================================================
// Data getters
//===========================================================================
const char* MpfrFloat::getAsString() const
{
    const unsigned length = mpfr_get_prec(mData->mFloat) / 3 + 10;
    gMpfrFloatString.resize(length);
    mpfr_snprintf(&gMpfrFloatString[0], length, "%RNf", mData->mFloat);
    return &gMpfrFloatString[0];
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
