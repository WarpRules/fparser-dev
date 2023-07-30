//$DEP: const_precise
//$DEP: comp_equal
//$DEP: func_floor
//$DEP: test_isint

    template<typename Value_t>
    inline bool isOddInteger(const Value_t& value)
    {
        const Value_t halfValue = (value + Value_t(1)) * fp_const_preciseDouble<Value_t>(0.5);
        return fp_equal(halfValue, fp_int(halfValue));
    }

    template<>
    inline bool isOddInteger(const long& value)
    {
        return value%2 != 0;
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline bool isOddInteger(const MpfrFloat& value)
    {
        return value.isInteger() && value%2 != MpfrFloat{};
    }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    inline bool isOddInteger(const GmpInt& value)
    {
        return value%2 != 0;
    }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline bool isOddInteger(const std::complex<T>& value)
    {
        return !value.imag() && isOddInteger(value.real());
    }
#endif
