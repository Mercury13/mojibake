// What we test
#include "mojibake.h"

// Google test
#include "gtest/gtest.h"

using namespace std::string_view_literals;


/////
/////  mojibake::isCpValid /////////////////////////////////////////////////////
/////


TEST (IsCpValid, Simple)
{
    EXPECT_TRUE (mojibake::isValid(0));
    EXPECT_TRUE (mojibake::isValid(1000));
    EXPECT_TRUE (mojibake::isValid(0xD7FF));
    EXPECT_FALSE(mojibake::isValid(0xD800));
    EXPECT_FALSE(mojibake::isValid(0xDFFF));
    EXPECT_TRUE (mojibake::isValid(0xE000));
    EXPECT_TRUE (mojibake::isValid(0xFFFF));
    EXPECT_TRUE (mojibake::isValid(0x10000));
    EXPECT_TRUE (mojibake::isValid(0xFFFFF));
    EXPECT_TRUE (mojibake::isValid(0x100000));
    EXPECT_TRUE (mojibake::isValid(0x10FFFF));
    EXPECT_FALSE(mojibake::isValid(0x110000));
    EXPECT_FALSE(mojibake::isValid(0x1E0000));
}


/////
/////  mojibake::put ///////////////////////////////////////////////////////////
/////


///
///  Simple runnability of UTF-32 mojibake::put
///
TEST (Put, Simple32)
{
    char32_t c[30];
    char32_t* start = c;
    char32_t* it = start;
    mojibake::put(it, 'a');
    mojibake::put(it, 'b');
    mojibake::put(it, 'c');
    mojibake::put(it, 0x1234);
    mojibake::put(it, 0x12345);

    std::basic_string_view sv(start, it - start);
    EXPECT_EQ(U"abc\u1234\U00012345", sv);
}


///
///  mojibake::put works in a wrong encoding
///
TEST (Put, WrongEnc)
{
    char16_t c[30];
    char16_t* start = c;
    char16_t* it = start;
    using It = decltype(it);
    using Enc = mojibake::Utf32;
    mojibake::put<It, Enc>(it, 'a');
    mojibake::put<It, Enc>(it, 'b');
    mojibake::put<It, Enc>(it, 'c');
    mojibake::put<It, Enc>(it, 0x1234);
    mojibake::put<It, Enc>(it, 0x12345);

    std::basic_string_view sv(start, it - start);
    EXPECT_EQ(u"abc\u1234\u2345", sv);
}


///
///  Same, but to char
///
TEST (Put, WrongEnc2)
{
    char c[30];
    char* start = c;
    char* it = start;
    using It = decltype(it);
    using Enc = mojibake::Utf32;
    mojibake::put<It, Enc>(it, 'a');
    mojibake::put<It, Enc>(it, 'b');
    mojibake::put<It, Enc>(it, 'c');
    mojibake::put<It, Enc>(it, 0x1234);
    mojibake::put<It, Enc>(it, 0x12345);

    std::basic_string_view sv(start, it - start);
    EXPECT_EQ("abc\x34\x45", sv);
}


///
///  Version of mojibake::put with Enc ahead
///
TEST (Put, ReverseEnc)
{
    char c[30];
    char* start = c;
    char* it = start;
    using Enc = mojibake::Utf32;
    mojibake::put<Enc>(it, 'a');
    mojibake::put<Enc>(it, 'b');
    mojibake::put<Enc>(it, 'c');
    mojibake::put<Enc>(it, 0x1234);
    mojibake::put<Enc>(it, 0x12345);

    std::basic_string_view sv(start, it - start);
    EXPECT_EQ("abc\x34\x45", sv);
}


///
///  Same, both Enc and It are specified
///
TEST (Put, ReverseEnc2)
{
    char c[30];
    char* start = c;
    char* it = start;
    using It = decltype(it);
    using Enc = mojibake::Utf32;
    mojibake::put<Enc, It>(it, 'a');
    mojibake::put<Enc, It>(it, 'b');
    mojibake::put<Enc, It>(it, 'c');
    mojibake::put<Enc, It>(it, 0x1234);
    mojibake::put<Enc, It>(it, 0x12345);

    std::basic_string_view sv(start, it - start);
    EXPECT_EQ("abc\x34\x45", sv);
}


///
///  Simple runnability of UTF-16 mojibake::put
///
TEST (Put, Simple16)
{
    char16_t c[30];
    char16_t* start = c;
    char16_t* it = start;
    mojibake::put(it, 'a');
    mojibake::put(it, 'b');
    mojibake::put(it, 'c');
    mojibake::put(it, 0x1234);
    mojibake::put(it, 0x12345);

    std::basic_string_view sv(start, it - start);
    EXPECT_EQ(6u, sv.length());
    EXPECT_EQ(u"abc\u1234\U00012345", sv);
}


///
///  Simple runnability of wchar_t mojibake::put
///
TEST (Put, Wchar)
{
    wchar_t c[30];
    wchar_t* start = c;
    wchar_t* it = start;
    mojibake::put(it, 'a');
    mojibake::put(it, 'b');
    mojibake::put(it, 'c');
    mojibake::put(it, 0x1234);
    mojibake::put(it, 0x12345);

    std::basic_string_view sv(start, it - start);
    if constexpr(sizeof(wchar_t) >= 4) {
        EXPECT_EQ(5u, sv.length());
    } else if constexpr(sizeof (wchar_t) == 2) {
        EXPECT_EQ(6u, sv.length());
    } else {
        // Unknown sizeof wchar
        EXPECT_EQ(1, 2);
    }
    EXPECT_EQ(L"abc\u1234\U00012345", sv);
}


///
///  Simple runnability of UTF-8 mojibake::put
///
TEST (Put, Simple8)
{
    char c[30];
    char* start = c;
    char* it = start;
    mojibake::put(it, 'a');
    mojibake::put(it, 'b');
    mojibake::put(it, 'c');
    mojibake::put(it, 0x40B);       // 2-byte cp
    mojibake::put(it, 0x1234);      // 3-byte cp
    mojibake::put(it, 0x12345);     // 4-byte cp

    std::basic_string_view sv(start, it - start);
    EXPECT_EQ(12u, sv.length());
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", sv);
}


///
///  Utf8CallIterator
///
TEST (Put, CallIterator)
{
    std::string s;

    auto func = [&s](auto c) { s.push_back(c); };
    mojibake::Utf8CallIterator it(func);

    mojibake::put(it, 'a');
    mojibake::put(it, 'b');
    mojibake::put(it, 'c');
    mojibake::put(it, 0x40B);       // 2-byte cp
    mojibake::put(it, 0x1234);      // 3-byte cp
    mojibake::put(it, 0x12345);     // 4-byte cp

    EXPECT_EQ(12u, s.length());
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", s);
}


/////
/////  mojibake::copy //////////////////////////////////////////////////////////
/////

///
/// Normal UTF-32
///
TEST (Copy, Utf32Normal)
{
    std::u32string_view s = U"abc\u040B\u1234\U00012345";
    char buf[30];
    using NoM = mojibake::handler::Skip<std::u32string_view::const_iterator>;
    auto end = mojibake::copy(s.begin(), s.end(), buf, NoM());
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Bad UTF-32
///
TEST (Copy, Utf32Bad)
{
    std::u32string s = U"abc";
    s.push_back(0xDD00);    // Surrogate
    s.push_back(0x040B);
    s.push_back(0x110000);  // Too high
    s.push_back(0x1234);
    s.push_back(0x12345);

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    using NoM = mojibake::handler::Skip<std::u32string::const_iterator>;
    auto end = mojibake::copy(s.begin(), s.end(), buf, NoM());
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Normal UTF-16
///
TEST (Copy, Utf16Normal)
{
    std::u16string_view s = u"abc\u040B\u1234\U00012345";
    char buf[30];
    using NoM = mojibake::handler::Skip<std::u16string_view::const_iterator>;
    auto end = mojibake::copy(s.begin(), s.end(), buf, NoM());
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Bad UTF-16
///
TEST (Copy, Utf16Bad)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    using NoM = mojibake::handler::Skip<std::u16string::const_iterator>;
    auto end = mojibake::copy(s.begin(), s.end(), buf, NoM());
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abcd", r);
}

///
/// Normal UTF-8
///
TEST (Copy, Utf8Normal)
{
    std::string_view s = "abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85";
    char32_t buf[30];
    using NoM = mojibake::handler::Skip<std::string_view::const_iterator>;
    auto end = mojibake::copy(s.begin(), s.end(), buf, NoM());
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"abc\u040B\u1234\U00012345", r);
}


/////
/////  mojibake::copyS /////////////////////////////////////////////////////////
/////

///
/// Normal UTF-32
///
TEST (CopyS, Utf32Normal)
{
    std::u32string_view s = U"abc\u040B\u1234\U00012345";
    char buf[30];
    auto end = mojibake::copyS(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Bad UTF-32
///
TEST (CopyS, Utf32Bad)
{
    std::u32string s = U"abc";
    s.push_back(0xDD00);    // Surrogate
    s.push_back(0x040B);
    s.push_back(0x110000);  // Too high
    s.push_back(0x1234);
    s.push_back(0x12345);

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyS(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Bad UTF-16
///
TEST (CopyS, Utf16Bad)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyS(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abcd", r);
}


///
/// Good UTF-8
///
TEST (CopyS, Utf8Good)
{
    std::string_view s = "abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85";
    char32_t buf[30];
    auto end = mojibake::copyS(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"abc\u040B\u1234\U00012345", r);
}


///
/// Bad UTF-8
///
TEST (CopyS, Utf8Bad)
{
    std::string_view s = "abc" "\xD0\x8B" "\xE1\x88" "\xF0\x92\x8D\x85";
    char32_t buf[30];
    auto end = mojibake::copyS(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"abc\u040B\U00012345", r);
}


///
/// Bug was in copyMH, but test it
///
TEST (CopyS, BugOverload1)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyS<mojibake::Utf16, mojibake::Utf8>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abcd", r);
}


///
/// Same stuff, other overload
///
TEST (CopyS, BugOverload2)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char16_t buf[30];
    auto end = mojibake::copyS<mojibake::Utf8>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(u"abcd", r);
}


///
/// CopyS — bhv in same encoding
///
TEST (CopyS, SameEnc)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char16_t buf[30];

    static_assert(std::is_same_v<
            mojibake::detail::ContUtfTraits<decltype(s)>::Enc,
            mojibake::detail::ItUtfTraits<decltype(std::begin(buf))>::Enc>,
            "Should be the same encoding");

    auto end = mojibake::copyS(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(u"abcd", r);
}


/////
/////  mojibake::copyM /////////////////////////////////////////////////////////
/////

///
/// Normal UTF-32
///
TEST (CopyM, Utf32Normal)
{
    std::u32string_view s = U"abc\u040B\u1234\U00012345";
    char buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}

// UTF-8 mojibake
#define U8_MOJ "\xEF\xBF\xBD"

// Same, in wider encoding (when we manually override)
#define U8_WMOJ "\u00EF\u00BF\u00BD"

// UTF-16 mojibake
#define U16_MOJ "\uFFFD"

///
/// Bad UTF-32
///
TEST (CopyM, Utf32Bad)
{
    std::u32string s = U"abc";
    s.push_back(0xDD00);    // Surrogate
    s.push_back(0x040B);
    s.push_back(0x110000);  // Too high
    s.push_back(0x1234);
    s.push_back(0x12345);

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" U8_MOJ "\xD0\x8B" U8_MOJ "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Bad UTF-16
///
TEST (CopyM, Utf16Bad)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("a" U8_MOJ "b" U8_MOJ "c" U8_MOJ U8_MOJ "d", r);
}


///
/// Bad UTF-8: unfinished CP → mojibake
///
TEST (CopyM, Utf8Bad)
{
    std::string_view s = "ab" "\xD0\x8B" "\xE1\x88" "\xF0\x92\x8D\x85";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\u040B\uFFFD\U00012345", r);
}


///
/// Bad UTF-8: multiple 80’s → ONE mojibake
///
TEST (CopyM, Utf8Multiple80)
{
    std::string_view s = "ab" "\x80\x81\x82\x83\x84\x85" "\xF0\x92\x8D\x85";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD\U00012345", r);
}


///
/// Bad UTF-8: onr 80 → ONE mojibake
///
TEST (CopyM, Utf8One80)
{
    std::string_view s = "ab" "\x81" "\xF0\x92\x8D\x85";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD\U00012345", r);
}


///
/// Bad UTF-8: Fx + multiple 80’s → ONE mojibake
///
TEST (CopyM, Utf8_F0)
{
    std::string_view s = "ab" "\xFE\x80\x81\x82\x83\x84\x85" "\xF0\x92\x8D\x85";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD\U00012345", r);
}


///
/// Bug was in copyMH, but test it
///
TEST (CopyM, BugOverload1)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyM<mojibake::Utf16, mojibake::Utf8>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    // WARNING: library behaves just this way, bhv may change
    //   (I mean two mojibake characters at the end)
    EXPECT_EQ("a" U8_MOJ "b" U8_MOJ "c" U8_MOJ U8_MOJ "d", r);
}


///
/// Same stuff, other overload
///
TEST (CopyM, BugOverload2)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char16_t buf[30];
    auto end = mojibake::copyM<mojibake::Utf8>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    // WARNING: library behaves just this way, bhv may change
    //   (I mean two mojibake characters at the end)
    EXPECT_EQ(u"a" U8_WMOJ "b" U8_WMOJ "c" U8_WMOJ U8_WMOJ "d", r);
}


///
/// CopyM — bhv in same encoding
///
TEST (CopyM, SameEnc)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char16_t buf[30];

    static_assert(std::is_same_v<
            mojibake::detail::ContUtfTraits<decltype(s)>::Enc,
            mojibake::detail::ItUtfTraits<decltype(std::begin(buf))>::Enc>,
            "Should be the same encoding");

    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(u"a" U16_MOJ "b" U16_MOJ "c" U16_MOJ U16_MOJ "d", r);
}


/////
///// Additional suite for testing UTF-8 correctness ///////////////////////////
///// in mojibake::copy                              ///////////////////////////
/////


///
/// 7F encoded in two bytes
///
TEST (CopyUtf8, Byte7F_Bad)
{
    std::string_view s = "ab" "\xC1\xBF";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD", r);
}


///
/// Byte 80 is good
///
TEST (CopyUtf8, Byte80_Good)
{
    std::string_view s = "ab" "\xC2\x80";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\u0080", r);
}


///
/// Byte 7FF, the greatest encodeable in two bytes
///
TEST (CopyUtf8, Byte7FF_Good)
{
    std::string_view s = "ab" "\xDF\xBF";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\u07FF", r);
}


///
/// Byte 7FF erroneously encoded in three bytes
///
TEST (CopyUtf8, Byte7FF_Bad)
{
    std::string_view s = "ab" "\xE0\x9F\xBF";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD", r);
}


///
/// Byte 800, the first encodeable with three bytes
///
TEST (CopyUtf8, Byte800_Good)
{
    std::string_view s = "ab" "\xE0\xA0\x80";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\u0800", r);
}


///
/// D7FF, the last before surrogate
///
TEST (CopyUtf8, ByteD7FF_Good)
{
    std::string_view s = "ab" "\xED\x9F\xBF";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uD7FF", r);
}


///
/// D800, the 1st surrogate
///
TEST (CopyUtf8, ByteD800_Bad)
{
    std::string_view s = "ab" "\xED\xA0\x80";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD", r);
}


///
/// DFFF, the last surrogate
///
TEST (CopyUtf8, ByteDFFF_Bad)
{
    std::string_view s = "ab" "\xED\xBF\xBF";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD", r);
}


///
/// E000, the 1st private-use
///
TEST (CopyUtf8, ByteE000_Good)
{
    std::string_view s = "ab" "\xEE\x80\x80";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uE000", r);
}


///
/// FFFF, the last 3-byte, non-char but still good
///
TEST (CopyUtf8, ByteFFFF_Good)
{
    std::string_view s = "ab" "\xEF\xBF\xBF";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFF", r);
}


///
/// 10000, the 1st SMP and 4-byte
///
TEST (CopyUtf8, Byte10000_Good)
{
    std::string_view s = "ab" "\xF0\x90\x80\x80";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\U00010000", r);
}


///
/// FFFF encoded with 4 bytes — too long
///
TEST (CopyUtf8, ByteFFFF_Bad)
{
    std::string_view s = "ab" "\xF0\x8F\xBF\xBF";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD", r);
}


///
/// 10FFFF, last in Unicode
///
TEST (CopyUtf8, Byte10FFFF_Good)
{
    std::string_view s = "ab" "\xF4\x8F\xBF\xBF";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\U0010FFFF", r);
}


///
/// 110000 bad
///
TEST (CopyUtf8, Byte110000_Bad)
{
    std::string_view s = "ab" "\xF4\x90\x80\x80";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD", r);
}


///
/// Incomplete + banned → BOTH are mojibake
///
TEST (CopyUtf8, IncompleteBanned1)
{
    std::string_view s = "ab" "\xE3\x81" "\xFD";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD\uFFFD", r);
}


///
/// Add a few 8x to banned
///
TEST (CopyUtf8, IncompleteBanned2)
{
    std::string_view s = "ab" "\xE3\x81" "\xFD\x80\x90\xA0\xB0\x81";
    char32_t buf[30];
    auto end = mojibake::copyM(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(U"ab\uFFFD\uFFFD", r);
}


/////
/////  mojibake::copyMH ////////////////////////////////////////////////////////
/////

///
/// Normal UTF-32
///
TEST (CopyMH, Utf32Normal)
{
    std::u32string_view s = U"abc\u040B\u1234\U00012345";
    char buf[30];
    auto end = mojibake::copyMH(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Bad UTF-32
///
TEST (CopyMH, Utf32Bad)
{
    std::u32string s = U"abc";
    s.push_back(0x040B);
    s.push_back(0x110000);  // Too high
    s.push_back(0x1234);
    s.push_back(0x12345);

    EXPECT_EQ(7u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyMH(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" U8_MOJ, r);
}


///
/// Bad UTF-16
///
TEST (CopyMH, Utf16Bad)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyMH(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("a" U8_MOJ, r);
}


///
/// CopyMH — bhv in same encoding
///
TEST (CopyMH, SameEnc)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char16_t buf[30];

    static_assert(std::is_same_v<
            mojibake::detail::ContUtfTraits<decltype(s)>::Enc,
            mojibake::detail::ItUtfTraits<decltype(std::begin(buf))>::Enc>,
            "Should be the same encoding");

    auto end = mojibake::copyMH(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(u"a" U16_MOJ, r);
}


///
/// Programming using copy/paste proved bad :(
///
TEST (CopyMH, BugOverload1)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyMH<mojibake::Utf16, mojibake::Utf8>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("a" U8_MOJ, r);
}


///
/// Same stuff, other overload
///
TEST (CopyMH, BugOverload2)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char16_t buf[30];
    auto end = mojibake::copyMH<mojibake::Utf8>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(u"a" U8_WMOJ, r);
}


/////
/////  mojibake::copyQ /////////////////////////////////////////////////////////
/////

///
/// Normal UTF-32
///
TEST (CopyQ, Utf32Normal)
{
    std::u32string_view s = U"abc\u040B\u1234\U00012345";
    char buf[30];
    auto end = mojibake::copyQ(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Bad UTF-32
///
TEST (CopyQ, Utf32Bad)
{
    std::u32string s = U"abc";
    s.push_back(0x040B);
    s.push_back(0x110000);  // Too high
    s.push_back(0x1234);
    s.push_back(0x12345);

    EXPECT_EQ(7u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyQ(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("abc" "\xD0\x8B" U8_MOJ "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
}


///
/// Bad UTF-16
///
TEST (CopyQ, Utf16Bad)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyQ(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("a" U8_MOJ "b" U8_MOJ "c" U8_MOJ U8_MOJ "d", r);
}


///
/// CopyQ — bhv in same encoding
///
TEST (CopyQ, SameEnc)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char16_t buf[30];

    static_assert(std::is_same_v<
            mojibake::detail::ContUtfTraits<decltype(s)>::Enc,
            mojibake::detail::ItUtfTraits<decltype(std::begin(buf))>::Enc>,
            "Should be the same encoding");

    auto end = mojibake::copyQ(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(s, r);
}


///
/// Programming using copy/paste proved bad :(
///
TEST (CopyQ, BugOverload1)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyQ<mojibake::Utf16, mojibake::Utf8>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ("a" U8_MOJ "b" U8_MOJ "c" U8_MOJ U8_MOJ "d", r);
}


///
/// Same stuff, other overload
///
TEST (CopyQ, BugOverload2)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    char16_t buf[30];
    auto end = mojibake::copyQ<mojibake::Utf8>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    EXPECT_EQ(u"a" U8_WMOJ "b" U8_WMOJ "c" U8_WMOJ U8_WMOJ "d", r);
}


///
/// Same encoding, different format
/// Should call ordinary copy
///
TEST (CopyQ, Overload2_Funky)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD905);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0x0E14);    // Some Thai
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(9u, s.length());   // Should contain those chars

    char buf[30];
    auto end = mojibake::copyQ<mojibake::Utf16>(s.begin(), s.end(), buf);
    std::basic_string_view r (buf, end - buf);
    // As there’s 0 inside, use SV literal
    EXPECT_EQ("a" "\x05" "b" "\0" "c" "\x14" "\xAB\xCD" "d"sv, r);
}


/////
/////  mojibake::toS ///////////////////////////////////////////////////////////
/////


///
/// Bad UTF-16
///
TEST (toS, Utf16Bad)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    auto r = mojibake::toS<std::string>(s);
    EXPECT_EQ("abcd", r);
}


TEST (toS, Utf16ConstChar)
{
    const char* s = "abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85";
    auto r = mojibake::toS<std::u32string>(s);
    EXPECT_EQ(U"abc\u040B\u1234\U00012345", r);
}


/////
/////  mojibake::toM ///////////////////////////////////////////////////////////
/////

///
/// Bad UTF-16
///
TEST (toM, Utf16Bad)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    auto r = mojibake::toM<std::string>(s);
    EXPECT_EQ("a" U8_MOJ "b" U8_MOJ "c" U8_MOJ U8_MOJ "d", r);
}


///
/// const char* overload
///
TEST (toM, ConstCharOverload)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    auto r = mojibake::toM<std::string>(s.c_str());
    EXPECT_EQ("a" U8_MOJ "b" U8_MOJ "c" U8_MOJ U8_MOJ "d", r);
}


/////
/////  mojibake::toQ ///////////////////////////////////////////////////////////
/////

///
/// Bad UTF-16
///
TEST (toQ, Utf16Bad)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    auto r = mojibake::toQ<std::string>(s);
    EXPECT_EQ("a" U8_MOJ "b" U8_MOJ "c" U8_MOJ U8_MOJ "d", r);
}


///
/// const char* overload
///
TEST (toQ, ConstCharOverload)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    auto r = mojibake::toQ<std::string>(s.c_str());
    EXPECT_EQ("a" U8_MOJ "b" U8_MOJ "c" U8_MOJ U8_MOJ "d", r);
}


///
/// Same encoding, funky version
///
TEST (toQ, SameEnc)
{
    std::u16string s;
    s.push_back('a');
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('b');
    s.push_back(0xDE00);    // Lone high surrogate
    s.push_back('c');
    s.push_back(0xD9AB);    // Double low surrogate
    s.push_back(0xD9CD);
    s.push_back('d');

    EXPECT_EQ(8u, s.length());   // Should contain those chars

    auto r = mojibake::toQ<std::string, std::u16string, mojibake::Utf16>(s);
    EXPECT_EQ("a" "\0" "b" "\0" "c" "\xAB\xCD" "d"sv, r);
}


/////
/////  mojibake::isValid ///////////////////////////////////////////////////////
/////

///
/// Good UTF-32
///
TEST (IsValid, Utf32Good)
{
    std::u32string_view s = U"abc\u040B\u1234\U00012345";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// UTF-32, const char* version
///
TEST (IsValid, Utf32ConstChar)
{
    EXPECT_TRUE(mojibake::isValid(U"abc\u040B\u1234\U00012345"));
}


///
/// Bad UTF-32
///
TEST (IsValid, Utf32Bad)
{
    std::u32string s = U"abc";
    s.push_back(0x040B);
    s.push_back(0x110000);  // Too high
    s.push_back(0x1234);
    s.push_back(0x12345);

    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// Good UTF-16
///
TEST (IsValid, Utf16Good)
{
    std::u16string_view s = u"abc\u040B\u1234\U00012345";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// UTF-16, const char* version
///
TEST (IsValid, Utf16ConstChar)
{
    EXPECT_TRUE(mojibake::isValid(u"abc\u040B\u1234\U00012345"));
}


///
/// Bad UTF-16: code point abruptly ended
///
TEST (IsValid, Utf16Bad_AbruptEnd)
{
    std::u16string s = u"abc\u040B";
    s.push_back(0xD900);    // Lone low surrogate
    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// Bad UTF-16: starting cp w/o ending
///
TEST (IsValid, Utf16Bad_StartingCp)
{
    std::u16string s = u"abc\u040B";
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('1');
    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// Bad UTF-16: starting cp w/o ending
///
TEST (IsValid, Utf16Bad_DoubleStartingCp)
{
    std::u16string s = u"abc\u040B";
    s.push_back(0xD900);    // Low surrogate
    s.push_back(0xDA00);    // Another low surrogate
    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// This UTF-16 is good :)
///
TEST (IsValid, Utf16Good_NormalCp)
{
    std::u16string s = u"abc\u040B";
    s.push_back(0xD900);    // Low surrogate
    s.push_back(0xDD00);    // High surrogate
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// Good UTF-8
///
TEST (IsValid, Utf8Good)
{
    std::string_view s = "abc" "\xD0\x8B" "\xE1\x88\xB4" "\xF0\x92\x8D\x85";
    EXPECT_TRUE(mojibake::isValid(s));
}


/////
/////  mojibake::isValid     ///////////////////////////////////////////////////
/////  Difficulties of UTF-8 ///////////////////////////////////////////////////
/////


///
/// 7F encoded in two bytes
///
TEST (IsValidU8, Byte7F_Bad)
{
    std::string_view s = "ab" "\xC1\xBF";
    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// Byte 80 is good
///
TEST (IsValidU8, Byte80_Good)
{
    std::string_view s = "ab" "\xC2\x80";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// Byte 7FF, the greatest encodeable in two bytes
///
TEST (IsValidU8, Byte7FF_Good)
{
    std::string_view s = "ab" "\xDF\xBF";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// Byte 7FF erroneously encoded in three bytes
///
TEST (IsValidU8, Byte7FF_Bad)
{
    std::string_view s = "ab" "\xE0\x9F\xBF";
    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// Byte 800, the first encodeable with three bytes
///
TEST (IsValidU8, Byte800_Good)
{
    std::string_view s = "ab" "\xE0\xA0\x80";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// D7FF, the last before surrogate
///
TEST (IsValidU8, ByteD7FF_Good)
{
    std::string_view s = "ab" "\xED\x9F\xBF";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// D800, the 1st surrogate
///
TEST (IsValidU8, ByteD800_Bad)
{
    std::string_view s = "ab" "\xED\xA0\x80";
    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// DFFF, the last surrogate
///
TEST (IsValidU8, ByteDFFF_Bad)
{
    std::string_view s = "ab" "\xED\xBF\xBF";
    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// E000, the 1st private-use
///
TEST (IsValidU8, ByteE000_Good)
{
    std::string_view s = "ab" "\xEE\x80\x80";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// FFFF, the last 3-byte, non-char but still good
///
TEST (IsValidU8, ByteFFFF_Good)
{
    std::string_view s = "ab" "\xEF\xBF\xBF";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// 10000, the 1st SMP and 4-byte
///
TEST (IsValidU8, Byte10000_Good)
{
    std::string_view s = "ab" "\xF0\x90\x80\x80";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// FFFF encoded with 4 bytes — too long
///
TEST (IsValidU8, ByteFFFF_Bad)
{
    std::string_view s = "ab" "\xF0\x8F\xBF\xBF";
    EXPECT_FALSE(mojibake::isValid(s));
}


///
/// 10FFFF, last in Unicode
///
TEST (IsValidU8, Byte10FFFF_Good)
{
    std::string_view s = "ab" "\xF4\x8F\xBF\xBF";
    EXPECT_TRUE(mojibake::isValid(s));
}


///
/// 110000 bad
///
TEST (IsValidU8, Byte110000_Bad)
{
    std::string_view s = "ab" "\xF4\x90\x80\x80";
    EXPECT_FALSE(mojibake::isValid(s));
}


/////
/////  fallbackCount1 //////////////////////////////////////////////////////////
/////
/////

///
/// As we use MinGW here, fallbackCount1 stays untested.
/// Test it somehow
///
TEST (FallbackCount1, xxx)
{
    EXPECT_EQ(0, mojibake::detail::fallbackCount1(0b0000'0000));
    EXPECT_EQ(0, mojibake::detail::fallbackCount1(0b0111'1111));
    EXPECT_EQ(1, mojibake::detail::fallbackCount1(0b1000'0000));
    EXPECT_EQ(1, mojibake::detail::fallbackCount1(0b1011'1111));
    EXPECT_EQ(2, mojibake::detail::fallbackCount1(0b1100'0000));
    EXPECT_EQ(2, mojibake::detail::fallbackCount1(0b1101'1111));
    EXPECT_EQ(3, mojibake::detail::fallbackCount1(0b1110'0000));
    EXPECT_EQ(3, mojibake::detail::fallbackCount1(0b1110'1111));
    EXPECT_EQ(4, mojibake::detail::fallbackCount1(0b1111'0000));
    EXPECT_EQ(4, mojibake::detail::fallbackCount1(0b1111'0111));
    EXPECT_EQ(5, mojibake::detail::fallbackCount1(0b1111'1000));
    EXPECT_EQ(5, mojibake::detail::fallbackCount1(0b1111'1011));
    EXPECT_EQ(6, mojibake::detail::fallbackCount1(0b1111'1100));
    EXPECT_EQ(6, mojibake::detail::fallbackCount1(0b1111'1101));
    EXPECT_EQ(7, mojibake::detail::fallbackCount1(0b1111'1110));
    EXPECT_EQ(8, mojibake::detail::fallbackCount1(0b1111'1111));
}


/////
/////  Error handling //////////////////////////////////////////////////////////
/////

constexpr char32_t U32_SMIL= 0x263A;  // some BMP smilie
#define U8_SMIL "\xE2\x98\xBA"

template <class Cont>
class MyHandler
{
public:
    using It = typename Cont::const_iterator;
    MyHandler(Cont& x) : start(x.begin()), firstPlace(x.end()) {}
    mutable int nEvents = 0;
    It start;
    mutable It firstPlace;
    mutable mojibake::Event firstEvent = mojibake::Event::END;

    char32_t operator()(It place, mojibake::Event event) const
    {
        if (++nEvents == 1) {
            firstPlace = place;
            firstEvent = event;
        }
        return U32_SMIL;
    }

    size_t pos() const { return firstPlace - start; }
};


///
/// UTF-32: good string, testing mock object
///
TEST (Error, U32Good)
{
    std::u32string s = U"abc\U0001E123\uA9A3";

    MyHandler h(s);
    auto r = mojibake::to<std::string>(s, h);
    EXPECT_EQ("abc" "\xF0\x9E\x84\xA3" "\xEA\xA6\xA3", r);
    EXPECT_EQ(0, h.nEvents);
    EXPECT_EQ(mojibake::Event::END, h.firstEvent);
    EXPECT_EQ(5u, h.pos());   // a,b,c,40B good
}


///
/// UTF-32: code too high
/// Bhv fixed, CODE at bad CP
///
/// We also test mock object’s ability to insert custom codepoints for mojibake
///
TEST (Error, U32TooHigh)
{
    std::u32string s = U"abc";
    s.push_back(0x040B);
    s.push_back(0x110000);  // Too high
    s.push_back(0x1234);
    s.push_back(0x12345);

    MyHandler h(s);
    auto r = mojibake::to<std::string>(s, h);
    EXPECT_EQ("abc" "\xD0\x8B" U8_SMIL "\xE1\x88\xB4" "\xF0\x92\x8D\x85", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::CODE, h.firstEvent);
    EXPECT_EQ(4u, h.pos());   // a,b,c,40B good
}


///
/// UTF-32: code is UTF-16 surrogate
/// Bhv fixed, CODE at bad CP
///
TEST (Error, U32Surrogate)
{
    std::u32string s = U"abc";
    s.push_back(0x040B);
    s.push_back(0x1234);
    s.push_back(0xDDDD);  // Surrogate
    s.push_back(0x12345);

    MyHandler h(s);
    auto r = mojibake::to<std::string>(s, h);
    EXPECT_EQ("abc" "\xD0\x8B" "\xE1\x88\xB4" U8_SMIL "\xF0\x92\x8D\x85", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::CODE, h.firstEvent);
    EXPECT_EQ(5u, h.pos());
}


///
/// UTF-16: abrupt end
/// Bhv fixed, END at CP’s start
///
TEST (Error, U16AbruptEnd)
{
    std::u16string s = u"x\u040B";
    s.push_back(0xD900);    // Lone low surrogate
    MyHandler h(s);
    auto r = mojibake::to<std::string>(s, h);
    EXPECT_EQ("x" "\xD0\x8B" U8_SMIL, r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::END, h.firstEvent);
    EXPECT_EQ(2u, h.pos());
}


///
/// UTF-16: interrupted codepoint
/// Bhv fixed, BYTE_NEXT where high surrogate should have been
///
TEST (Error, U16InterruptedCp1)
{
    std::u16string s = u"q\U00016A85";  // Some surrogate from Tangsa
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back('a');
    MyHandler h(s);
    auto r = mojibake::to<std::string>(s, h);
    EXPECT_EQ("q" "\xF0\x96\xAA\x85" U8_SMIL "a", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_NEXT, h.firstEvent);
    EXPECT_EQ(4u, h.pos()); // q, 2×Tangsa, low sur OK, and #4 is bad
}


///
/// UTF-16: interrupted codepoint
/// Bhv fixed, BYTE_NEXT where high surrogate should have been
///
TEST (Error, U16InterruptedCp2)
{
    std::u16string s = u"t\u040B";
    s.push_back(0xD900);    // Lone low surrogate
    s.push_back(0xD838);    // Some surrogate from Toto
        s.push_back(0xDE9B);
    MyHandler h(s);
    auto r = mojibake::to<std::string>(s, h);
    EXPECT_EQ("t" "\xD0\x8B" U8_SMIL "\xF0\x9E\x8A\x9B", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_NEXT, h.firstEvent);
    EXPECT_EQ(3u, h.pos());  // t, Cyr, low sur OK, and #3 is bad
}


///
/// UTF-16: high surrogate w/o prior low
/// Bhv fixed, BYTE_START at that surrogate
///
TEST (Error, U16HighSurrogate)
{
    std::u16string s = u"q\U00016A85";  // Some surrogate from Tangsa
    s.push_back(0xDF34);    // High surrogate
    s.push_back('a');
    MyHandler h(s);
    auto r = mojibake::to<std::string>(s, h);
    EXPECT_EQ("q" "\xF0\x96\xAA\x85" U8_SMIL "a", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_START, h.firstEvent);
    EXPECT_EQ(3u, h.pos()); // q, 2×Tangsa OK, and #4 (high sur) is bad
}


///
/// UTF-8: abrupt end 1/2
/// Bhv fixed, END at start of CP
///
TEST (Error, Utf8AbruptEnd12)
{
    std::string s = "ab" "\xE1\x9D\x8C" "\xD7";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"ab" "\u174C\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::END, h.firstEvent);
    EXPECT_EQ(5u, h.pos()); // ab, 3×Buhid OK, and #5 (unfinished Hebrew) is bad
}


///
/// UTF-8: unfinished 1/2
/// Bhv fixed, BYTE_NEXT at bad byte
///
TEST (Error, Utf8Unfinished12)
{
    std::string s = "ab" "\xE1\x9D\x8C" "\xD7" "c";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"ab" "\u174C\u263A" "c", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_NEXT, h.firstEvent);
    EXPECT_EQ(6u, h.pos()); // ab, 3×Buhid, unfinished Hebrew OK, c is bad
}


///
/// UTF-8: long code of length 2
///   C1 BF = 7F encoded in two bytes
/// Bhv fixed, CODE at start of CP
///
TEST (Error, Utf8LongCode2)
{
    std::string s = "ab" "\xD1\xA6" "\xC1\xBF";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"ab" "\u0466\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::CODE, h.firstEvent);
    EXPECT_EQ(4u, h.pos()); // ab, 2×Cyr OK, and #4 (7F in two bytes) is bad
}


///
/// UTF-8: abrupt end 1/3
/// Bhv fixed, END at start of CP
///
TEST (Error, Utf8AbruptEnd13)
{
    std::string s = "abcde" "\xE0\xAC\x8A" "\xE1";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"abcde" "\u0B0A\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::END, h.firstEvent);
    EXPECT_EQ(8u, h.pos()); // abcde, 3×Oriya OK, and #8 (unfinished Ethiopic) is bad
}


///
/// UTF-8: abrupt end 2/3
/// Bhv fixed, END at start of CP
///
TEST (Error, Utf8AbruptEnd23)
{
    std::string s = "abcde" "\xE0\xAC\x8A" "\xE1\x8A";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"abcde" "\u0B0A\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::END, h.firstEvent);
    EXPECT_EQ(8u, h.pos()); // abcde, 3×Oriya OK, and #8 (unfinished Ethiopic) is bad
}


///
/// UTF-8: long code of length 2
///   C1 < 80, need 1 byte
/// Bhv varies
///  • CODE at start of CP
///  • END at start of CP   <<<<<<<
///
TEST (Error, Utf8LongCodeAbruptEnd2)
{
    std::string s = "ab" "\xD1\xA6" "\xC1";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"ab" "\u0466\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::END, h.firstEvent);
    EXPECT_EQ(4u, h.pos()); // ab, 2×Cyr OK, and #4 is bad
}


///
/// UTF-8: unfinished 1/3
/// Bhv fixed, BYTE_NEXT at bad byte
///
TEST (Error, Utf8Unfinished13)
{
    std::string s = "abcde" "\xE0\xAC\x8A" "\xE1" "Q";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"abcde" "\u0B0A\u263A" "Q", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_NEXT, h.firstEvent);
    EXPECT_EQ(9u, h.pos()); // abcde, 3×Oriya, unfinished Ethiopic OK, and #9 Q is bad
}


///
/// UTF-8: abrupt end 2/3
/// Bhv fixed, BYTE_NEXT at bad byte
///
TEST (Error, Utf8Unfinished23)
{
    std::string s = "abcde" "\xE0\xAC\x8A" "\xE1\x8A" "Q";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"abcde" "\u0B0A\u263A" "Q", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_NEXT, h.firstEvent);
    EXPECT_EQ(10u, h.pos()); // abcde, 3×Oriya, 2×unfinished Ethiopic OK, and #10 Q is bad
}


///
/// UTF-8: long code of length 3
///   E0 9F BF = 7FF encoded in three bytes
/// Bhv fixed, CODE at start of CP
///
TEST (Error, Utf8LongCode3)
{
    std::string s = "xyz" "\xD1\xA6" "\xE0\x9F\xBF";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"xyz" "\u0466\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::CODE, h.firstEvent);
    EXPECT_EQ(5u, h.pos()); // xyz, 2×Cyr OK, and #4 (7F in two bytes) is bad
}


///
/// UTF-8: surrogate code of length 3
///   (surrogate codes are OUTRIGHT BANNED in Unicode)
///   ED BB B0 = DEF0, high sur
/// Bhv fixed, CODE at start of CP
///
TEST (Error, Utf8Surrogate3)
{
    std::string s = "u" "\xD1\xA6" "\xED\xBB\xB0";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"u" "\u0466\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::CODE, h.firstEvent);
    EXPECT_EQ(3u, h.pos()); // u, 2×Cyr OK, and #3 (high-sur in UTF-8) is bad
}


///
/// UTF-8: long code of length 3 + abrupt end
///   E0 9F <= 7FF, need 2 bytes
/// Bhv varies
///  • CODE at start of CP
///  • END at start of CP   <<<<<<<
///
TEST (Error, Utf8LongCodeAbruptEnd3)
{
    std::string s = "xyz" "\xD1\xA6" "\xE0\x9F";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"xyz" "\u0466\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::END, h.firstEvent);
    EXPECT_EQ(5u, h.pos()); // xyz, 2×Cyr OK, and #4 (bad CP) is bad
}


///
/// UTF-8: long code of length 3 + bad byte
///   E0 9F <= 7FF, need 2 bytes
/// Bhv fixed: BYTE_NEXT at bad byte
///
TEST (Error, Utf8LongCodeBadByte3)
{
    std::string s = "xyz" "\xD1\xA6" "\xE0\x9F" "t";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"xyz" "\u0466\u263A" "t", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_NEXT, h.firstEvent);
    EXPECT_EQ(7u, h.pos()); // xyz, 2×Cyr, E0 9F OK, and #7 is bad byte
}




///
/// UTF-8: banned byte
/// Bhv varies: first BYTE_START on FA, then 80??
///   (we skip those 80)
///
TEST (Error, Utf8BannedByte1)
{
    std::string s = "xyz" "\xD1\xA6" "\xFA\x80\x80\x80\x80\x80";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"xyz" "\u0466\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_START, h.firstEvent);
    EXPECT_EQ(5u, h.pos()); // xyz, 2×Cyr, and #5 is bad byte
}


///
/// UTF-8: banned byte
/// Bhv varies: first BYTE_START on FA, then 80??
///   (we skip those 80)
///
TEST (Error, Utf8BannedByte2)
{
    std::string s = "xyz" "\xD1\xA6" "\xFA\x81\x82\x93\x94\xA5\xB6" "u";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"xyz" "\u0466\u263A" "u", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_START, h.firstEvent);
    EXPECT_EQ(5u, h.pos()); // xyz, 2×Cyr, and #5 is bad byte
}


///
/// UTF-8: continuation byte
/// Bhv varies: first BYTE_START on 80 then what????
///   (we trigger exactly one alarm)
///
TEST (Error, Utf8EightyByte1)
{
    std::string s = "xyz" "\xD1\xA6" "\x80\x80\x80\x80\x80";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"xyz" "\u0466\u263A", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_START, h.firstEvent);
    EXPECT_EQ(5u, h.pos()); // xyz, 2×Cyr, and #5 is bad byte
}


///
/// UTF-8: continuation byte
/// Bhv varies: first BYTE_START on 80 then what????
///   (we trigger exactly one alarm)
///
TEST (Error, Utf8EightyByte2)
{
    std::string s = "xyz" "\xD1\xA6" "\x81\x82\x93\x94\xA5\xB6" "u";
    MyHandler h(s);
    auto r = mojibake::to<std::u32string>(s, h);
    EXPECT_EQ(U"xyz" "\u0466\u263A" "u", r);
    EXPECT_EQ(1, h.nEvents);
    EXPECT_EQ(mojibake::Event::BYTE_START, h.firstEvent);
    EXPECT_EQ(5u, h.pos()); // xyz, 2×Cyr, and #5 is bad byte
}


///// mojibake::simpleCaseFold /////////////////////////////////////////////////


TEST (SimpleCaseFold, SomeChars)
{
    // Missing
    EXPECT_EQ(0u, mojibake::simpleCaseFold(0));
    // Simple Latin (bicameral)
    EXPECT_EQ(U'q', mojibake::simpleCaseFold('q'));
    // Smth from Cyrillic (bicameral)
    EXPECT_EQ(0x047Fu, mojibake::simpleCaseFold(0x047E));
    // Smth from Kannada (unicameral)
    EXPECT_EQ(0x0C9Bu, mojibake::simpleCaseFold(0x0C9B));
    // Complex Greek (to ensure simplicity)
    EXPECT_EQ(0x1FA4u, mojibake::simpleCaseFold(0x1FAC));
    // Cherokee (lo → hi, unlike most scripts)
    EXPECT_EQ(0x13DCu, mojibake::simpleCaseFold(0xABAC));
    // Adlam (last bicameral)
    EXPECT_EQ(0x1E92Du, mojibake::simpleCaseFold(0x1E90B));
    // Just limit
    EXPECT_EQ(0x1E93Fu, mojibake::simpleCaseFold(0x1E93F));
    // More than limit
    EXPECT_EQ(0x1E940u, mojibake::simpleCaseFold(0x1E940));
}


TEST (SimpleCaseFold, Container)
{
    std::string_view input = "\x00" "q" "\u047E" "\u0C9B" "\u1FAC" "\uABAC" "\U0001E90B" "\U0001E93F" "\U0001E940";
    auto r = mojibake::simpleCaseFold<std::u16string>(input);
    EXPECT_EQ(u"\u0000" "q" "\u047F" "\u0C9B" "\u1FA4" "\u13DC" "\u1E92D" "\u1E93F" "\u1E940", r);
}


TEST (SimpleCaseFold, Container2)
{
    std::string_view input = "\x00" "q" "\u047E" "\u0C9B" "\u1FAC" "\uABAC" "\U0001E90B" "\U0001E93F" "\U0001E940";
    std::u16string r;
    mojibake::simpleCaseFold<std::u16string>(input, r);
    EXPECT_EQ(u"\u0000" "q" "\u047F" "\u0C9B" "\u1FA4" "\u13DC" "\u1E92D" "\u1E93F" "\u1E940", r);
}


TEST (SimpleCaseFold, ConstChar)
{
    const char* input = "\x00" "q" "\u047E" "\u0C9B" "\u1FAC" "\uABAC" "\U0001E90B" "\U0001E93F" "\U0001E940";
    auto r = mojibake::simpleCaseFold<std::u16string>(input);
    EXPECT_EQ(u"\u0000" "q" "\u047F" "\u0C9B" "\u1FA4" "\u13DC" "\u1E92D" "\u1E93F" "\u1E940", r);
}


///// mojibake::copyLim ////////////////////////////////////////////////////////


TEST (CopyLim, SimpleBegEnd)
{
    std::u32string_view src = U"abc" "\u1234" "\U00012345" "def";
    char dest[30];
    std::fill(std::begin(dest), std::end(dest), 'Q');
    auto r = mojibake::copyLimM(src, std::begin(dest), std::end(dest));
    EXPECT_EQ('Q', *r);

    *r = '\0';
    EXPECT_STREQ("abc" "\u1234" "\U00012345" "def", dest);
}


TEST (CopyLim, SimpleBegLen)
{
    std::u32string_view src = U"abc" "\u1234" "\U00012345" "def";
    char dest[30];
    std::fill(std::begin(dest), std::end(dest), 'Q');
    auto r = mojibake::copyLimM(src, std::begin(dest), std::size(dest));
    EXPECT_EQ('Q', *r);

    *r = '\0';
    EXPECT_STREQ("abc" "\u1234" "\U00012345" "def", dest);
}


///
/// UTF-32: every CP is a single unit, just check that it stops
///
TEST (CopyLim, U32Lim)
{
    std::u16string_view src = u"abc" "\u1234" "\U00012345" "def";
    char32_t dest[8];
    std::fill(std::begin(dest), std::end(dest), 'W');
    auto r = mojibake::copyLimM(src, dest, dest + 4);
    EXPECT_EQ(dest + 4, r);
    EXPECT_EQ(U'W', *r);

    *r = '\0';
    std::u32string_view expected = U"abc" "\u1234";
    EXPECT_EQ(expected, dest);
}


///
/// Same, overload start-len
///
TEST (CopyLim, U32LimLen)
{
    std::u16string_view src = u"abc" "\u1234" "\U00012345" "def";
    char32_t dest[8];
    std::fill(std::begin(dest), std::end(dest), 'X');
    auto r = mojibake::copyLimM(src, dest, 4);
    EXPECT_EQ(dest + 4, r);
    EXPECT_EQ(U'X', *r);

    *r = '\0';
    std::u32string_view expected = U"abc" "\u1234";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-16: stop at 1-word CPs
///
TEST (CopyLim, U16Bad1)
{
    std::u32string_view src = U"АБВГДЕЁ";
    char16_t dest[8];
    std::fill(std::begin(dest), std::end(dest), 'S');
    auto r = mojibake::copyLimM(src, dest, dest + 4);
    EXPECT_EQ(dest + 4, r);
    EXPECT_EQ('S', *r);

    *r = '\0';
    std::u16string_view expected = u"АБВГ";
    EXPECT_EQ(expected, dest);
}


///
/// Same, overload start-len
///
TEST (CopyLim, U16Bad1Len)
{
    std::u32string_view src = U"АБВГДЕЁ";
    char16_t dest[8];
    std::fill(std::begin(dest), std::end(dest), 'S');
    auto r = mojibake::copyLimM(src, dest, 4);
    EXPECT_EQ(dest + 4, r);
    EXPECT_EQ('S', *r);

    *r = '\0';
    std::u16string_view expected = u"АБВГ";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-16: stop at 2-word CPs when only 1 byte left
///
TEST (CopyLim, U16Bad2)
{
    std::u32string_view src = U"АБ" "\U00012345" "\U00023456" "abc";
    char16_t dest[8];
    std::fill(std::begin(dest), std::end(dest), 'D');
    auto r = mojibake::copyLimM(src, dest, dest + 5);
    EXPECT_EQ(dest + 4, r);
    EXPECT_EQ('D', *r);

    *r = '\0';
    std::u16string_view expected = u"АБ" "\U00012345";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-16: stop at 2-word CPs when it just fits
///
TEST (CopyLim, U16Ok2)
{
    std::u32string_view src = U"АБ" "\U00012345" "\U00023456" "\U00034567" "abc";
    char16_t dest[8];
    std::fill(std::begin(dest), std::end(dest), 'Z');
    auto r = mojibake::copyLimM(src, dest, dest + 6);
    EXPECT_EQ(dest + 6, r);
    EXPECT_EQ('Z', *r);

    *r = '\0';
    std::u16string_view expected = u"АБ" "\U00012345" "\U00023456";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-8: stop at 1-byte CPs
///
TEST (CopyLim, U8Bad1)
{
    std::u32string_view src = U"abcdefgh";
    char dest[8];
    std::fill(std::begin(dest), std::end(dest), 'E');
    auto r = mojibake::copyLimM(src, dest, dest + 4);
    EXPECT_EQ(dest + 4, r);
    EXPECT_EQ('E', *r);

    *r = '\0';
    std::string_view expected = "abcd";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-8: stop at 2-byte CPs: 1 byte left
///
TEST (CopyLim, U8Bad2)
{
    std::u32string_view src = U"АБВГДЕЁЖ";  // Cyrillic here
    char dest[8];
    std::fill(std::begin(dest), std::end(dest), 'Y');
    auto r = mojibake::copyLimM(src, dest, dest + 7);
    EXPECT_EQ(dest + 6, r);
    EXPECT_EQ('Y', *r);

    *r = '\0';
    std::string_view expected = "АБВ";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-8: stop at 2-byte CPs: just fits
///
TEST (CopyLim, U8Ok2)
{
    std::u32string_view src = U"АБВГДЕЁЖ";  // Cyrillic here
    char dest[10];
    std::fill(std::begin(dest), std::end(dest), 'U');
    auto r = mojibake::copyLimM(src, dest, dest + 8);
    EXPECT_EQ(dest + 8, r);
    EXPECT_EQ('U', *r);

    *r = '\0';
    std::string_view expected = "АБВГ";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-8: stop at 3-byte CPs: 2 bytes left
///
TEST (CopyLim, U8Bad3)
{
    std::u32string_view src = U"აბგდევზ";   // Georgian here
    char dest[15];
    std::fill(std::begin(dest), std::end(dest), 'P');
    auto r = mojibake::copyLimM(src, dest, dest + 11);
    EXPECT_EQ(dest + 9, r);
    EXPECT_EQ('P', *r);

    *r = '\0';
    std::string_view expected = "აბგ";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-8: stop at 3-byte CPs: just OK
///
TEST (CopyLim, U8Ok3)
{
    std::u32string_view src = U"აბგდევზ";   // Georgian here
    char dest[15];
    std::fill(std::begin(dest), std::end(dest), 'L');
    auto r = mojibake::copyLimM(src, dest, dest + 12);
    EXPECT_EQ(dest + 12, r);
    EXPECT_EQ('L', *r);

    *r = '\0';
    std::string_view expected = "აბგდ";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-8: stop at 4-byte CPs: 3 bytes left
///
TEST (CopyLim, U8Bad4)
{
    std::u32string_view src = U"😀😁😂😃😄😅";   // Smilies here
    char dest[15];
    std::fill(std::begin(dest), std::end(dest), 'B');
    auto r = mojibake::copyLimM(src, dest, dest + 11);
    EXPECT_EQ(dest + 8, r);
    EXPECT_EQ('B', *r);

    *r = '\0';
    std::string_view expected = "😀😁";
    EXPECT_EQ(expected, dest);
}


///
/// UTF-8: stop at 4-byte CPs: just OK
///
TEST (CopyLim, U8Ok4)
{
    std::u32string_view src = U"😀😁😂😃😄😅";   // Smilies here
    char dest[15];
    std::fill(std::begin(dest), std::end(dest), 'J');
    auto r = mojibake::copyLimM(src, dest, dest + 12);
    EXPECT_EQ(dest + 12, r);
    EXPECT_EQ('J', *r);

    *r = '\0';
    std::string_view expected = "😀😁😂";
    EXPECT_EQ(expected, dest);
}


///
/// Test this bhv of Skip:
/// 3-byte mojibake EF BF BD does not fit, but the next char fits
///
TEST (CopyLim, SkipMojibakeBadCharacterFits)
{
    char32_t src1[] { 'a', 'b', 'c', 0x12345678, 'd', 'e', 'f' };
    std::u32string_view src{ src1, std::size(src1) };
    char dest[15];
    std::fill(std::begin(dest), std::end(dest), 'K');
    auto r = mojibake::copyLimS(src, dest, 4);
    EXPECT_EQ(dest + 4, r);
    EXPECT_EQ('K', *r);\

    *r = '\0';
    std::string_view expected = "abcd";
    EXPECT_EQ(expected, dest);
}


///
/// Test this bhv of Skip:
/// Same but we write mojibake
///
TEST (CopyLim, U8MojibakeDoesNotFit)
{
    char32_t src1[] { 'a', 'b', 'c', 0x12345678, 'd', 'e', 'f' };
    std::u32string_view src{ src1, std::size(src1) };
    char dest[15];
    std::fill(std::begin(dest), std::end(dest), 'N');
    auto r = mojibake::copyLimM(src, dest, 4);
    EXPECT_EQ(dest + 3, r);
    EXPECT_EQ('N', *r);\

    *r = '\0';
    std::string_view expected = "abc";
    EXPECT_EQ(expected, dest);
}

///
/// Same but mojibake just fits
///
TEST (CopyLim, U8MojibakeJustFits)
{
    char32_t src1[] { 'a', 'b', 'c', 0x12345678, 'd', 'e', 'f' };
    std::u32string_view src{ src1, std::size(src1) };
    char dest[15];
    std::fill(std::begin(dest), std::end(dest), 'N');
    auto r = mojibake::copyLimM(src, dest, 6);
    EXPECT_EQ(dest + 6, r);
    EXPECT_EQ('N', *r);\

    *r = '\0';
    std::string_view expected = "abc\uFFFD";
    EXPECT_EQ(expected, dest);
}


///
/// Same but have 1 character left
///
TEST (CopyLim, U8MojibakeGoOn)
{
    char32_t src1[] { 'a', 'b', 'c', 0x12345678, 'd', 'e', 'f' };
    std::u32string_view src{ src1, std::size(src1) };
    char dest[15];
    std::fill(std::begin(dest), std::end(dest), 'N');
    auto r = mojibake::copyLimM(src, dest, 7);
    EXPECT_EQ(dest + 7, r);
    EXPECT_EQ('N', *r);\

    *r = '\0';
    std::string_view expected = "abc\uFFFD" "d";
    EXPECT_EQ(expected, dest);
}


///// isAliasable //////////////////////////////////////////////////////////////

struct OneByte {
    unsigned char c;
};

struct TwoBytes {
    unsigned char lo, hi;
};

// All false’s can turn to true on some strange machines
static_assert (mojibake::detail::isAliasable<char, char>());
static_assert( mojibake::detail::isAliasable<char16_t, uint16_t>());
static_assert( mojibake::detail::isAliasable<uint16_t, char16_t>());
static_assert(!mojibake::detail::isAliasable<char32_t, uint16_t>());
static_assert(!mojibake::detail::isAliasable<uint16_t, char32_t>());
static_assert(!mojibake::detail::isAliasable<char16_t, TwoBytes>());
static_assert( mojibake::detail::isAliasable<TwoBytes, char16_t>());


///// ConvString ///////////////////////////////////////////////////////////////


///
/// Same types: e.g. char and char
///
TEST (ConvString, SameType)
{
    /// Banned bytes here
    std::string_view s = "xyz" "\xD1\xA6" "\xFA\x81\x82\x93\x94\xA5\xB6" "u";
    using Conv = mojibake::ConvString<char, char>;
    Conv q(s);
    static_assert(Conv::isStringView());
    static_assert(!Conv::isConverted());
    EXPECT_EQ(s, q.sv());
    EXPECT_EQ(s.data(), q.data());
}


///
/// Close types
///
TEST (ConvString, CloseTypes)
{
    /// Banned bytes here
    std::string_view s = "xyz" "\xD1\xA6" "\xFA\x81\x82\x93\x94\xA5\xB6" "u";
    using Conv = mojibake::ConvString<OneByte, char>;
    Conv q(s);
    static_assert(Conv::isStringView());
    static_assert(!Conv::isConverted());
    EXPECT_EQ(static_cast<const void*>(s.data()), static_cast<const void*>(q.data()));
    EXPECT_EQ(s.length(), q.length());
    // Cannot compare those sv’s, but equal data and length should be enough
}


///
/// Really different types
///
TEST (ConvString, DiffTypes)
{
    /// Banned bytes here
    std::string_view s = "xyz" "\xD1\xA6" "\xFA\x81\x82\x93\x94\xA5\xB6" "u";
    using Conv = mojibake::ConvString<uint32_t, char>;
    Conv q(s);
    static_assert(Conv::isConverted());
    static_assert(!Conv::isStringView());
    // A single mojibake here: FFFD
    static uint32_t cps[] { 'x', 'y', 'z', U'\u0466', U'\uFFFD', 'u' };

    EXPECT_EQ(std::size(cps), q.length());
    auto sv = q.sv();
    EXPECT_TRUE(std::equal(std::begin(cps), std::end(cps), sv.begin(), sv.end()));
}


///// Call iterators ///////////////////////////////////////////////////////////


TEST (CallIterator, ToUtf8)
{
    std::string r;
    auto func = [&r](char x) {
        r += x;
    };
    std::u32string_view src = U"abc\u040B\u1234\U00012345";

    mojibake::copyS(src.begin(), src.end(), mojibake::Utf8CallIterator(func));
    EXPECT_EQ("abc\u040B\u1234\U00012345", r);
}


TEST (CallIterator, ToUtf16)
{
    std::u16string r;
    auto func = [&r](char16_t x) {
        r += x;
    };
    std::u32string_view src = U"abc\u040B\u1234\U00012345";

    mojibake::copyS(src.begin(), src.end(), mojibake::Utf16CallIterator(func));
    EXPECT_EQ(u"abc\u040B\u1234\U00012345", r);
}


TEST (CallIterator, ToUtf32)
{
    std::u32string r;
    auto func = [&r](char32_t x) {
        r += x;
    };
    std::u16string_view src = u"abc\u040B\u1234\U00012345";

    mojibake::copyS(src.begin(), src.end(), mojibake::Utf32CallIterator(func));
    EXPECT_EQ(U"abc\u040B\u1234\U00012345", r);
}


///// countCps /////////////////////////////////////////////////////////////////


///
/// UTF-8, ASCII string
///
TEST(CountCps, Utf8Ascii)
{
    std::string_view s = "\u0000 alphabravo\u007F"sv;
    EXPECT_EQ(13u, s.length());
    EXPECT_EQ(13u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(13u, mojibake::countCps(s));
}


///
/// UTF-8, normal string
///
TEST(CountCps, Utf8Normal)
{
    std::string_view s = "abc\u040B\u1234\U00012345"sv;
    EXPECT_EQ(12, s.length());  // All 3 lengths here
    EXPECT_EQ(6u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(6u, mojibake::countCps(s));
}


///
/// UTF-8, string from Base Plane
///
TEST(CountCps, Utf16Bmp)
{
    std::u16string_view s = u"\u0000 alphaЖႴꔀ\uD7FF\uE000bravo\uFFFF"sv;
    EXPECT_EQ(18u, s.length());
    EXPECT_EQ(18u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(18u, mojibake::countCps(s));
}


///
/// UTF-16, normal string
///
TEST(CountCps, Utf16Normal)
{
    std::u16string_view s = u"abc\u040B\u1234\U00012345"sv;
    EXPECT_EQ(7u, s.length());
    EXPECT_EQ(6u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(6u, mojibake::countCps(s));
}


///
/// UTF-16, zoo of misc. troubles
/// Known troubles:
/// • 1st surrogate w/o 2nd
/// • 2nd surrogate
///
TEST(CountCps, Utf16Zoo)
{
    std::u16string s = u"abc\u040B";
    s.push_back(0xD800);
    s += u"\u1234";
    s.push_back(0xDDDD);
    s += u"\U00012345";
    EXPECT_EQ(9u, s.length());
    EXPECT_EQ(6u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(6u, mojibake::countCps(s));
}


///
/// UTF-16, pair abruptly ends: prerequisite
/// Also a simple check for pairs
///
TEST(CountCps, Utf16AbruptEndPrereq)
{
    std::u16string_view s = u"\U00012345\U000EEEEE"sv;
    EXPECT_EQ(4, s.length());
    EXPECT_EQ(2u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(2u, mojibake::countCps(s));
}


///
/// UTF-16, pair abruptly ends: should not read beyond end
///
TEST(CountCps, Utf16AbruptEnd)
{
    std::u16string_view s0 = u"\U00012345\U000EEEEE";
    auto s = s0.substr(0, 3);
    EXPECT_EQ(3, s.length());
    EXPECT_EQ(1u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(1u, mojibake::countCps(s));
}


///
/// UTF-32, normal string
///
TEST(CountCps, Utf32Normal)
{
    std::u32string_view s = U"abc\u040B\u1234\U00012345";
    EXPECT_EQ(6u, s.length());
    EXPECT_EQ(6u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(6u, mojibake::countCps(s));
}


///
/// UTF-32, zoo of misc. troubles
/// Known troubles:
/// • Lone surrogate of either type
/// • Too high CP
///
TEST(CountCps, Utf32Zoo)
{
    std::u32string s = U"abc";  // 3
    s.push_back(0xDD00);    // Surrogate
    s.push_back(0x040B);        // 4
    s.push_back(0xD999);    // Surrogate
    s.push_back(0x110000);  // Too high
    s.push_back(0x1234);        // 5
    s.push_back(0x12345);       // 6
    EXPECT_EQ(6u, mojibake::countCps(s.cbegin(), s.cend()));
    EXPECT_EQ(6u, mojibake::countCps(s));
}
