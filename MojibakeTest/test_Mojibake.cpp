// What we test
#include "mojibake.h"

// Google test
#include "gtest/gtest.h"


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
