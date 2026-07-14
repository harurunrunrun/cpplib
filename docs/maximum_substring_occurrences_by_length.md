---
title: Maximum Substring Occurrences by Length (長さ別部分文字列最大出現回数) [NSUBSTR]
documentation_of: ../src/algorithm/string/maximum_substring_occurrences_by_length.hpp
---

文字列の各長さについて、同じ内容の部分文字列が現れる回数の最大値を返す。

## API

```cpp
template<int MAX_LENGTH, int ALPHABET = 26, char OFFSET = 'a'>
vector<long long> maximum_substring_occurrences_by_length(string_view text)
```

## 引数・戻り値

- `text`: 対象の文字列。
- 戻り値の `i` 番目は、長さ `i + 1` の部分文字列の最大出現回数。
  空文字列では空のvectorを返す。

## API別の時間計算量・空間計算量

$N=|text|$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `maximum_substring_occurrences_by_length(text)` | $O(MAX_LENGTH\cdot ALPHABET+N)$ | $O(MAX_LENGTH\cdot ALPHABET+N)$ |

## 注意点

- `text.size() <= MAX_LENGTH` が必要で、違反時は `runtime_error` を送出する。
- すべての文字は `[OFFSET, OFFSET + ALPHABET)` に含まれる必要があり、違反時は
  `runtime_error` を送出する。
- 出現は開始位置で区別し、互いに重なる出現もそれぞれ数える。
- `MAX_LENGTH * 2 + 1` が `int` に収まる必要がある。
