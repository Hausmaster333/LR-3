#include "compression/lzw.h"
#include "core/sequence.h"
#include <gtest/gtest.h>
#include <cstdint>
#include <stdexcept>

MutableArraySequence<uint8_t>* make_bytes(const char* str) {
    auto* seq = new MutableArraySequence<uint8_t>();
    for (int idx = 0; str[idx] != '\0'; idx++) {
        seq->append(static_cast<uint8_t>(str[idx]));
    }
    return seq;
}

bool sequences_equal(const MutableArraySequence<uint8_t>* a, const MutableArraySequence<uint8_t>* b) { // Сравнивает две последовательности байтов 
    if (a->get_count() != b->get_count()) return false;

    for (int idx = 0; idx < a->get_count(); idx++) {
        if (a->get(idx) != b->get(idx)) return false;
    }

    return true;
}

void expect_codes_equal(const MutableArraySequence<uint16_t>* actual, const uint16_t* expected, int count) { // Сравнивает результат сжатия с заранее ожидаемыми LZW-кодами
    ASSERT_EQ(actual->get_count(), count);

    for (int idx = 0; idx < count; idx++) {
        EXPECT_EQ(actual->get(idx), expected[idx]);
    }
}

TEST(LzwTest, EmptyInput) {
    MutableArraySequence<uint8_t> empty;
    auto* compressed = lzw_compress(&empty);
    EXPECT_EQ(compressed->get_count(), 0);

    auto* decompressed = lzw_decompress(compressed);
    EXPECT_EQ(decompressed->get_count(), 0);

    delete compressed;
    delete decompressed;
}

TEST(LzwTest, NullInputThrows) {
    EXPECT_THROW(lzw_compress(nullptr), std::invalid_argument);
    EXPECT_THROW(lzw_decompress(nullptr), std::invalid_argument);
}

TEST(LzwTest, EncodeDictAddsChildrenWithSequentialCodes) {
    LzwEncodeDict dict;

    EXPECT_EQ(dict.find_child(static_cast<uint16_t>('A'), static_cast<uint8_t>('B')), -1);

    dict.add_child(static_cast<uint16_t>('A'), static_cast<uint8_t>('B'));
    dict.add_child(static_cast<uint16_t>('A'), static_cast<uint8_t>('C'));
    dict.add_child(256, static_cast<uint8_t>('A'));

    EXPECT_EQ(dict.find_child(static_cast<uint16_t>('A'), static_cast<uint8_t>('B')), 256);
    EXPECT_EQ(dict.find_child(static_cast<uint16_t>('A'), static_cast<uint8_t>('C')), 257);
    EXPECT_EQ(dict.find_child(256, static_cast<uint8_t>('A')), 258);
    EXPECT_EQ(dict.find_child(static_cast<uint16_t>('A'), static_cast<uint8_t>('D')), -1);
}

TEST(LzwTest, SingleByte) {
    auto* input = make_bytes("A");
    auto* compressed = lzw_compress(input);
    EXPECT_EQ(compressed->get_count(), 1);
    EXPECT_EQ(compressed->get(0), static_cast<uint16_t>('A'));

    auto* decompressed = lzw_decompress(compressed);
    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, AllSameBytes) {
    auto* input = make_bytes("AAAAAAA");
    auto* compressed = lzw_compress(input);

    EXPECT_LT(compressed->get_count(), input->get_count());

    auto* decompressed = lzw_decompress(compressed);
    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, KnownCodesForSimplePattern) {
    auto* input = make_bytes("ABABABA");
    auto* compressed = lzw_compress(input);
    const uint16_t expected[] = {static_cast<uint16_t>('A'), static_cast<uint16_t>('B'), 256, 258};

    expect_codes_equal(compressed, expected, 4);

    auto* decompressed = lzw_decompress(compressed);
    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, DecompressesKnownSimplePattern) {
    MutableArraySequence<uint16_t> compressed;
    compressed.append(static_cast<uint16_t>('A'));
    compressed.append(static_cast<uint16_t>('B'));
    compressed.append(256);
    compressed.append(258);

    auto* expected = make_bytes("ABABABA");
    auto* decompressed = lzw_decompress(&compressed);

    EXPECT_TRUE(sequences_equal(expected, decompressed));

    delete expected;
    delete decompressed;
}

TEST(LzwTest, KnownCodesForClassicText) {
    auto* input = make_bytes("TOBEORNOTTOBEORTOBEORNOT");
    auto* compressed = lzw_compress(input);
    const uint16_t expected[] = {
        static_cast<uint16_t>('T'),
        static_cast<uint16_t>('O'),
        static_cast<uint16_t>('B'),
        static_cast<uint16_t>('E'),
        static_cast<uint16_t>('O'),
        static_cast<uint16_t>('R'),
        static_cast<uint16_t>('N'),
        static_cast<uint16_t>('O'),
        static_cast<uint16_t>('T'),
        256,
        258,
        260,
        265,
        259,
        261,
        263
    };

    expect_codes_equal(compressed, expected, 16);

    delete input;
    delete compressed;
}

TEST(LzwTest, DecompressesClassicTextCodes) {
    MutableArraySequence<uint16_t> compressed;
    compressed.append(static_cast<uint16_t>('T'));
    compressed.append(static_cast<uint16_t>('O'));
    compressed.append(static_cast<uint16_t>('B'));
    compressed.append(static_cast<uint16_t>('E'));
    compressed.append(static_cast<uint16_t>('O'));
    compressed.append(static_cast<uint16_t>('R'));
    compressed.append(static_cast<uint16_t>('N'));
    compressed.append(static_cast<uint16_t>('O'));
    compressed.append(static_cast<uint16_t>('T'));
    compressed.append(256);
    compressed.append(258);
    compressed.append(260);
    compressed.append(265);
    compressed.append(259);
    compressed.append(261);
    compressed.append(263);

    auto* expected = make_bytes("TOBEORNOTTOBEORTOBEORNOT");
    auto* decompressed = lzw_decompress(&compressed);

    EXPECT_TRUE(sequences_equal(expected, decompressed));

    delete expected;
    delete decompressed;
}

TEST(LzwTest, SimplePattern) {
    auto* input = make_bytes("ABABABAB");
    auto* compressed = lzw_compress(input);

    auto* decompressed = lzw_decompress(compressed);
    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, DecompressesKwKwKCase) {
    MutableArraySequence<uint16_t> compressed;
    compressed.append(static_cast<uint16_t>('A'));
    compressed.append(256);

    auto* expected = make_bytes("AAA");
    auto* decompressed = lzw_decompress(&compressed);

    EXPECT_TRUE(sequences_equal(expected, decompressed));

    delete expected;
    delete decompressed;
}

TEST(LzwTest, KnownCodesForZeroBytes) {
    auto* input = new MutableArraySequence<uint8_t>();
    for (int i = 0; i < 5; i++) {
        input->append(0);
    }

    auto* compressed = lzw_compress(input);
    const uint16_t expected[] = {0, 256, 256};

    expect_codes_equal(compressed, expected, 3);

    auto* decompressed = lzw_decompress(compressed);
    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, KnownCodesForBinaryAlternation) {
    auto* input = new MutableArraySequence<uint8_t>();
    for (int i = 0; i < 4; i++) {
        input->append(0);
        input->append(255);
    }

    auto* compressed = lzw_compress(input);
    const uint16_t expected[] = {0, 255, 256, 258, 255};

    expect_codes_equal(compressed, expected, 5);

    auto* decompressed = lzw_decompress(compressed);
    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, RoundTrip_Text) {
    auto* input = make_bytes("The quick brown fox jumps over the lazy dog. " "The quick brown fox jumps over the lazy dog.");
    auto* compressed = lzw_compress(input);
    auto* decompressed = lzw_decompress(compressed);

    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, RoundTrip_WithZeroBytesInside) {
    auto* input = new MutableArraySequence<uint8_t>();
    for (int i = 0; i < 40; i++) {
        input->append(0);
        input->append(static_cast<uint8_t>('A' + (i % 3)));
        input->append(255);
        input->append(0);
    }

    auto* compressed = lzw_compress(input);
    auto* decompressed = lzw_decompress(compressed);

    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, RoundTrip_Binary) {
    auto* input = new MutableArraySequence<uint8_t>();
    for (int i = 0; i < 256; i++) {
        input->append(static_cast<uint8_t>(i));
    }

    auto* compressed = lzw_compress(input);
    auto* decompressed = lzw_decompress(compressed);

    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, DecompressesMaxLiteralByte) {
    MutableArraySequence<uint16_t> compressed;
    compressed.append(255);

    auto* decompressed = lzw_decompress(&compressed);

    ASSERT_EQ(decompressed->get_count(), 1);
    EXPECT_EQ(decompressed->get(0), 255);

    delete decompressed;
}

TEST(LzwTest, InvalidFirstCodeThrows) {
    MutableArraySequence<uint16_t> compressed;
    compressed.append(256);

    EXPECT_THROW(lzw_decompress(&compressed), std::runtime_error);
}

TEST(LzwTest, InvalidCodeAfterFirstThrows) {
    MutableArraySequence<uint16_t> compressed;
    compressed.append(static_cast<uint16_t>('A'));
    compressed.append(300);

    EXPECT_THROW(lzw_decompress(&compressed), std::runtime_error);
}

TEST(LzwTest, RoundTrip_Large) {
    auto* input = new MutableArraySequence<uint8_t>();
    uint32_t state = 12345;
    for (int i = 0; i < 10000; i++) {
        state = state * 1103515245 + 12345;
        input->append(static_cast<uint8_t>((state >> 16) & 0xFF));
    }

    auto* compressed = lzw_compress(input);
    auto* decompressed = lzw_decompress(compressed);

    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}

TEST(LzwTest, CompressesWell) {
    auto* input = new MutableArraySequence<uint8_t>();
    for (int i = 0; i < 1000; i++) {
        input->append(static_cast<uint8_t>('A' + (i % 4)));
    }

    auto* compressed = lzw_compress(input);

    EXPECT_LT(compressed->get_count(), static_cast<int>(input->get_count() / 2));

    auto* decompressed = lzw_decompress(compressed);
    EXPECT_TRUE(sequences_equal(input, decompressed));

    delete input;
    delete compressed;
    delete decompressed;
}
