---
title: Myers Bit-Parallel Edit Distance
documentation_of: ../src/algorithm/string/myers_edit_distance.hpp
---

Myers の bit-vector 法で、2つの byte 文字列の Levenshtein 距離を求める。
挿入、削除、置換のコストはいずれも1である。

内部では短い方の文字列を pattern とし、64 bit word を複数連結して処理する。
pattern 長が64を超える場合も同じ API を使用できる。
任意コストや byte 文字列以外の列には `edit_distance` を使用する。

# API

~~~cpp
size_t myers_bit_parallel_edit_distance(
    string_view first,
    string_view second
);
~~~

`first` を `second` に変換するために必要な、挿入・削除・置換の最小回数を返す。
距離は対称なので、内部では短い方を pattern に選び直す。
空文字列を渡した場合はもう一方の長さを返す。

文字は `unsigned char` に変換して扱うため、値が128以上の byte や
文字列中の null byte も使用できる。文字コード単位ではなく byte 単位の距離である。

# 計算量

短い方の長さを $M$、長い方の長さを $N$、
$B=\lceil M/64\rceil$、短い方に現れる異なる byte 数を $D$ とする。
$D\leq 256$ である。

| API / 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| equality mask の構築 | $O(M+DB)$ | $O(DB)$ |
| 1 byte の処理 | $O(B)$ | 追加 $O(1)$ |
| `myers_bit_parallel_edit_distance(first, second)` | $O(M+DB+NB)$ | $O((D+2)B)$ |
| 空文字列を含む場合 | $O(1)$ | $O(1)$ |

byte alphabet の大きさ256を定数とみなせば、全体の時間計算量は
$O(N\lceil M/64\rceil+M)$、空間計算量は $O(M)$ である。
各 word 間では加算 carry と左 shift carry を伝播する。
