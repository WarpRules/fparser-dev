#ifdef FP_SUPPORT_COMPLEX_NUMBERS
    /* libstdc++ already defines a streaming operator for complex values,
     * but we redefine our own that it is compatible with the input
     * accepted by fparser. I.e. instead of (5,3) we now get (5+3i),
     * and instead of (-4,0) we now get -4.
     */
    template<typename T>
    inline std::ostream& operator<<(std::ostream& os, const std::complex<T>& value)
    {
        if(!std::signbit(value.imag()) && value.imag() == T()) return os << value.real();
        if(!std::signbit(value.real()) && value.real() == T()) return os << value.imag() << 'i';
        if(value.imag() < T())
            return os << '(' << value.real() << "-" << -value.imag() << "i)";
        else
            return os << '(' << value.real() << "+" << value.imag() << "i)";
    }
#endif
