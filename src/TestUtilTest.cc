/* Copyright (c) 2010 Stanford University
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR(S) DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL AUTHORS BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "TestUtil.h"
#include "Buffer.h"

namespace RAMCloud {

class TestUtilTest : public ::testing::Test {
  public:
    TestUtilTest()
    {
    }

    DISALLOW_COPY_AND_ASSIGN(TestUtilTest);
};

TEST_F(TestUtilTest, fillRandom) {
    uint8_t ored[128];
    uint8_t buf[128];
    memset(ored, 0, sizeof(ored));
    for (uint32_t i = 0; i < 50; i++) {
        TestUtil::fillRandom(buf, sizeof(buf));
        for (uint32_t j = 0; j < arrayLength(buf); j++)
            ored[j] |= buf[j];
    }
    for (uint32_t j = 0; j < arrayLength(ored); j++) {
        ++ored[j]; // should overflow
        EXPECT_EQ(0, ored[j]);
    }
}

TEST_F(TestUtilTest, toString) {
    Buffer b;
    int32_t *ip = new(&b, APPEND) int32_t;
    *ip = -45;
    ip = new(&b, APPEND) int32_t;
    *ip = 0x1020304;
    char *p = new(&b, APPEND) char[10];
    memcpy(p, "abcdefghi", 10);
    ip = new(&b, APPEND) int32_t;
    *ip = 99;
    EXPECT_EQ("-45 0x1020304 abcdefghi/0 99",
              TestUtil::toString(&b));
}

TEST_F(TestUtilTest, toString_stringNotTerminated) {
    Buffer b;
    char *p = new(&b, APPEND) char[5];
    memcpy(p, "abcdefghi", 5);
    EXPECT_EQ("abcde", TestUtil::toString(&b));
}

TEST(CoreStringUtilTest, toString_templated) {
    EXPECT_EQ("3", TestUtil::toString(3));
}

TEST_F(TestUtilTest, bufferToDebugString) {
    Buffer b;
    b.append("abc\nxyz", 7);
    b.append("0123456789012345678901234567890abcdefg", 37);
    b.append("xyz", 3);
    EXPECT_EQ("abc/nxyz | 01234567890123456789(+17 chars) " "| xyz",
              TestUtil::bufferToDebugString(&b));
}

TEST_F(TestUtilTest, convertChar) {
    Buffer b;
    const char *test = "abc \x17--\x80--\x3--\n--\x7f--\\--\"--";
    uint32_t length = downCast<uint32_t>(strlen(test)) + 1;
    memcpy(static_cast<char*>(new(&b, APPEND) char[length]),
            test, length);
    EXPECT_EQ("abc /x17--/x80--/x03--/n--/x7f--/x5c--/x22--/0",
              TestUtil::toString(&b));
}

}  // namespace RAMCloud
