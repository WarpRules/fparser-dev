//$DEP: util_epsilon
//$DEP: util_isinttype
//$DEP: util_complexcompare

    template<typename Value_t>
    inline bool fp_less(const Value_t& x, const Value_t& y)
    {
        return IsIntType<Value_t>::value
            ? (x < y)
            : (x < y - Epsilon<Value_t>::value);
    }

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline bool fp_less(const std::complex<T>& x, const std::complex<T>& y)
    {
        return fp_less(x.real(), y.real());
    }
#endif
