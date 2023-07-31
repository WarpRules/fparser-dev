//$DEP: util_epsilon
//$DEP: util_isinttype
//$DEP: util_complexcompare
//$DEP: func_abs

    template<typename Value_t>
    inline bool fp_equal(const Value_t& x, const Value_t& y)
    {
        return IsIntType<Value_t>::value
            ? (x == y)
            : (fp_abs(x - y) <= fp_real(Epsilon<Value_t>::value));
    }
