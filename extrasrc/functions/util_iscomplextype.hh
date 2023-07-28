    template<typename>
    struct IsComplexType: public std::false_type { };

#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    template<typename T>
    struct IsComplexType<std::complex<T> >: public std::true_type { };
#endif
