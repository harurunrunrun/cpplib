// competitive-verifier: STANDALONE

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include "../../src/structure/wavelet_matrix/detail/persistent_btree_bit_sequence.hpp"

namespace{

using WeightedSequence =
    wavelet_matrix_detail::PersistentBTreeBitSequence<long long, long long>;
using PlainSequence = wavelet_matrix_detail::PersistentBTreeBitVector<>;

struct TaggedPayload{
    std::string tag;
    long long value = 0;
};

struct TaggedLift{
    long long operator()(const TaggedPayload& payload) const{
        return payload.value;
    }
};

void self_test_non_trivial_payload(){
    using Sequence = wavelet_matrix_detail::PersistentBTreeBitSequence<
        TaggedPayload,
        long long,
        TaggedLift,
        1,
        4
    >;

    std::vector<unsigned char> bits(129);
    std::vector<TaggedPayload> payload(129);
    long long zero_sum = 0;
    long long one_sum = 0;
    for(int index = 0; index < 129; index++){
        bits[static_cast<std::size_t>(index)] =
            static_cast<unsigned char>(index % 3 == 0);
        payload[static_cast<std::size_t>(index)] = {
            std::to_string(index),
            index - 60
        };
        if(bits[static_cast<std::size_t>(index)] != 0){
            one_sum += index - 60;
        }else{
            zero_sum += index - 60;
        }
    }

    Sequence sequence;
    const auto original = sequence.build(bits, payload);
    const auto stats = sequence.range_stats(original, 0, 129);
    assert(stats.zero_sum == zero_sum);
    assert(stats.one_sum == one_sum);

    const auto snapshot = sequence.snapshot();
    const auto inserted = sequence.insert(
        original,
        64,
        true,
        TaggedPayload{"inserted", 777}
    );
    assert(sequence.size(inserted) == 130);
    assert(sequence.access(inserted, 64).payload.tag == "inserted");
    assert(sequence.size(original) == 129);

    const auto changed = sequence.set_payload(
        inserted,
        64,
        TaggedPayload{"changed", -222}
    );
    assert(changed.bit);
    assert(changed.old_payload.tag == "inserted");
    assert(sequence.access(changed.root, 64).payload.tag == "changed");

    const auto erased = sequence.erase(changed.root, 64);
    assert(erased.bit);
    assert(erased.payload.tag == "changed");
    assert(sequence.size(erased.root) == 129);
    for(int index : {0, 63, 64, 128}){
        assert(
            sequence.access(erased.root, index).payload.tag
            == sequence.access(original, index).payload.tag
        );
    }

    sequence.rollback(snapshot);
    assert(sequence.size(original) == 129);
    bool rejected = false;
    try{
        (void)sequence.size(inserted);
    }catch(const std::runtime_error&){
        rejected = true;
    }
    assert(rejected);
}

} // namespace

int main(){
    self_test_non_trivial_payload();

    int n, query_count;
    if(!(std::cin >> n >> query_count)) return 0;

    std::vector<unsigned char> bits(static_cast<std::size_t>(n));
    std::vector<long long> payload(static_cast<std::size_t>(n));
    for(int index = 0; index < n; index++){
        int bit;
        std::cin >> bit >> payload[static_cast<std::size_t>(index)];
        bits[static_cast<std::size_t>(index)] =
            static_cast<unsigned char>(bit != 0);
    }

    WeightedSequence weighted;
    PlainSequence plain;
    std::vector<WeightedSequence::Root> weighted_root{
        weighted.build(bits, payload)
    };
    std::vector<PlainSequence::Root> plain_root{
        plain.build(bits)
    };

    while(query_count--){
        std::string operation;
        std::cin >> operation;
        if(operation == "INSERT"){
            int version, position, bit;
            long long weight;
            std::cin >> version >> position >> bit >> weight;
            weighted_root.push_back(weighted.insert(
                weighted_root[static_cast<std::size_t>(version)],
                position,
                bit != 0,
                weight
            ));
            plain_root.push_back(plain.insert(
                plain_root[static_cast<std::size_t>(version)],
                position,
                bit != 0
            ));
            assert(weighted.size(weighted_root.back()) == plain.size(plain_root.back()));
            std::cout << weighted_root.size() - 1 << '\n';
        }else if(operation == "ERASE"){
            int version, position;
            std::cin >> version >> position;
            const auto weighted_result = weighted.erase(
                weighted_root[static_cast<std::size_t>(version)],
                position
            );
            const auto plain_result = plain.erase(
                plain_root[static_cast<std::size_t>(version)],
                position
            );
            assert(weighted_result.bit == plain_result.bit);
            weighted_root.push_back(weighted_result.root);
            plain_root.push_back(plain_result.root);
            std::cout << weighted_root.size() - 1 << ' '
                      << weighted_result.bit << ' '
                      << weighted_result.payload << '\n';
        }else if(operation == "SETP"){
            int version, position;
            long long weight;
            std::cin >> version >> position >> weight;
            const auto result = weighted.set_payload(
                weighted_root[static_cast<std::size_t>(version)],
                position,
                weight
            );
            weighted_root.push_back(result.root);
            plain_root.push_back(
                plain_root[static_cast<std::size_t>(version)]
            );
            std::cout << weighted_root.size() - 1 << ' '
                      << result.bit << ' '
                      << result.old_payload << '\n';
        }else if(operation == "ACCESS"){
            int version, position;
            std::cin >> version >> position;
            const auto entry = weighted.access(
                weighted_root[static_cast<std::size_t>(version)],
                position
            );
            assert(
                entry.bit
                == plain.access(
                    plain_root[static_cast<std::size_t>(version)],
                    position
                ).bit
            );
            std::cout << entry.bit << ' ' << entry.payload << '\n';
        }else if(operation == "RANK"){
            int version, bit, left, right;
            std::cin >> version >> bit >> left >> right;
            const int result = weighted.rank(
                weighted_root[static_cast<std::size_t>(version)],
                bit != 0,
                left,
                right
            );
            assert(
                result
                == plain.rank(
                    plain_root[static_cast<std::size_t>(version)],
                    bit != 0,
                    left,
                    right
                )
            );
            std::cout << result << '\n';
        }else if(operation == "SELECT"){
            int version, bit, occurrence;
            std::cin >> version >> bit >> occurrence;
            const int result = weighted.select(
                weighted_root[static_cast<std::size_t>(version)],
                bit != 0,
                occurrence
            );
            assert(
                result
                == plain.select(
                    plain_root[static_cast<std::size_t>(version)],
                    bit != 0,
                    occurrence
                )
            );
            std::cout << result << '\n';
        }else if(operation == "STATS"){
            int version, left, right;
            std::cin >> version >> left >> right;
            const auto result = weighted.range_stats(
                weighted_root[static_cast<std::size_t>(version)],
                left,
                right
            );
            const auto plain_rank = plain.rank_pair(
                plain_root[static_cast<std::size_t>(version)],
                left,
                right
            );
            assert(result.ones_l == plain_rank.ones_l);
            assert(result.ones_r == plain_rank.ones_r);
            std::cout << result.ones_l << ' '
                      << result.ones_r << ' '
                      << result.zero_sum << ' '
                      << result.one_sum << '\n';
        }else if(operation == "FIRST"){
            int version, bit, count;
            std::cin >> version >> bit >> count;
            std::cout << weighted.weight_of_first(
                weighted_root[static_cast<std::size_t>(version)],
                bit != 0,
                count
            ) << '\n';
        }else if(operation == "FIRST_FROM"){
            int version, bit, left, count;
            std::cin >> version >> bit >> left >> count;
            std::cout << weighted.weight_of_first(
                weighted_root[static_cast<std::size_t>(version)],
                bit != 0,
                left,
                count
            ) << '\n';
        }else if(operation == "SIZE"){
            int version;
            std::cin >> version;
            const int result = weighted.size(
                weighted_root[static_cast<std::size_t>(version)]
            );
            assert(
                result
                == plain.size(plain_root[static_cast<std::size_t>(version)])
            );
            std::cout << result << '\n';
        }else{
            throw std::runtime_error("unknown operation");
        }
    }
}
