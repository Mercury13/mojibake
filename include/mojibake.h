#pragma once

///
///  MOJIBAKE
///    simple C++17 Unicode conversion library
///
///  LICENSE: MIT
///

#include <algorithm>
#include <iterator>

namespace mojibake {

    constexpr char32_t SURROGATE_MIN = 0xD800;
    constexpr char32_t SURROGATE_MAX = 0xDFFF;
    constexpr char32_t SURROGATE_LO_MIN = SURROGATE_MIN;
    constexpr char32_t SURROGATE_HI_MIN = 0xDC00;
    constexpr char32_t SURROGATE_LO_MAX = SURROGATE_HI_MIN - 1;
    constexpr char32_t SURROGATE_HI_MAX = SURROGATE_MAX;
    constexpr char32_t MOJIBAKE = 0xFFFD;
    constexpr char32_t UNICODE_MAX = 0x10FFFF;
    constexpr char32_t U8_1BYTE_MAX = 0x7F;
    constexpr char32_t U8_2BYTE_MIN = 0x80;
    constexpr char32_t U8_2BYTE_MAX = 0x7FF;
    constexpr char32_t U8_3BYTE_MIN = 0x800;
    constexpr char32_t U8_3BYTE_MAX = 0xFFFF;
    constexpr char32_t U8_4BYTE_MIN = 0x10000;
    constexpr char32_t U8_4BYTE_MAX = UNICODE_MAX;
    constexpr char32_t U16_1WORD_MAX = 0xFFFF;
    constexpr char32_t U16_2WORD_MIN = 0x10000;
    constexpr char32_t U16_2WORD_MAX = UNICODE_MAX;

    namespace enc {
        class Utf8  { public: using Ch = char; };
        class Utf16 { public: using Ch = char16_t; };
        class Utf32 { public: using Ch = char32_t; };
    }

    #include "internal/detail.hpp"

    ///
    /// @return [+] codepoint is good: allocated, unallocated, reserved,
    ///               private-use, non-character
    ///         [-] it is bad: surrogate, too high
    ///
    inline bool isGood(char32_t cp) noexcept
    {
        return (cp < SURROGATE_MIN
                || (cp > SURROGATE_MAX && cp <= UNICODE_MAX));
    }

    ///
    /// Puts code point to some iterator
    /// @tparam  It   iterator
    /// @tparam  Enc  enc::Utf8, enc::Utf16, etc::Utf32
    /// @param [in,out]  it   iterator
    /// @param [in]      cp   code point, SHOULD BE GOOD
    /// @warning  Bhv on bad cp is implementation-specific
    ///
    template <class It,
              class Enc = typename detail::ItUtfTraits<It>::Enc,
              class = std::void_t<typename std::iterator_traits<It>::value_type>>
    inline void put(It& it, char32_t cp)
        { detail::ItEnc<It, Enc>::put(it, cp); }

    template <class Enc,
              class It,
              class = std::void_t<typename std::iterator_traits<It>::value_type>>
    inline void put(It& it, char32_t cp)
        { detail::ItEnc<It, Enc>::put(it, cp); }

    template <class Enc, class Func>
    class CallIterator
    {
    public:
        using value_type = typename Enc::Ch;
        CallIterator(const Func& aFunc) noexcept : func(aFunc) {}
        const CallIterator& operator ++() const noexcept { return *this; }
        const CallIterator& operator * () const noexcept { return *this; }
        const CallIterator* operator ->() const noexcept { return this; }
        void operator = (value_type c) const { func(c); }
    private:
        const Func& func;
    };

    template <class Func>
    class Utf8CallIterator : public CallIterator<enc::Utf8, Func>
    {
    public:
        using CallIterator<enc::Utf8, Func>::CallIterator;
    };

    template <class Func>
    Utf8CallIterator(const Func&) -> Utf8CallIterator<Func>;

    template <class Func>
    class Utf16CallIterator : public CallIterator<enc::Utf16, Func>
    {
    public:
        using CallIterator<enc::Utf8, Func>::CallIterator;
    };

    template <class Func>
    Utf16CallIterator(const Func&) -> Utf16CallIterator<Func>;

    template <class Func>
    class Utf32CallIterator : public CallIterator<enc::Utf32, Func>
    {
    public:
        using CallIterator<enc::Utf8, Func>::CallIterator;
    };

    template <class Func>
    Utf32CallIterator(const Func&) -> Utf32CallIterator<Func>;

//    template <class To, class From>
//    To to(const From& from);

}   // namespace mojibake


namespace std {

    template <class Enc, class Func>
    class iterator_traits<mojibake::CallIterator<Enc, Func>>
    {
    public:
        using difference_type = int;
        using CI = typename mojibake::CallIterator<Enc, Func>;
        using value_type = typename CI::value_type;
        using pointer = CI*;
        using reference = CI&;
        using iterator_category = forward_iterator_tag;
    };

    template <class Func>
    class iterator_traits<mojibake::Utf8CallIterator<Func>>
    {
    public:
        using difference_type = int;
        using CI = typename mojibake::Utf8CallIterator<Func>;
        using value_type = typename CI::value_type;
        using pointer = CI*;
        using reference = CI&;
        using iterator_category = forward_iterator_tag;
    };

    template <class Func>
    class iterator_traits<mojibake::Utf16CallIterator<Func>>
    {
    public:
        using difference_type = int;
        using CI = typename mojibake::Utf16CallIterator<Func>;
        using value_type = typename CI::value_type;
        using pointer = CI*;
        using reference = CI&;
        using iterator_category = forward_iterator_tag;
    };

    template <class Func>
    class iterator_traits<mojibake::Utf32CallIterator<Func>>
    {
    public:
        using difference_type = int;
        using CI = typename mojibake::Utf32CallIterator<Func>;
        using value_type = typename CI::value_type;
        using pointer = CI*;
        using reference = CI&;
        using iterator_category = forward_iterator_tag;
    };

}   // namespace std
