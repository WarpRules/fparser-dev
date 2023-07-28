//$DEP: const_precise
//$DEP: util_isinttype
//$DEP: util_complexcompare

    template<typename Value_t>
    inline bool fp_absTruth(const Value_t& abs_d)
    {
        return IsIntType<Value_t>::value
                ? abs_d > Value_t()
                : abs_d >= fp_const_preciseDouble<Value_t>(0.5);
    }
