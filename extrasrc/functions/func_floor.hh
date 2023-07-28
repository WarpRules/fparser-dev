    template<typename Value_t>
    inline Value_t fp_floor(const Value_t& x) { return std::floor(x); }

    inline long fp_floor(const long& x) { return x; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_floor(const MpfrFloat& x) { return MpfrFloat::floor(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_floor(const GmpInt& x) { return x; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_floor(const std::complex<T>& x)
    {
        return std::complex<T> (fp_floor(x.real()), fp_floor(x.imag()));
    }
#endif
