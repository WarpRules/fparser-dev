/* NOTE:
  Do not include this file in your project. The fparser.cc file #includes
this file internally and thus you don't need to do anything (other than keep
this file in the same directory as fparser.cc).

  Part of this file is generated code (by using the make_function_name_parser
utility, found in the development version of this library). It's not intended
to be modified by hand.
*/

        unsigned nameLength = 0;
        const unsigned maximumNameLength = 0x80000000U-8;
        /*
        Due to the manner the identifier lengths are returned from
        the readOpcode() function, the maximum supported length for
        identifiers is 0x7FFFFFFF bytes. We minus 8 here to add some
        buffer, because of the multibyteness of UTF-8.
        Function names are limited to 0xFFFF bytes instead, but because
        function names that long just are not defined, the point is moot.
        */
        const unsigned char* const uptr = (const unsigned char*) input;
        typedef signed char schar;
        while(likely(nameLength < maximumNameLength))
        {
            unsigned char byte = uptr[nameLength+0];
            /* Handle the common case of A-Za-z first */
            if(byte >= 0x40)
            {
                if(byte < 0x80) // 0x40..0x7F - most common case
                {
                    // Valid characters in 40..7F: A-Za-z_
                    // Valid bitmask for 40..5F: 01111111111111111111111111100001
                    // Valid bitmask for 60..7F: 01111111111111111111111111100000
                    if(sizeof(unsigned long) == 8)
                    {
                        const unsigned n = sizeof(unsigned long)*8-32;
                        // ^ avoids compiler warning when not 64-bit
                        unsigned long masklow6bits = 1UL << (byte & 0x3F);
                        if(masklow6bits & ~((1UL << 0) | (0x0FUL << (0x1B  ))
                                          | (1UL << n) | (0x1FUL << (0x1B+n))))
                            { ++nameLength; continue; }
                    }
                    else
                    {
                        unsigned masklow5bits = 1 << (byte & 0x1F);
                        if((masklow5bits & ~(1 | (0x1F << 0x1B))) || byte == '_')
                            { ++nameLength; continue; }
                    }
                    break;
                }
                if(byte < 0xF0)
                {
                    if(byte < 0xE0)
                    {
                        if(byte < 0xC2) break; // 0x80..0xC1
                        if(byte == 0xC2 && uptr[nameLength+1]==0xA0) break; // skip nbsp
                        // C2-DF - next common case when >= 0x40
                        // Valid sequence: C2-DF 80-BF
                        if(schar(uptr[nameLength+1]) > schar(0xBF)) break;
                        nameLength += 2;
                        continue;
                    }
                    if(byte == 0xE0) // E0
                    {
                        // Valid sequence: E0 A0-BF 80-BF
                        if((unsigned char)(uptr[nameLength+1] - 0xA0) > (0xBF-0xA0)) break;
                    }
                    else
                    {
                        if(byte == 0xED) break; // ED is invalid
                        // Valid sequence: E1-EC 80-BF 80-BF
                        //            And: EE-EF 80-BF 80-BF
                        if(byte == 0xE2)
                        {
                            // break on various space characters
                            if(uptr[nameLength+1] == 0x80
                            && (schar(uptr[nameLength+2]) <= schar(0x8B)
                            || (uptr[nameLength+2] == 0xAF))) break;
                            if(uptr[nameLength+1] == 0x81
                            && uptr[nameLength+2] == 0x9F) break;
                        } else
                        if(byte == 0xE3 && uptr[nameLength+1] == 0x80
                        && uptr[nameLength+2] == 0x80) break; // this too

                        if(schar(uptr[nameLength+1]) > schar(0xBF)) break;
                    }
                    if(schar(uptr[nameLength+2]) > schar(0xBF)) break;
                    nameLength += 3;
                    continue;
                }
                if(byte == 0xF0) // F0
                {
                    // Valid sequence: F0 90-BF 80-BF 80-BF
                    if((unsigned char)(uptr[nameLength+1] - 0x90) > (0xBF-0x90)) break;
                }
                else
                {
                    if(byte > 0xF4) break; // F5-FF are invalid
                    if(byte == 0xF4) // F4
                    {
                        // Valid sequence: F4 80-8F
                        if(schar(uptr[nameLength+1]) > schar(0x8F)) break;
                    }
                    else
                    {
                        // F1-F3
                        // Valid sequence: F1-F3 80-BF 80-BF 80-BF
                        if(schar(uptr[nameLength+1]) > schar(0xBF)) break;
                    }
                }
                if(schar(uptr[nameLength+2]) > schar(0xBF)) break;
                if(schar(uptr[nameLength+3]) > schar(0xBF)) break;
                nameLength += 4;
                continue;
            }
            if(nameLength > 0)
            {
                if(sizeof(unsigned long) == 8)
                {
                    // Valid bitmask for 00..1F: 00000000000000000000000000000000
                    // Valid bitmask for 20..3F: 00000000000000001111111111000000
                    const unsigned n = sizeof(unsigned long)*8-32;
                    // ^ avoids compiler warning when not 64-bit
                    unsigned long masklow6bits = 1UL << byte;
                    if(masklow6bits & (((1UL << 10)-1UL) << (16+n)))
                        { ++nameLength; continue; }
                }
                else
                {
                    if(byte >= '0' && byte <= '9')
                        { ++nameLength; continue; }
                }
            }
            break;
        }

        /* This function generated with make_function_name_parser.cc */
        switch(nameLength)
        {
             case 2:
    /* prefix  */if('i' == uptr[0]
    && 'f' == uptr[1]) return (cIf<<16) | 0x80000002U;
    return 2;

             case 3:
    /* prefix  */switch(uptr[0]) {
    case 'a':
    /* prefix a */switch(uptr[1]) {
    case 'b':
    /* prefix ab */if('s' == uptr[2]) return (cAbs<<16) | 0x80000003U;
    return 3;
    case 'r':
    /* prefix ar */if('g' == uptr[2]) return (cArg<<16) | 0x80000003U;
    return 3;
    default: return 3; }
    case 'c':
    /* prefix c */switch(uptr[1]) {
    case 'o':
    /* prefix co */switch(uptr[2]) {
    case 's':
    /* prefix cos */return (cCos<<16) | 0x80000003U;
    case 't':
    /* prefix cot */return (cCot<<16) | 0x80000003U;
    default: return 3; }
    case 's':
    /* prefix cs */if('c' == uptr[2]) return (cCsc<<16) | 0x80000003U;
    return 3;
    default: return 3; }
    case 'e':
    /* prefix e */if('x' == uptr[1]
    && 'p' == uptr[2]) return (cExp<<16) | 0x80000003U;
    return 3;
    case 'i':
    /* prefix i */if('n' == uptr[1]
    && 't' == uptr[2]) return (cInt<<16) | 0x80000003U;
    return 3;
    case 'l':
    /* prefix l */if('o' == uptr[1]
    && 'g' == uptr[2]) return (cLog<<16) | 0x80000003U;
    return 3;
    case 'm':
    /* prefix m */switch(uptr[1]) {
    case 'a':
    /* prefix ma */if('x' == uptr[2]) return (cMax<<16) | 0x80000003U;
    return 3;
    case 'i':
    /* prefix mi */if('n' == uptr[2]) return (cMin<<16) | 0x80000003U;
    return 3;
    default: return 3; }
    case 'p':
    /* prefix p */if('o' == uptr[1]
    && 'w' == uptr[2]) return (cPow<<16) | 0x80000003U;
    return 3;
    case 's':
    /* prefix s */switch(uptr[1]) {
    case 'e':
    /* prefix se */if('c' == uptr[2]) return (cSec<<16) | 0x80000003U;
    return 3;
    case 'i':
    /* prefix si */if('n' == uptr[2]) return (cSin<<16) | 0x80000003U;
    return 3;
    default: return 3; }
    case 't':
    /* prefix t */if('a' == uptr[1]
    && 'n' == uptr[2]) return (cTan<<16) | 0x80000003U;
    return 3;
    default: return 3; }

             case 4:
    /* prefix  */switch(uptr[0]) {
    case 'a':
    /* prefix a */switch(uptr[1]) {
    case 'c':
    /* prefix ac */if('o' == uptr[2]
    && 's' == uptr[3]) return (cAcos<<16) | 0x80000004U;
    return 4;
    case 's':
    /* prefix as */if('i' == uptr[2]
    && 'n' == uptr[3]) return (cAsin<<16) | 0x80000004U;
    return 4;
    case 't':
    /* prefix at */if('a' == uptr[2]
    && 'n' == uptr[3]) return (cAtan<<16) | 0x80000004U;
    return 4;
    default: return 4; }
    case 'c':
    /* prefix c */switch(uptr[1]) {
    case 'b':
    /* prefix cb */if('r' == uptr[2]
    && 't' == uptr[3]) return (cCbrt<<16) | 0x80000004U;
    return 4;
    case 'e':
    /* prefix ce */if('i' == uptr[2]
    && 'l' == uptr[3]) return (cCeil<<16) | 0x80000004U;
    return 4;
    case 'o':
    /* prefix co */switch(uptr[2]) {
    case 'n':
    /* prefix con */if('j' == uptr[3]) return (cConj<<16) | 0x80000004U;
    return 4;
    case 's':
    /* prefix cos */if('h' == uptr[3]) return (cCosh<<16) | 0x80000004U;
    return 4;
    default: return 4; }
    default: return 4; }
    case 'e':
    /* prefix e */{static const char tmp[3] = {'x','p','2'};
    if(std::memcmp(uptr+1, tmp, 3) == 0) return (cExp2<<16) | 0x80000004U;
    return 4; }
    case 'i':
    /* prefix i */{static const char tmp[3] = {'m','a','g'};
    if(std::memcmp(uptr+1, tmp, 3) == 0) return (cImag<<16) | 0x80000004U;
    return 4; }
    case 'l':
    /* prefix l */{static const char tmp[3] = {'o','g','2'};
    if(std::memcmp(uptr+1, tmp, 3) == 0) return (cLog2<<16) | 0x80000004U;
    return 4; }
    case 'r':
    /* prefix r */{static const char tmp[3] = {'e','a','l'};
    if(std::memcmp(uptr+1, tmp, 3) == 0) return (cReal<<16) | 0x80000004U;
    return 4; }
    case 's':
    /* prefix s */switch(uptr[1]) {
    case 'i':
    /* prefix si */if('n' == uptr[2]
    && 'h' == uptr[3]) return (cSinh<<16) | 0x80000004U;
    return 4;
    case 'q':
    /* prefix sq */if('r' == uptr[2]
    && 't' == uptr[3]) return (cSqrt<<16) | 0x80000004U;
    return 4;
    default: return 4; }
    case 't':
    /* prefix t */{static const char tmp[3] = {'a','n','h'};
    if(std::memcmp(uptr+1, tmp, 3) == 0) return (cTanh<<16) | 0x80000004U;
    return 4; }
    default: return 4; }

             case 5:
    /* prefix  */switch(uptr[0]) {
    case 'a':
    /* prefix a */switch(uptr[1]) {
    case 'c':
    /* prefix ac */{static const char tmp[3] = {'o','s','h'};
    if(std::memcmp(uptr+2, tmp, 3) == 0) return (cAcosh<<16) | 0x80000005U;
    return 5; }
    case 's':
    /* prefix as */{static const char tmp[3] = {'i','n','h'};
    if(std::memcmp(uptr+2, tmp, 3) == 0) return (cAsinh<<16) | 0x80000005U;
    return 5; }
    case 't':
    /* prefix at */if('a' == uptr[2]) {
    /* prefix ata */if('n' == uptr[3]) {
    /* prefix atan */switch(uptr[4]) {
    case '2':
    /* prefix atan2 */return (cAtan2<<16) | 0x80000005U;
    case 'h':
    /* prefix atanh */return (cAtanh<<16) | 0x80000005U;
    default: return 5; }
    }return 5;}return 5;default: return 5; }
    case 'f':
    /* prefix f */{static const char tmp[4] = {'l','o','o','r'};
    if(std::memcmp(uptr+1, tmp, 4) == 0) return (cFloor<<16) | 0x80000005U;
    return 5; }
    case 'h':
    /* prefix h */{static const char tmp[4] = {'y','p','o','t'};
    if(std::memcmp(uptr+1, tmp, 4) == 0) return (cHypot<<16) | 0x80000005U;
    return 5; }
    case 'l':
    /* prefix l */{static const char tmp[4] = {'o','g','1','0'};
    if(std::memcmp(uptr+1, tmp, 4) == 0) return (cLog10<<16) | 0x80000005U;
    return 5; }
    case 'p':
    /* prefix p */{static const char tmp[4] = {'o','l','a','r'};
    if(std::memcmp(uptr+1, tmp, 4) == 0) return (cPolar<<16) | 0x80000005U;
    return 5; }
    case 't':
    /* prefix t */{static const char tmp[4] = {'r','u','n','c'};
    if(std::memcmp(uptr+1, tmp, 4) == 0) return (cTrunc<<16) | 0x80000005U;
    return 5; }
    default: return 5; }

            default: break;
        }
        return nameLength;
