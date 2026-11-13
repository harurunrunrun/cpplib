---
title: Prefix Substring LCS (接頭辞と部分文字列の最長共通部分列) [prefix_substring_lcs]
documentation_of: ../../../../src/algorithm/string/sequence/prefix_substring_lcs.hpp
---

`prefix_substring_lcs(S, T, queries)` は、各クエリ `{prefix_length, left, right}` について、
$S$ の先頭 `prefix_length` 文字と $T$ の半開区間 `[left, right)` の最長共通部分列の長さを返す。
クエリはオフラインで処理し、答えは入力順に返す。空の接頭辞・空の区間も許す。
seaweed（半局所 LCS）の交差順序を更新し、各接頭辞に対する問い合わせを Fenwick 木上の区間計数に帰着する。

## API

```cpp
struct PrefixSubstringLcsQuery {
    int prefix_length;
    int left;
    int right;
};

std::vector<int> prefix_substring_lcs(
    std::string_view first,
    std::string_view second,
    const std::vector<PrefixSubstringLcsQuery>& queries
)
```

## 時間計算量

$M=\lvert S\rvert$、$N=\lvert T\rvert$、クエリ数を $Q$、問い合わせがある相異なる接頭辞の個数を $R$ として
$O(MN+RN\log(N+1)+Q\log(Q+1)+Q\log(N+1))$ である。追加空間は $O(N+Q)$。

## 注意点

- 各添字・文字列長・クエリ数は `int` の範囲内とする。
- `0 <= prefix_length <= M` かつ `0 <= left <= right <= N` を満たさないクエリがあると `std::out_of_range` を送出する。
- 文字の一致だけを使う。UTF-8 はバイト列として比較する。
