---
title: Distinct Subsequences Count (相異なる部分列の個数) [DSUBSEQ]
documentation_of: ../../../../src/algorithm/string/sequence/count_distinct_subsequences.hpp
---

空の部分列を含む、相異なる部分列の個数を法 `modulus` で返す。
各文字について直前の出現時点の総数を引く、標準的な DP を用いる。

## count_distinct_subsequences

```cpp
long long count_distinct_subsequences(const string& text,
                                      long long modulus = 1000000007);
```

`text` の各バイト値を 1 文字として扱う。戻り値は $[0,\texttt{modulus})$ に入る。

## 時間計算量・空間計算量

$N$ を `text` の長さ、$\sigma$ を現れる文字の種類数（256 以下）とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `count_distinct_subsequences` | 期待 $O(N)$ | $O(\sigma)$ |

## 注意点

- `modulus` は正でなければならず、違反時は `std::runtime_error` を送出する。
- 空文字列では `1 % modulus` を返す（空の部分列のみ）。
