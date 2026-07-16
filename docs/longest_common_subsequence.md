---
title: Longest Common Subsequence (最長共通部分列) [ALDS1_10_C]
documentation_of: ../src/algorithm/string/longest_common_subsequence.hpp
---

2 列の両方から順序を保って取り出せる最長の部分列を求める。

## 関数

```cpp
std::size_t longest_common_subsequence_length_bytes(
    std::string_view a,
    std::string_view b
)
int longest_common_subsequence_length(a, b)
Sequence longest_common_subsequence(a, b)
```

- `longest_common_subsequence_length_bytes`: byte 列 `a`, `b` の LCS
  長を bit-parallel DP で返す。埋め込み NUL を含む `0x00` から
  `0xFF` までの全 byte 値を区別する。
- `longest_common_subsequence_length`: `operator==` で比較できる任意の
  ランダムアクセス列について、LCS 長を通常の DP で返す。
- `longest_common_subsequence`: 通常の DP 表から LCS を 1 つ復元して返す。
  LCS が複数ある場合、返す列は一意とは限らない。

## API別の時間計算量・空間計算量

$N=|a|$, $M=|b|$, $S=\min(N,M)$, $T=\max(N,M)$、復元する LCS
長を $L$ とする。bit-parallel 版の機械語長は 64 bit である。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `longest_common_subsequence_length_bytes(a,b)` | $O(T\lceil S/64\rceil+S)$ | $O(256\lceil S/64\rceil)$ |
| `longest_common_subsequence_length(a,b)` | $O(NM)$ | $O(M)$ |
| `longest_common_subsequence(a,b)` | $O(NM+L)$ | $O(NM+L)$ |

byte 版は短い列について、各 byte 値の出現位置を 256 個の bitset にする。
各 byte を処理するたびに、DP 行の差分 bitset を語単位の論理演算と減算で
更新する。復元版は DP 表を前処理した後、$L$ 要素の結果列を構築する。

## 注意点

- byte 版の引数は `std::string_view` であり、入力を変更しない。返すのは
  長さだけで、LCS 自体は復元しない。
- byte 版は入力の短い側を自動的に bitset 化するため、引数順によって
  計算量は変わらない。
- 汎用版の列長は `int` に収まる必要がある。
- 復元版が返す要素列は、入力中の相対順序を保つ。
