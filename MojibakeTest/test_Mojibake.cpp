// What we test
#include "mojibake.h"

// Google test
#include "gtest/gtest.h"


/////
/////  mojibake::isGood ///////////////////////////////////////////////////////////
/////


TEST (IsGood, Simple)
{
    EXPECT_TRUE (mojibake::isGood(0));
    EXPECT_TRUE (mojibake::isGood(1000));
    EXPECT_TRUE (mojibake::isGood(0xD7FF));
    EXPECT_FALSE(mojibake::isGood(0xD800));
    EXPECT_FALSE(mojibake::isGood(0xDFFF));
    EXPECT_TRUE (mojibake::isGood(0xE000));
    EXPECT_TRUE (mojibake::isGood(0xFFFF));
    EXPECT_TRUE (mojibake::isGood(0x10000));
    EXPECT_TRUE (mojibake::isGood(0xFFFFF));
    EXPECT_TRUE (mojibake::isGood(0x100000));
    EXPECT_TRUE (mojibake::isGood(0x10FFFF));
    EXPECT_FALSE(mojibake::isGood(0x110000));
    EXPECT_FALSE(mojibake::isGood(0x1E0000));
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
    using Enc = mojibake::enc::Utf32;
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
    using Enc = mojibake::enc::Utf32;
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
    using Enc = mojibake::enc::Utf32;
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
    using Enc = mojibake::enc::Utf32;
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
