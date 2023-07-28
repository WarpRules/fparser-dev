//$DEP: help_polar_scalar
//$DEP: func_cos
    template<typename Value_t> // Meaningless number for real numbers
    inline Value_t fp_polar(const Value_t& x, const Value_t& y)
        { return x * fp_cos(y); }

    template<typename T>
    inline std::complex<T> fp_polar(const std::complex<T>& x, const std::complex<T>& y)
    {
        // x * cos(y) + i * x * sin(y) -- arguments are supposed to be REAL numbers
        return fp_polar_scalar<T> (x.real(), y.real());
        //return std::polar(x.real(), y.real());
        //return x * (fp_cos(y) + (std::complex<T>(0,1) * fp_sin(y));
    }
