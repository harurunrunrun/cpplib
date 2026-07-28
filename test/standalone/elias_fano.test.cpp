// competitive-verifier: STANDALONE

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <vector>

#include "../../src/structure/compressed/elias_fano.hpp"

void check_sequence(const std::vector<std::uint64_t>& values) {
    EliasFano encoded(values);
    assert(encoded.empty() == values.empty());
    assert(encoded.size() == values.size());
    assert(encoded.universe() == (values.empty() ? 0 : values.back()));
    for (std::size_t i = 0; i < values.size(); ++i) {
        assert(encoded.select(i) == values[i]);
    }

    std::mt19937_64 random(0x31b467ULL + values.size());
    for (int query = 0; query < 1000; ++query) {
        const std::uint64_t value = random();
        assert(encoded.lower_bound(value) == static_cast<std::size_t>(
            std::lower_bound(values.begin(), values.end(), value)
            - values.begin()
        ));
        assert(encoded.rank(value) == static_cast<std::size_t>(
            std::upper_bound(values.begin(), values.end(), value)
            - values.begin()
        ));
    }
    assert(encoded.rank(std::numeric_limits<std::uint64_t>::max())
        == values.size());
}

int main() {
    check_sequence({});
    check_sequence({0, 1, 1, 7, 20, 1000});
    check_sequence({
        0,
        std::numeric_limits<std::uint64_t>::max()
    });

    std::mt19937_64 random(0x6d45e9ULL);
    for (int repetition = 0; repetition < 6; ++repetition) {
        const std::size_t size =
            90000 + static_cast<std::size_t>(random() % 10000);
        const std::size_t gap_index =
            1 + static_cast<std::size_t>(random() % 255);
        std::vector<std::uint64_t> values(size);
        for (std::size_t i = 0; i < gap_index; ++i) {
            values[i] = static_cast<std::uint64_t>(i / 3);
        }
        const std::uint64_t high_begin =
            std::numeric_limits<std::uint64_t>::max() - size;
        for (std::size_t i = gap_index; i < size; ++i) {
            values[i] = high_begin + i;
        }
        check_sequence(values);
    }

    EliasFano reusable({1, 2, 3});
    reusable.build({});
    assert(reusable.empty());
    bool unsorted = false;
    try {
        reusable.build({2, 1});
    } catch (const std::invalid_argument&) {
        unsorted = true;
    }
    assert(unsorted);

    bool out_of_range = false;
    try {
        static_cast<void>(reusable.select(0));
    } catch (const std::out_of_range&) {
        out_of_range = true;
    }
    assert(out_of_range);

    std::cout << "OK\n";
}
