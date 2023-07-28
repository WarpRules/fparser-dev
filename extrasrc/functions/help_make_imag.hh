    template<typename Value_t>
    inline const Value_t fp_make_imag(const Value_t& ) // Imaginary 1. In real mode, always zero.
    {
        return Value_t();
    }

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline const std::complex<T> fp_make_imag(const std::complex<T>& v)
    {
        return std::complex<T> ( T(), v.real() );
    }
#endif
