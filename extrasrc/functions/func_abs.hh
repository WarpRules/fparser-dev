//$DEP: func_hypot
    template<typename Value_t>
    inline Value_t fp_abs(const Value_t& x) { return std::fabs(x); }

    inline long fp_abs(const long& x) { return x < 0 ? -x : x; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_abs(const MpfrFloat& x) { return MpfrFloat::abs(x); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_abs(const GmpInt& x) { return GmpInt::abs(x); }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline T fp_abs(const std::complex<T>& x)
    {
        return std::abs(x);
        //T extent = fp_max(fp_abs(x.real()), fp_abs(x.imag()));
        //if(extent == T()) return x;
        //return extent * fp_hypot(x.real() / extent, x.imag() / extent);
    }
#endif
