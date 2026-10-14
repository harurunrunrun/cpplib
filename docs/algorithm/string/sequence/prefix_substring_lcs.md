---
title: Prefix Substring LCS (接頭辞と部分文字列の最長共通部分列) [prefix_substring_lcs]
documentation_of: ../../../../src/algorithm/string/sequence/prefix_substring_lcs.hpp
---

`prefix_substring_lcs(S, T, queries)` は、各 `{prefix_length, left, right}` に対し
`S[0:prefix_length]` と `T[left:right]` の最長共通部分列長を返す。
クエリはオフラインで処理し、答えは入力順に返す。空の接頭辞・区間を許す。
seaweed の交差順序を更新し、各接頭辞の問い合わせを Fenwick tree 上の区間計数に帰着する。

## 時間計算量

文字列長を $M,N$、クエリ数を $Q$、問い合わせがある接頭辞数を $R$ として
$O(MN+RN\log(N+1)+Q\log(Q+1)+Q\log(N+1))$。追加空間は $O(N+Q)$。

## 注意点

- 各添字・文字列長・クエリ数は `int` の範囲内とする。
- 範囲外のクエリは `out_of_range`。
- 文字の一致だけを使う。UTF-8 はバイト列として比較する。
