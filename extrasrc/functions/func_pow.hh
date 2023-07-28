//$DEP: func_log
//$DEP: func_exp
//$DEP: func_arg
//$DEP: help_polar_scalar
//$DEP: help_makelong
//$DEP: util_fastcomplex
//$DEP: test_islong
//$DEP: test_isint

    // Forward declaration of fp_pow_base() to break dependency loop
    template<typename Value_t>
    inline Value_t fp_pow_base(const Value_t& x, const Value_t& y);

    // Commented versions in fparser.cc
    template<typename Value_t>
    inline Value_t fp_pow_with_exp_log(const Value_t& x, const Value_t& y)
    {
        return fp_exp(fp_log(x) * y);
    }

    template<typename Value_t>
    inline Value_t fp_powi(Value_t x, unsigned long y)
    {
        Value_t result(1);
        while(y != 0)
        {
            if(y & 1) { result *= x; y -= 1; }
            else      { x *= x;      y /= 2; }
        }
        return result;
    }

    template<typename Value_t>
    Value_t fp_pow(const Value_t& x, const Value_t& y)
    {
        if(x == Value_t(1)) return Value_t(1);
        if(isLongInteger(y))
        {
            if(y >= Value_t(0))
                return fp_powi(x, makeLongInteger(y));
            else
                return Value_t(1) / fp_powi(x, -makeLongInteger(y));
        }
        if(y >= Value_t(0))
        {
            if(x > Value_t(0)) return fp_pow_with_exp_log(x, y);
            if(x == Value_t(0)) return Value_t(0);
            if(!isInteger(y*Value_t(16)))
                return -fp_pow_with_exp_log(-x, y);
        }
        else
        {
            if(x > Value_t(0)) return fp_pow_with_exp_log(Value_t(1) / x, -y);
            if(x < Value_t(0))
            {
                if(!isInteger(y*Value_t(-16)))
                    return -fp_pow_with_exp_log(Value_t(-1) / x, -y);
            }
        }
        return fp_pow_base(x, y);
    }

    inline long fp_pow(const long&, const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_pow(const MpfrFloat& x, const MpfrFloat& y) { return MpfrFloat::pow(x, y); }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_pow(const GmpInt&, const GmpInt&) { return 0; }
#endif

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    inline std::complex<T> fp_pow(const std::complex<T>& x, const std::complex<T>& y)
    {
        //if(FP_ProbablyHasFastLibcComplex<T>::value)
        //    return std::pow(x,y);

        // With complex numbers, pow(x,y) can be solved with
        // the general formula: exp(y*log(x)). It handles
        // all special cases gracefully.
        // It expands to the following:
        // A)
        //     t1 = log(x)
        //     t2 = y * t1
        //     res = exp(t2)
        // B)
        //     t1.r = log(x.r * x.r + x.i * x.i) * 0.5  \ fp_log()
        //     t1.i = atan2(x.i, x.r)                   /
        //     t2.r = y.r*t1.r - y.i*t1.i          \ multiplication
        //     t2.i = y.r*t1.i + y.i*t1.r          /
        //     rho   = exp(t2.r)        \ fp_exp()
        //     theta = t2.i             /
        //     res.r = rho * cos(theta)   \ fp_polar(), called from
        //     res.i = rho * sin(theta)   / fp_exp(). Uses sincos().
        // Aside from the common "norm" calculation in atan2()
        // and in the log parameter, both of which are part of fp_log(),
        // there does not seem to be any incentive to break this
        // function down further; it would not help optimizing it.
        // However, we do handle the following special cases:
        //
        // When x is real (positive or negative):
        //     t1.r = log(abs(x.r))
        //     t1.i = x.r<0 ? -pi : 0
        // When y is real:
        //     t2.r = y.r * t1.r
        //     t2.i = y.r * t1.i
        const std::complex<T> t =
            (x.imag() != T())
            ? fp_log(x)
            : std::complex<T> (fp_log(fp_abs(x.real())),
                               fp_arg(x.real())); // Note: Uses real-value fp_arg() here!
        return y.imag() != T()
            ? fp_exp(y * t)
            : fp_polar_scalar<T> (fp_exp(y.real()*t.real()), y.real()*t.imag());
    }
#endif
