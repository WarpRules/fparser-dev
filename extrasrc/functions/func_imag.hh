    template<typename Value_t>
    inline Value_t fp_imag(const Value_t& ) { return Value_t{}; }

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline T fp_imag(const std::complex<T>& x)
    {
        return x.imag();
    }
#endif
