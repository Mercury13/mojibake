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

    class Utf8  { public: using Ch = char; };
    class Utf16 { public: using Ch = char16_t; };
    class Utf32 { public: using Ch = char32_t; };

    ///
    /// @return [+] codepoint is good: allocated, unallocated, reserved,
    ///               private-use, non-character
    ///         [-] it is bad: surrogate, too high
    ///
    inline bool isValid(char32_t cp) noexcept
    {
        return (cp < SURROGATE_MIN
                || (cp > SURROGATE_MAX && cp <= UNICODE_MAX));
    }

    namespace handler {

        constexpr char32_t RET_SKIP = 0xFFFFFF;
        constexpr char32_t FG_HALT = 0x80000000;
        constexpr char32_t MASK_CODE = 0xFFFFFF;
        constexpr char32_t RET_HALT = RET_SKIP | FG_HALT;

        ///
        /// Mojibake handler MUST:
        /// • accept two params: start of bad CP, and place where it became bad
        /// • return code point SPECIAL_SKIP, SPECIAL_HALT or valid code point
        ///
        /// Mojibake support varies between serialization types, and…
        /// • [U32] bad codepoint is just replaced with mojibake
        /// • [U8/16] implementation-dependent, but…
        ///   • [U8] wrong but well-serialized codepoint = EXACTLY ONE mojibake
        ///   • [U8] long code sequence = EXACTLY ONE mojibake
        ///   • incomplete CP between two good = EXACTLY ONE mojibake
        ///
        template <class It>
        class Skip final {
        public:
            inline char32_t operator () (
                    [[maybe_unused]] It cpStart,
                    [[maybe_unused]] It badPlace) const noexcept { return RET_SKIP; }
        };  // class Skip

        template <class It>
        class Moji final {
        public:
            inline char32_t operator () (
                    [[maybe_unused]] It cpStart,
                    [[maybe_unused]] It badPlace) const noexcept { return MOJIBAKE; }
        };  // class Skip

        template <class It>
        class MojiHalt final {
        public:
            inline char32_t operator () (
                    [[maybe_unused]] It cpStart,
                    [[maybe_unused]] It badPlace) const noexcept { return MOJIBAKE | FG_HALT; }
        };  // class Skip

    }   // namespace handler

    #include "internal/detail.hpp"

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


    ///
    /// Copies data to another,
    /// REACTING APPROPRIATELY TO MOJIBAKE
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class Mjh,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copy(It1 beg, It1 end, It2 dest, const Mjh& onMojibake = Mjh{})
    {
        return detail::ItEnc<It1, Enc1>::template copy<It2, Enc2, Mjh>(beg, end, dest, onMojibake);
    }

    template <class Enc1, class Enc2,  class Mjh, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copy(It1 beg, It1 end, It2 dest, const Mjh& onMojibake = Mjh{})
    {
        return copy<It1, It2, Enc1, Enc2, Mjh>(beg, end, dest, onMojibake);
    }

    template <class Enc2, class Mjh, class It1, class It2, class Enc1,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copy(It1 beg, It1 end, It2 dest, const Mjh& onMojibake = Mjh{})
    {
        return copy<It1, It2, Enc1, Enc2, Mjh>(beg, end, dest, onMojibake);
    }

    ///
    /// Copies data to another, SKIPPING MOJIBAKE
    /// So mo mojibake handler
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyS(It1 beg, It1 end, It2 dest)
    {
        using Sk = mojibake::handler::Skip<It1>;
        return detail::ItEnc<It1, Enc1>::template copy<It2, Enc2, Sk>(beg, end, dest, Sk{});
    }

    template <class Enc1, class Enc2, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyS(It1 beg, It1 end, It2 dest)
    {
        return copyS<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    template <class Enc2, class It1, class It2, class Enc1,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyS(It1 beg, It1 end, It2 dest)
    {
        return copyS<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Copies data to another, WRITING MOJIBAKE
    /// So mo mojibake handler
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyM(It1 beg, It1 end, It2 dest)
    {
        using Mo = mojibake::handler::Moji<It1>;
        return detail::ItEnc<It1, Enc1>::template copy<It2, Enc2, Mo>(beg, end, dest, Mo{});
    }

    template <class Enc1, class Enc2, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyM(It1 beg, It1 end, It2 dest)
    {
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    template <class Enc2, class It1, class It2, class Enc1,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyM(It1 beg, It1 end, It2 dest)
    {
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Copies data to another, WRITING MOJIBAKE AND HALTING
    /// So mo mojibake handler
    ///
    template <class It1, class It2,
              class Enc1 = typename detail::ItUtfTraits<It1>::Enc,
              class Enc2 = typename detail::ItUtfTraits<It2>::Enc,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyMH(It1 beg, It1 end, It2 dest)
    {
        using Mh = mojibake::handler::MojiHalt<It1>;
        return detail::ItEnc<It1, Enc1>::template copy<It2, Enc2, Mh>(beg, end, dest, Mh{});
    }

    template <class Enc1, class Enc2, class It1, class It2,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyMH(It1 beg, It1 end, It2 dest)
    {
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    template <class Enc2, class It1, class It2, class Enc1,
              class = std::void_t<typename std::iterator_traits<It1>::value_type>,
              class = std::void_t<typename std::iterator_traits<It2>::value_type>>
    inline It2 copyMH(It1 beg, It1 end, It2 dest)
    {
        return copyM<It1, It2, Enc1, Enc2>(beg, end, dest);
    }

    ///
    /// Generic conversion
    ///
    template <class To, class From, class Mjh,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc>
    To to(const From& from, const Mjh& onMojibake = Mjh{})
    {
        To r;
        std::back_insert_iterator it(r);
        using It1 = decltype(std::begin(from));
        using It2 = decltype(it);
        copy<It1, It2, Enc1, Enc2, Mjh>(std::begin(from), std::end(from), it, onMojibake);
        return r;
    }

    template <class To, class From,
              class Enc2 = typename detail::ContUtfTraits<To>::Enc,
              class Enc1 = typename detail::ContUtfTraits<From>::Enc>
    inline To toS(const From& from)
    {
        using It = decltype(std::begin(from));
        using Sk = mojibake::handler::Skip<It>;
        return to<To, From, Sk, Enc2, Enc1>(from);
    }

    ///
    /// Pseudo-iterator for mojibake::put that calls some functor instead
    ///
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
    class Utf8CallIterator : public CallIterator<Utf8, Func>
    {
    public:
        using CallIterator<Utf8, Func>::CallIterator;
    };

    template <class Func>
    Utf8CallIterator(const Func&) -> Utf8CallIterator<Func>;

    template <class Func>
    class Utf16CallIterator : public CallIterator<Utf16, Func>
    {
    public:
        using CallIterator<Utf8, Func>::CallIterator;
    };

    template <class Func>
    Utf16CallIterator(const Func&) -> Utf16CallIterator<Func>;

    template <class Func>
    class Utf32CallIterator : public CallIterator<Utf32, Func>
    {
    public:
        using CallIterator<Utf8, Func>::CallIterator;
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
