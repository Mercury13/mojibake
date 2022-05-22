#pragma once

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
    class LenTraits<1>
    {
    public:
        using Enc = enc::Utf8;
    };

    template<>
    class LenTraits<2>
    {
    public:
        using Enc = enc::Utf16;
    };

    template<>
    class LenTraits<4>
    {
    public:
        using Enc = enc::Utf32;
    };

    template<>
    class LenTraits<8>
    {
    public:
        using Enc = enc::Utf32;
    };

    ///// UtfTraits ////////////////////////////////////////////////////////////

    template<>
    class UtfTraits<char>
    {
    public:
        using Enc = enc::Utf8;
    };

    #if __cplusplus >= 202002L
        template<>
        class UtfTraits<char8_t>
        {
            using Enc = enc::Utf8;
        };
    #endif

    template<>
    class UtfTraits<char16_t>
    {
    public:
        using Enc = enc::Utf16;
    };

    template<>
    class UtfTraits<char32_t>
    {
    public:
        using Enc = enc::Utf32;
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
    class ItEnc<It, enc::Utf32>
    {
    public:
        static inline void put(It& it, char32_t cp)
        {
            *it = cp;
            ++it;
        }
    };

    template <class It>
    class ItEnc<It, enc::Utf16>
    {
    public:
        static void put(It& it, char32_t cp);
    };

    template <class It>
    void ItEnc<It, enc::Utf16>::put(It& it, char32_t cp)
    {
        if (cp < U16_2WORD_MIN) {  // 1 word
            *(it) = static_cast<wchar_t>(cp);
            ++it;
        } else if (cp <= U16_2WORD_MAX) {    // 2 words
            cp -= U16_2WORD_MIN;
            // Hi word
            const wchar_t lo10 = cp & 0x3FF;
            const wchar_t hi10 = cp >> 10;
            *it = static_cast<wchar_t>(0xD800 | hi10);
            ++it;
            *it = static_cast<wchar_t>(0xDC00 | lo10);
            ++it;
        }
    }

}   // namespace detail
