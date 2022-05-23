#pragma once

#if __cplusplus >= 2020002L
    #define CPP20_LIKELY [[likely]]
    #define CPP20_UNLIKELY [[likely]]
#else
    #define CPP20_LIKELY
    #define CPP20_UNLIKELY
#endif

#include <iterator> // won’t be included actually

namespace detail {

    template <class It>
    using ChType = typename std::iterator_traits<It>::value_type;

    template <class Ch>
    class UtfTraits;

    template <size_t Len>
    class LenTraits;

    template <class It>
    using ItUtfTraits = UtfTraits<ChType<It>>;

    ///// LenTraits ////////////////////////////////////////////////////////////

    template<>
    class LenTraits<1> {
    public:
        using Enc = Utf8;
    };

    template<>
    class LenTraits<2> {
    public:
        using Enc = Utf16;
    };

    template<>
    class LenTraits<4> {
    public:
        using Enc = Utf32;
    };

    template<>
    class LenTraits<8> {
    public:
        using Enc = Utf32;
    };

    ///// UtfTraits ////////////////////////////////////////////////////////////

    template<>
    class UtfTraits<char> {
    public:
        using Enc = Utf8;
    };

    #if __cplusplus >= 202002L
        template<>
        class UtfTraits<char8_t> {
        public:
            using Enc = enc::Utf8;
        };
    #endif

    template<>
    class UtfTraits<char16_t> {
    public:
        using Enc = Utf16;
    };

    template<>
    class UtfTraits<char32_t> {
    public:
        using Enc = Utf32;
    };

    template<>
    class UtfTraits<wchar_t>
    {
    public:
        using Enc = typename LenTraits<sizeof(wchar_t)>::Enc;
    };

    ///// ItEnc ////////////////////////////////////////////////////////////////

    ///
    ///  Iterator + encoding
    ///
    template <class It, class Enc>
    class ItEnc;

    template <class It>
    class ItEnc<It, Utf32>
    {
    public:
        static inline void put(It& it, char32_t cp)
            noexcept (noexcept(*it = cp) && noexcept (++it))
        {
            *it = cp;
            ++it;
        }

        template <class It2, class Enc2, class Mjh>
        static inline It2 copy(It p, It end, It2 dest, const Mjh& onMojibake);
    };

    /// @return [+] halt
    template <class Enc2, class It1, class It2, class Mjh>
    inline bool handleMojibake(It1 start, It1 bad, It2& dest, const Mjh& onMojibake)
    {
        auto result = onMojibake(start, bad);
        bool halt = result & handler::FG_HALT;
        result &= handler::MASK_CODE;
        if (result != handler::RET_SKIP)
            ItEnc<It2, Enc2>::put(dest, result);
        return halt;
    }

    template <class It> template <class It2, class Enc2, class Mjh>
    inline It2 ItEnc<It, Utf32>::copy(It p, It end, It2 dest, const Mjh& onMojibake)
    {
        for (; p != end; ++p) {
            char32_t c = *p;
            if (mojibake::isValid(c)) CPP20_LIKELY {
                ItEnc<It2, Enc2>::put(dest, c);
            } else CPP20_UNLIKELY {
                if (handleMojibake<Enc2>(p, p, dest, onMojibake))
                    break;
            }
        }
        return dest;
    }

    template <class It>
    class ItEnc<It, Utf16>
    {
    public:
        static void put(It& it, char32_t cp)
                noexcept (noexcept(*it = cp) && noexcept (++it));
        template <class It2, class Enc2, class Mjh>
        static inline It2 copy(It p, It end, It2 dest, const Mjh& onMojibake);
    };

    template <class It>
    void ItEnc<It, Utf16>::put(It& it, char32_t cp)
            noexcept (noexcept(*it = cp) && noexcept (++it))
    {
        if (cp < U16_2WORD_MIN) CPP20_LIKELY {   // 1 word
            *(it) = static_cast<wchar_t>(cp);
            ++it;
        } else if (cp <= U16_2WORD_MAX) CPP20_UNLIKELY { // 2 words
            cp -= U16_2WORD_MIN;
            const wchar_t lo10 = cp & 0x3FF;
            const wchar_t hi10 = cp >> 10;
            *it = static_cast<wchar_t>(0xD800 | hi10);
            ++it;
            *it = static_cast<wchar_t>(0xDC00 | lo10);
            ++it;
        }
    }

    template <class It> template <class It2, class Enc2, class Mjh>
    inline It2 ItEnc<It, Utf16>::copy(It p, It end, It2 dest, const Mjh& onMojibake)
    {
        for (; p != end;) {
            auto cpStart = p++;
            char16_t word1 = *cpStart;
            if (word1 < SURROGATE_HI_MIN) CPP20_LIKELY {
                if (word1 < SURROGATE_MIN) { // Low BMP char => OK
                    ItEnc<It2, Enc2>::put(dest, word1);
                } else {  // Leading surrogate
                    if (p == end) CPP20_UNLIKELY {
                        // Abrupt end
                        if (handleMojibake<Enc2>(cpStart, p, dest, onMojibake))
                            break;
                    } else {
                        char16_t word2 = *p;
                        if (word2 < SURROGATE_HI_MIN || word2 > SURROGATE_HI_MAX)
                        CPP20_UNLIKELY {
                            if (handleMojibake<Enc2>(cpStart, p, dest, onMojibake))
                                break;
                        } else CPP20_LIKELY {
                            ++p;
                            char32_t cp = (((word1 & 0x3FF) << 10) | (word2 & 0x3FF)) + 0x10000;
                            ItEnc<It2, Enc2>::put(dest, cp);
                        }
                    }
                }
            } else {
                if (word1 <= SURROGATE_MAX) CPP20_UNLIKELY { // Trailing surrogate
                    if (handleMojibake<Enc2>(cpStart, cpStart, dest, onMojibake))
                        break;
                } else { // High BMP char => OK
                    ItEnc<It2, Enc2>::put(dest, word1);
                }
            }   // big if
        }   // for
        return dest;
    }

    template <class It>
    class ItEnc<It, Utf8>
    {
    public:
        static void put(It& it, char32_t cp)
                noexcept (noexcept(*it = cp) && noexcept (++it));
        template <class It2, class Enc2, class Mjh>
        static inline It2 copy(It p, It end, It2 dest, const Mjh& onMojibake);
    };

    template <class It>
    void ItEnc<It, Utf8>::put(It& it, char32_t cp)
            noexcept (noexcept(*it = cp) && noexcept (++it))
    {
        if (cp <= U8_2BYTE_MAX) CPP20_LIKELY {  // 1 or 2 bytes, the most frequent case
            if (cp <= U8_1BYTE_MAX) {  // 1 byte
                *it = cp;  ++it;
            } else { // 2 bytes
                *it     = (cp >> 6)   | 0xC0;
                *(++it) = (cp & 0x3F) | 0x80;  ++it;
            }
        } else CPP20_UNLIKELY {  // 3 or 4 bytes
            if (cp <= U8_3BYTE_MAX) {  // 3 bytes
                *it     =  (cp >> 12)        | 0xE0;
                *(++it) = ((cp >> 6) & 0x3F) | 0x80;
                *(++it) =  (cp       & 0x3F) | 0x80;  ++it;
            } else {    // 4 bytes
                *it     = ((cp >> 18) & 0x07) | 0xF0;
                *(++it) = ((cp >> 12) & 0x3F) | 0x80;
                *(++it) = ((cp >> 6)  & 0x3F) | 0x80;
                *(++it) =  (cp        & 0x3F) | 0x80; ++it;
            }
        }
    }

    template <class It> template <class It2, class Enc2, class Mjh>
    inline It2 ItEnc<It, Utf8>::copy(It p, It end, It2 dest, const Mjh& onMojibake)
    {
    }

}   // namespace detail
