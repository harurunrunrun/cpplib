---
title: Maximum Substring Occurrences by Length (長さ別部分文字列最大出現回数) [NSUBSTR]
documentation_of: ../../../../src/algorithm/string/suffix/maximum_substring_occurrences_by_length.hpp
---

文字列の各長さについて、同じ内容の部分文字列が現れる回数の最大値を返す。

## API

```cpp
template<int MAX_LENGTH = -1, int ALPHABET = 26, char OFFSET = 'a'>
std::vector<long long> maximum_substring_occurrences_by_length(std::string_view text)
```

`MAX_LENGTH` を省略する（`-1` にする）と、実行時のサイズに合わせて `std::vector` で suffix automaton の状態を確保する。
`MAX_LENGTH` を明示すると、状態数 `2 * MAX_LENGTH + 1` の固定容量の suffix automaton をヒープ上に確保する。

```cpp
auto result = maximum_substring_occurrences_by_length(text);
auto fixed = maximum_substring_occurrences_by_length<100000>(text);
```

## 引数・戻り値

- `text`: 対象の文字列。
- 戻り値の `i` 番目は、長さ `i + 1` の部分文字列の最大出現回数である。
  空文字列では空の `std::vector` を返す。

## API 別の時間計算量・空間計算量

$N = \lvert \mathtt{text}\rvert$、アルファベットの大きさ `ALPHABET` を $A$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 可変長版 | $O(AN)$ | $O(AN)$ |
| 固定容量版 `<MAX_LENGTH>` | $O(A \cdot \texttt{MAX_LENGTH}+N)$ | $O(A \cdot \texttt{MAX_LENGTH}+N)$ |

## 注意点

- 固定容量版では `text.size() <= MAX_LENGTH` が必要である。また、いずれの版でも `text.size() <= (INT_MAX - 1) / 2` が必要である。違反時は `std::runtime_error` を送出する。
- すべての文字は `[OFFSET, OFFSET + ALPHABET)` に含まれる必要があり、違反時は `std::runtime_error` を送出する。
- 出現は開始位置で区別し、互いに重なる出現もそれぞれ数える。
- `MAX_LENGTH * 2 + 1` が `int` に収まる必要がある（`static_assert` で検査する）。
