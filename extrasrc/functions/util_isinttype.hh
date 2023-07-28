/* Everything is non-int except long and GmpInt */

    template<typename>
    struct IsIntType: public std::false_type { };

    template<>
    struct IsIntType<long>: public std::true_type { };

#ifdef FP_SUPPORT_GMP_INT_TYPE
    template<>
    struct IsIntType<GmpInt>: public std::true_type { };
#endif
