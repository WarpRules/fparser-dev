//$DEP: func_log
//$DEP: func_exp
//$DEP: func_arg
//$DEP: help_polar_scalar
//$DEP: help_makelong
//$DEP: help_pow_base
//$DEP: util_fastcomplex
//$DEP: test_islong
//$DEP: test_isint
//$DEP: help_inv

    // Commented versions in fparser.cc
    template<typename Value_t>
    inline Value_t fp_pow_with_exp_log(const Value_t& x, const Value_t& y)
    {
        // Exponentiation using exp(log(x)*y).
        // See http://en.wikipedia.org/wiki/Exponentiation#Real_powers
        // Requirements: x > 0.
        return fp_exp(fp_log(x) * y);
    }

    template<typename Value_t>
    inline Value_t fp_powi(Value_t x, unsigned long y)
    {
        // Fast binary exponentiation algorithm
        // See http://en.wikipedia.org/wiki/Exponentiation_by_squaring
        // Requirements: y is non-negative integer.
        Value_t result(1);
        while(y != 0)
        {
            if(y & 1) { result *= x; y -= 1; }
            else      { x *= x;      y /= 2; }
        }
        return result;
    }

    /* fp_pow() is a wrapper for std::pow()
     * that produces an identical value for
     * exp(1) ^ 2.0  (0x4000000000000000)
     * as exp(2.0)   (0x4000000000000000)
     * - std::pow() on x86_64
     * produces 2.0  (0x3FFFFFFFFFFFFFFF) instead!
     * See comments below for other special traits.
     */
    template<typename Value_t>
    Value_t fp_pow(const Value_t& x, const Value_t& y)
    {
        if(x == Value_t(1)) return Value_t(1);
        // y is now zero or positive
        if(isLongInteger(y))
        {
            // Use fast binary exponentiation algorithm
            if(y >= Value_t(0))
                return fp_powi(x,         makeLongInteger(y));
            else
                return fp_inv(fp_powi(x, -makeLongInteger(y)));
        }
        if(y >= Value_t(0))
        {
            // y is now positive. Calculate using exp(log(x)*y).
            if(x > Value_t(0)) return fp_pow_with_exp_log(x, y);
            if(x == Value_t(0)) return Value_t(0);
            // At this point, y > 0.0 and x is known to be < 0.0,
            // because positive and zero cases are already handled.
            if(x < 0 && isLongInteger(fp_inv(y)))
                return -fp_pow_with_exp_log(-x, y);
            // ^This is not technically correct, but it allows
            // functions such as cbrt(x^5), that is, x^(5/3),
            // to be evaluated when x is negative.
            // It is too complicated (and slow) to test whether y
            // is a formed from a ratio of an integer to an odd integer.
            // (And due to floating point inaccuracy, pointless too.)
            // For example, x^1.30769230769... is
            // actually x^(17/13), i.e. (x^17) ^ (1/13).
            // (-5)^(17/13) gives us now -8.204227562330453.
            // To see whether the result is right, we can test the given
            // root: (-8.204227562330453)^13 gives us the value of (-5)^17,
            // which proves that the expression was correct.
            //
            // The y*16 check prevents e.g. (-4)^(3/2) from being calculated,
            // as it would confuse functioninfo when pow() returns no error
            // but sqrt() does when the formula is converted into sqrt(x)*x.
            //
            // The errors in this approach are:
            //     (-2)^sqrt(2) should produce NaN
            //                  or actually sqrt(2)I + 2^sqrt(2),
            //                  produces -(2^sqrt(2)) instead.
            //                  (Impact: Neglible)
            // Thus, at worst, we're changing a NaN (or complex)
            // result into a negative real number result.
        }
        else
        {
            // y is negative. Utilize the x^y = 1/(x^-y) identity.
            if(x > Value_t(0)) return fp_pow_with_exp_log(Value_t(1) / x, -y);
            if(x < Value_t(0))
            {
                if(!isInteger(y*Value_t(-16)))
                    return -fp_pow_with_exp_log(Value_t(-1) / x, -y);
                // ^ See comment above.
            }
            // Remaining case: 0.0 ^ negative number
        }
        // This is reached when:
        //      x=0, and y<0
        //      x<0, and y*16 is either positive or negative integer
        // It is used for producing error values and as a safe fallback.
        return fp_pow_base(x, y);
    }

    inline long fp_pow(const long&, const long&) { return 0; }

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
