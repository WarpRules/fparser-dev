//$DEP: help_sincos
#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T> // Internal use in fpaux.hh
    inline std::complex<T> fp_polar_scalar(const T& x, const T& y)
    {
        T si, co; fp_sinCos(si, co, y);
        return std::complex<T> (x*co, x*si);
    }
#endif
