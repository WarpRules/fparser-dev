//$DEP: comp_equal
//$DEP: func_floor

    template<typename Value_t>
    inline bool isInteger(const Value_t& value)
    {
        return fp_equal(value, fp_floor(value));
    }

    template<>
    inline bool isInteger(const long&) { return true; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline bool isInteger(const MpfrFloat& value) { return value.isInteger(); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    inline bool isInteger(const GmpInt&) { return true; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline bool isInteger(const std::complex<T>& value)
    {
        return !value.imag() && isInteger(value.real());
    }
#endif
