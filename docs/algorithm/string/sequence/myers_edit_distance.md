---
title: Myers Bit-Parallel Edit Distance (Myersのビット並列編集距離) [DPL_1_E]
documentation_of: ../../../../src/algorithm/string/sequence/myers_edit_distance.hpp
---

Myers のビットベクトル法で、2 つのバイト文字列の Levenshtein 距離を求める。
挿入・削除・置換のコストはいずれも 1 である。

内部では短い方の文字列をパターンとし、64 ビットのワードを複数連結して処理する。
パターン長が 64 を超える場合も同じ API を使える。
任意のコストや、バイト文字列以外の列には `edit_distance` を使う。

## API

~~~cpp
size_t myers_bit_parallel_edit_distance(
    string_view first,
    string_view second
);
~~~

`first` を `second` に変換するために必要な、挿入・削除・置換の最小回数を返す。
距離は対称なので、内部では短い方をパターンに選び直す。
空文字列を渡した場合は、もう一方の長さを返す。

文字は `unsigned char` に変換して扱うため、値が 128 以上のバイトや
文字列中の NUL バイトも使える。文字コード単位ではなくバイト単位の距離である。

## 時間計算量・空間計算量

短い方の長さを $M$、長い方の長さを $N$、
$B=\lceil M/64\rceil$、短い方に現れる相異なるバイトの個数を $D$（$D\le 256$）とする。

| API / 操作 | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| 一致マスクの構築 | $O(M+DB)$ | $O(DB)$ |
| 1 バイトの処理 | $O(B)$ | 追加 $O(1)$ |
| `myers_bit_parallel_edit_distance(first, second)` | $O(M+DB+NB)$ | $O((D+2)B)$ |
| 空文字列を含む場合 | $O(1)$ | $O(1)$ |

アルファベットの大きさ 256 を定数とみなせば、全体の時間計算量は
$O(N\lceil M/64\rceil+M)$、空間計算量は $O(M)$ である。
ワード間では加算の繰り上がりと左シフトのあふれを伝播する。

## 注意点

- 返すのは距離だけで、編集操作の列は復元しない（復元には `diff_sequence` を使う）。
