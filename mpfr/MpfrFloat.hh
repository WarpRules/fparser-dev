#ifndef ONCE_FP_MPFR_FLOAT_
#define ONCE_FP_MPFR_FLOAT_

#include <iostream>

class MpfrFloat
{
 public:
    /* A default of 256 bits will be used unless changed with this function.
       Note that all existing and cached GMP objects will be resized to the
       specified precision (which can be a somewhat heavy operation).
    */
    static void setDefaultMantissaBits(unsigned long bits);

    static unsigned long getCurrentDefaultMantissaBits();

    MpfrFloat();
    MpfrFloat(double value);
    MpfrFloat(const char* value);

    ~MpfrFloat();

    MpfrFloat(const MpfrFloat&);

    MpfrFloat& operator=(const MpfrFloat&);
    MpfrFloat& operator=(double value);
    MpfrFloat& operator=(const char* value);

    void parseValue(const char* value, char** endptr);


    /* This function can be used to retrieve the raw mpfr_t data structure
       used by this object. A pointer to the destination mpfr_t object should
       be given as parameter (the function does not return it directly in
       order to avoid a dependency of this header file with <mpfr.h>.)
       In other words, it can be called like:

         mpfr_t raw_mpfr_data;
         floatValue.get_raw_mpfr_data(&raw_mpfr_data);

       Note that the returned mpf_t should be considered as read-only and
       not be modified from the outside because it may be shared among
       several objects. If the calling code needs to modify the data, it
       should copy it for itself first with the appropriate GMP library
       functions.
     */
    void get_raw_mpfr_data(void* dest_mpfr_t);


    /* Note that the returned char* points to an internal (shared) buffer
       which will be valid until the next time this function is called
       (by any object).
    */
    const char* getAsString() const;


    MpfrFloat& operator+=(const MpfrFloat&);
    MpfrFloat& operator+=(double);
    MpfrFloat& operator-=(const MpfrFloat&);
    MpfrFloat& operator-=(double);
    MpfrFloat& operator*=(const MpfrFloat&);
    MpfrFloat& operator*=(double);
    MpfrFloat& operator/=(const MpfrFloat&);
    MpfrFloat& operator/=(double);
    MpfrFloat& operator%=(const MpfrFloat&);

    void negate();
    void abs();

    MpfrFloat operator+(const MpfrFloat&) const;
    MpfrFloat operator+(double) const;
    MpfrFloat operator-(const MpfrFloat&) const;
    MpfrFloat operator-(double) const;
    MpfrFloat operator*(const MpfrFloat&) const;
    MpfrFloat operator*(double) const;
    MpfrFloat operator/(const MpfrFloat&) const;
    MpfrFloat operator/(double) const;
    MpfrFloat operator%(const MpfrFloat&) const;

    MpfrFloat operator-() const;

    bool operator<(const MpfrFloat&) const;
    bool operator<(double) const;
    bool operator<=(const MpfrFloat&) const;
    bool operator<=(double) const;
    bool operator>(const MpfrFloat&) const;
    bool operator>(double) const;
    bool operator>=(const MpfrFloat&) const;
    bool operator>=(double) const;
    bool operator==(const MpfrFloat&) const;
    bool operator==(double) const;
    bool operator!=(const MpfrFloat&) const;
    bool operator!=(double) const;


 private:
    struct MpfrFloatData;
    class MpfrFloatDataContainer;

    static MpfrFloatDataContainer gMpfrFloatDataContainer;
    MpfrFloatData* mData;

    enum DummyType { kNoInitialization };
    MpfrFloat(DummyType);

    void copyIfShared();

    friend MpfrFloat operator+(double lhs, const MpfrFloat& rhs);
    friend MpfrFloat operator-(double lhs, const MpfrFloat& rhs);
};

MpfrFloat operator+(double lhs, const MpfrFloat& rhs);
MpfrFloat operator-(double lhs, const MpfrFloat& rhs);
MpfrFloat operator*(double lhs, const MpfrFloat& rhs);
MpfrFloat operator/(double lhs, const MpfrFloat& rhs);
MpfrFloat operator%(double lhs, const MpfrFloat& rhs);

inline bool operator<(double lhs, const MpfrFloat& rhs) { return rhs > lhs; }
inline bool operator<=(double lhs, const MpfrFloat& rhs) { return rhs >= lhs; }
inline bool operator>(double lhs, const MpfrFloat& rhs) { return rhs < lhs; }
inline bool operator>=(double lhs, const MpfrFloat& rhs) { return rhs <= lhs; }
inline bool operator==(double lhs, const MpfrFloat& rhs) { return rhs == lhs; }
inline bool operator!=(double lhs, const MpfrFloat& rhs) { return rhs != lhs; }

inline std::ostream& operator<<(std::ostream& os, const MpfrFloat& value)
{
    os << value.getAsString();
    return os;
}

#endif
