//$DEP: const_precise
//$DEP: comp_equal
//$DEP: func_floor
//$DEP: test_isint

    template<typename Value_t>
    inline bool isEvenInteger(const Value_t& value)
    {
        const Value_t halfValue = value * fp_const_preciseDouble<Value_t>(0.5);
        return fp_equal(halfValue, fp_int(halfValue));
    }

    template<>
    inline bool isEvenInteger(const long& value)
    {
        return value%2 == 0;
    }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    template<>
    inline bool isEvenInteger(const MpfrFloat& value)
    {
        return isInteger(value) && value%2 == MpfrFloat{};
    }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    inline bool isEvenInteger(const GmpInt& value)
    {
        return value%2 == 0;
    }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline bool isEvenInteger(const std::complex<T>& value)
    {
        return !value.imag() && isEvenInteger(value.real());
    }
#endif
