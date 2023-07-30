    template<typename Value_t>
    inline const Value_t& fp_min(const Value_t& d1, const Value_t& d2)
    {
        return d1<d2 ? d1 : d2;
    }

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_min(const std::complex<T>& x, const std::complex<T>& y)
    {
        return fp_abs(x).real() < fp_abs(y).real() ? x : y;
    }
#endif
