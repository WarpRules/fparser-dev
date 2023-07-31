//$DEP: const_pi

    // arg() for real numbers
    template<typename Value_t>
    inline Value_t fp_arg(const Value_t& x)
    {
        return x < Value_t() ? -fp_const_pi<Value_t>() : Value_t();
    }

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline T fp_arg(const std::complex<T>& x)
    {
        return std::arg(x);
    }
#endif
