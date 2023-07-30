//$DEP: func_int

    template<typename Value_t>
    inline long makeLongInteger(const Value_t& value)
    {
#ifdef FP_SUPPORT_CPLUSPLUS11_MATH_FUNCS
        return std::lround(value);
#else
        return (long) fp_int(value);
#endif
    }

    template<>
    inline long makeLongInteger(const long& value)
    {
        return value;
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline long makeLongInteger(const MpfrFloat& value)
    {
        return (long) value.toInt();
    }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    inline long makeLongInteger(const GmpInt& value)
    {
        return (long) value.toInt();
    }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline long makeLongInteger(const std::complex<T>& value)
    {
        return (long) fp_int( std::abs(value) );
    }
#endif
