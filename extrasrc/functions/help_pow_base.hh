//$DEP: func_exp
//$DEP: func_log
//$DEP: help_inv
//$DEP: test_isint
//$DEP: test_islong
//$DEP: help_makelong

    template<typename Value_t>
    inline Value_t fp_pow_base(const Value_t& x, const Value_t& y)
    {
        return std::pow(x, y);
    }

    inline long fp_pow_base(const long&, const long&) { return 0; }

#ifdef FP_SUPPORT_MPFR_FLOAT_TYPE
    inline MpfrFloat fp_pow_base(const MpfrFloat& x, const MpfrFloat& y)
    {
        return MpfrFloat::pow(x,y);
    }
#endif

#ifdef FP_SUPPORT_GMP_INT_TYPE
    inline GmpInt fp_pow_base(const GmpInt&, const GmpInt&) { return 0; }
#endif
