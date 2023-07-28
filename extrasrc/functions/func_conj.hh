    template<typename Value_t>
    inline const Value_t& fp_conj(const Value_t& x) { return x; }

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_conj(const std::complex<T>& x)
    {
        return std::conj(x);
    }
#endif
