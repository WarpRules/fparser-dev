//$DEP: func_trunc

    template<typename Value_t>
    inline Value_t fp_mod(const Value_t& x, const Value_t& y)
    {
        return std::fmod(x, y);
    }

    inline long fp_mod(const long& x, const long& y) { return x % y; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_mod(const MpfrFloat& x, const MpfrFloat& y) { return x % y; }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_mod(const GmpInt& x, const GmpInt& y) { return x % y; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_mod(const std::complex<T>& x, const std::complex<T>& y)
    {
        // Modulo function is probably not defined for complex numbers.
        // But we do our best to calculate it the same way as it is done
        // with real numbers, so that at least it is in some way "consistent".
        if(y.imag() == T{}) return fp_mod(x.real(), y.real()); // optimization
        std::complex<T> n = fp_trunc(x / y);
        return x - n * y;
    }
#endif
