#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    /* Less-than or greater-than operators are not technically defined
     * for Complex types. However, in fparser and its tool set, these
     * operators are widely required to be present.
     * Our implementation here is based on converting the complex number
     * into a scalar and the doing a scalar comparison on the value.
     * The means by which the number is changed into a scalar is based
     * on the following principles:
     * - Does not introduce unjustified amounts of extra inaccuracy
     * - Is transparent to purely real values
     *     (this disqualifies something like x.real() + x.imag())
     * - Does not ignore the imaginary value completely
     *     (this may be relevant especially in testbed)
     * - Is not so complicated that it would slow down a great deal
     *
     * Basically our formula here is the same as std::abs(),
     * except that it keeps the sign of the original real number,
     * and it does not do a sqrt() calculation that is not really
     * needed because we are only interested in the relative magnitudes.
     *
     * Equality and nonequality operators must not need to be overloaded.
     * They are already implemented in standard, and we must
     * not introduce flawed equality assumptions.
     */
    template<typename T>
    inline T fp_complexScalarize(const std::complex<T>& x)
    {
        T res(std::norm(x));
        if(x.real() < T()) res = -res;
        return res;
    }
    template<typename T>
    inline T fp_realComplexScalarize(const T& x)
    {
        T res(x*x);
        if(x < T()) res = -res;
        return res;
    }
    //    { return x.real() * (T(1.0) + fp_abs(x.imag())); }
    #define d(op) \
    template<typename T> \
    inline bool operator op (const std::complex<T>& x, T y) \
        { return fp_complexScalarize(x) op fp_realComplexScalarize(y); } \
    template<typename T> \
    inline bool operator op (const std::complex<T>& x, const std::complex<T>& y) \
        { return fp_complexScalarize(x) op \
                 fp_complexScalarize(y); } \
    template<typename T> \
    inline bool operator op (T x, const std::complex<T>& y) \
        { return fp_realComplexScalarize(x) op fp_complexScalarize(y); }
    d( < ) d( <= ) d( > ) d( >= )
    #undef d
#endif
