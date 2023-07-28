//$DEP: const_precise
//$DEP: func_abs
//$DEP: util_isinttype
//$DEP: util_complexcompare

    template<typename Value_t>
    inline bool fp_truth(const Value_t& d)
    {
        return IsIntType<Value_t>::value
                ? d != Value_t()
                : fp_abs(d) >= fp_const_preciseDouble<Value_t>(0.5);
    }
