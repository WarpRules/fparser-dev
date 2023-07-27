namespace
{
    /* These functions in fparser produce bool values. However,
     * the testing functions require that they produce Value_t's. */
    #define BoolProxy(Fname) \
    template<typename Value_t> \
    inline Value_t tb_##Fname(const Value_t& a, const Value_t& b) \
        { return Value_t(FUNCTIONPARSERTYPES::Fname(a,b)); }

    BoolProxy(fp_less)
    BoolProxy(fp_lessOrEq)
    BoolProxy(fp_greater)
    BoolProxy(fp_greaterOrEq)
    BoolProxy(fp_equal)
    BoolProxy(fp_nequal)

    template<typename Value_t>
    inline Value_t tb_fp_truth(const Value_t& a)
    { return Value_t(FUNCTIONPARSERTYPES::fp_truth(a)); }

    #define fp_less tb_fp_less
    #define fp_lessOrEq tb_fp_lessOrEq
    #define fp_greater tb_fp_greater
    #define fp_greaterOrEq tb_fp_greaterOrEq
    #define fp_equal tb_fp_equal
    #define fp_nequal tb_fp_nequal
    #define fp_truth tb_fp_truth
}
