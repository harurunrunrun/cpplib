---
title: Longest Common Subsequence (最長共通部分列) [ALDS1_10_C]
documentation_of: ../../../../src/algorithm/string/sequence/longest_common_subsequence.hpp
---

2 つの列の両方から順序を保って取り出せる最長の部分列（LCS）を求める。

## 関数

```cpp
std::size_t longest_common_subsequence_length_bytes(
    std::string_view a,
    std::string_view b
)
template<class Sequence>
int longest_common_subsequence_length(const Sequence& a, const Sequence& b)
template<class Sequence>
Sequence longest_common_subsequence(const Sequence& a, const Sequence& b)
```

- `longest_common_subsequence_length_bytes`: バイト列 `a`, `b` の LCS
  の長さをビット並列 DP で返す。埋め込みの NUL を含む `0x00` から
  `0xFF` までの全バイト値を区別する。
- `longest_common_subsequence_length`: `operator==` で比較できる任意の
  ランダムアクセス列について、LCS の長さを通常の DP で返す。
- `longest_common_subsequence`: 通常の DP 表から LCS を 1 つ復元して返す。
  LCS が複数ある場合、どれを返すかは規定しない。

## 時間計算量・空間計算量

$N=\lvert a\rvert$、$M=\lvert b\rvert$、$S=\min(N,M)$、$T=\max(N,M)$、復元する LCS
の長さを $L$ とする。ビット並列版の機械語長は 64 ビットである。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `longest_common_subsequence_length_bytes(a, b)` | $O(T\lceil S/64\rceil+S)$ | $O(256\lceil S/64\rceil)$ |
| `longest_common_subsequence_length(a, b)` | $O(NM)$ | $O(M)$ |
| `longest_common_subsequence(a, b)` | $O(NM+L)$ | $O(NM+L)$ |

バイト版は短い方の列について、各バイト値の出現位置を 256 個のビット集合にする。
長い方の列の各バイトを処理するたびに、DP 行の差分を表すビット集合をワード単位の論理演算と減算で
更新する。復元版は DP 表を作った後、$L$ 要素の結果の列を構築する。

## 注意点

- バイト版の引数は `std::string_view` であり、入力を変更しない。返すのは
  長さだけで、LCS 自体は復元しない。
- バイト版は入力の短い側を自動的にビット集合にするため、引数の順序によって
  計算量は変わらない。入力が長すぎて作業領域を確保できない場合は `std::length_error` を送出する。
- 汎用版の列長は `int` に収まる必要がある。
- 復元版が返す要素の列は、入力中の相対順序を保つ。
