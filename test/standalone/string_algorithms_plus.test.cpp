// competitive-verifier: STANDALONE

#include <algorithm>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "../../src/algorithm/string/automata/dynamic_aho_corasick.hpp"
#include "../../src/algorithm/string/automata/minimal_absent_words.hpp"
#include "../../src/algorithm/string/compression/crochemore_factorization.hpp"
#include "../../src/algorithm/string/compression/lz77_factorization.hpp"
#include "../../src/algorithm/string/compression/lz78_factorization.hpp"
#include "../../src/algorithm/string/palindrome/palindromic_factorization.hpp"
#include "../../src/algorithm/string/sequence/main_lorentz.hpp"
#include "../../src/algorithm/string/sequence/run_enumeration.hpp"
#include "../../src/algorithm/string/suffix/dictionary_of_basic_factors.hpp"
#include "../../src/algorithm/string/suffix/generalized_suffix_tree.hpp"
#include "../../src/algorithm/string/suffix/suffix_tree.hpp"

namespace{

int occurrence_count(const std::string& text, const std::string& pattern){
    int result = 0;
    for(int begin = 0;
        begin + static_cast<int>(pattern.size())
            <= static_cast<int>(text.size());
        ++begin){
        result += text.compare(
            static_cast<std::size_t>(begin), pattern.size(), pattern
        ) == 0;
    }
    return result;
}

int brute_lce(const std::string& text, int first, int second){
    int result = 0;
    while(first + result < static_cast<int>(text.size())
        && second + result < static_cast<int>(text.size())
        && text[static_cast<std::size_t>(first + result)]
            == text[static_cast<std::size_t>(second + result)]){
        ++result;
    }
    return result;
}

bool is_palindrome(const std::string& text, int left, int right){
    while(left < --right){
        if(text[static_cast<std::size_t>(left++)]
            != text[static_cast<std::size_t>(right)]){
            return false;
        }
    }
    return true;
}

std::vector<StringRun> brute_runs(const std::string& text){
    const int size = static_cast<int>(text.size());
    std::vector<StringRun> result;
    for(int left = 0; left < size; ++left){
        for(int right = left + 2; right <= size; ++right){
            int period = 1;
            for(; period * 2 <= right - left; ++period){
                bool valid = true;
                for(int index = left + period; index < right; ++index){
                    if(text[static_cast<std::size_t>(index)]
                        != text[static_cast<std::size_t>(index - period)]){
                        valid = false;
                        break;
                    }
                }
                if(valid) break;
            }
            if(period * 2 > right - left) continue;
            if(left != 0
                && text[static_cast<std::size_t>(left - 1)]
                    == text[static_cast<std::size_t>(left - 1 + period)]){
                continue;
            }
            if(right != size
                && text[static_cast<std::size_t>(right)]
                    == text[static_cast<std::size_t>(right - period)]){
                continue;
            }
            result.push_back({left, right, period});
        }
    }
    std::sort(result.begin(), result.end(), [](const StringRun& left,
                                              const StringRun& right){
        return std::tuple(left.left, left.right, left.period)
            < std::tuple(right.left, right.right, right.period);
    });
    return result;
}

bool verify(const std::string& text){
    const int size = static_cast<int>(text.size());
    const SuffixTree tree(text);
    for(int left = 0; left <= size; ++left){
        for(int right = left; right <= size; ++right){
            const std::string pattern = text.substr(
                static_cast<std::size_t>(left),
                static_cast<std::size_t>(right - left)
            );
            if(tree.count(pattern) != occurrence_count(text, pattern)){
                return false;
            }
            std::vector<int> expected_locations;
            for(int begin = 0;
                begin + static_cast<int>(pattern.size()) <= size;
                ++begin){
                if(text.compare(
                    static_cast<std::size_t>(begin),
                    pattern.size(),
                    pattern
                ) == 0){
                    expected_locations.push_back(begin);
                }
            }
            if(tree.locate(pattern) != expected_locations) return false;
        }
    }
    if(decode_lz77(lz77_factorization(text)) != text) return false;
    if(decode_lz78(lz78_factorization(text)) != text) return false;
    int position = 0;
    for(const LZ77Factor& factor: lz77_factorization(text)){
        int best = 0;
        for(int source = 0; source < position; ++source){
            best = std::max(best, brute_lce(text, position, source));
        }
        if(factor.length != best) return false;
        position += factor.length + (factor.has_next_symbol ? 1 : 0);
    }
    if(position != size) return false;
    position = 0;
    for(const CrochemoreFactor& factor: crochemore_factorization(text)){
        int best = 0;
        for(int source = 0; source < position; ++source){
            best = std::max(best, brute_lce(text, position, source));
        }
        best = std::max(best, 1);
        if(factor.begin != position || factor.length != best) return false;
        position += factor.length;
    }
    if(position != size) return false;
    if(enumerate_runs(text) != brute_runs(text)) return false;
    std::vector<TandemRepeat> repetitions;
    for(int begin = 0; begin < size; ++begin){
        for(int half = 1; begin + half * 2 <= size; ++half){
            if(text.compare(
                static_cast<std::size_t>(begin),
                static_cast<std::size_t>(half),
                text,
                static_cast<std::size_t>(begin + half),
                static_cast<std::size_t>(half)
            ) == 0){
                repetitions.push_back({begin, half});
            }
        }
    }
    if(main_lorentz_tandem_repeats(text) != repetitions) return false;
    const auto absent = minimal_absent_words(text, "abc");
    for(const std::string& word: absent){
        if(text.find(word) != std::string::npos) return false;
        if(word.size() > 1
            && (text.find(word.substr(0, word.size() - 1))
                    == std::string::npos
                || text.find(word.substr(1)) == std::string::npos)){
            return false;
        }
    }
    const PalindromicFactorizationResult factorization =
        palindromic_factorization(text);
    std::vector<int> minimum(static_cast<std::size_t>(size + 1), size + 1);
    minimum[0] = 0;
    for(int right = 1; right <= size; ++right){
        for(int left = 0; left < right; ++left){
            if(is_palindrome(text, left, right)){
                minimum[static_cast<std::size_t>(right)] = std::min(
                    minimum[static_cast<std::size_t>(right)],
                    minimum[static_cast<std::size_t>(left)] + 1
                );
            }
        }
    }
    if(factorization.factor_count
        != minimum[static_cast<std::size_t>(size)]){
        return false;
    }
    position = 0;
    for(const auto [left, right]: factorization.factors){
        if(left != position || !is_palindrome(text, left, right)) return false;
        position = right;
    }
    if(position != size) return false;
    const DictionaryOfBasicFactors factors(text);
    for(int first = 0; first <= size; ++first){
        for(int second = 0; second <= size; ++second){
            if(factors.lce(first, second)
                != brute_lce(text, first, second)){
                return false;
            }
        }
    }
    return true;
}

} // namespace

int main(){
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int case_count;
    if(!(std::cin >> case_count)) return 1;
    while(case_count-- > 0){
        std::string text;
        std::cin >> text;
        if(text == "-") text.clear();
        if(!verify(text)) return 2;
    }
    const GeneralizedSuffixTree generalized({"banana", "band", "ananas"});
    if(generalized.count("ban") != 2
        || generalized.count("ana") != 4
        || generalized.count("") != 19){
        return 3;
    }
    DynamicAhoCorasick aho;
    aho.insert("a");
    aho.insert("aba");
    aho.insert("");
    if(aho.count_matches("ababa") != 11) return 4;
    aho.erase("a");
    if(aho.count_matches("ababa") != 8) return 5;
    std::cout << "OK\n";
}
