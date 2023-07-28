    template<typename Value_t>
    inline Value_t fp_ceil(const Value_t& x) { return std::ceil(x); }

    inline long fp_ceil(const long& x) { return x; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_ceil(const MpfrFloat& x) { return MpfrFloat::ceil(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_ceil(const GmpInt& x) { return x; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_ceil(const std::complex<T>& x)
    {
        return std::complex<T> (fp_ceil(x.real()), fp_ceil(x.imag()));
    }
#endif
