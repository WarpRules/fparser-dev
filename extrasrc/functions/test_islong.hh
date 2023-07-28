//$DEP: help_makelong

    // Is value an integer that fits in "long" datatype?
    template<typename Value_t>
    inline bool isLongInteger(const Value_t& value)
    {
        return value == Value_t( makeLongInteger(value) );
    }

    template<>
    inline bool isLongInteger(const long&) { return true; }
