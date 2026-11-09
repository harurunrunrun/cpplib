---
title: Subset Convolution (部分集合畳み込み) [subset_convolution]
documentation_of: ../../../../src/algorithm/math/polynomial/subset_convolution.hpp
---

## `subset_convolution`

```cpp
template<class T>
std::vector<T> subset_convolution(
    const std::vector<T>& left,
    const std::vector<T>& right
)
```

長さ $2^N$ の集合関数 $f, g$ に対する部分集合畳み込み（subset convolution）

$$
h(S)=\sum_{T\subseteq S}f(T)g(S\setminus T)
$$

を返す。ビットマスクを部分集合として扱う。

入力の長さが異なる場合、空の場合、または 2 の冪でない場合は `std::invalid_argument` を送出する。
`T` は、デフォルト構築で $0$ になり、加算代入・減算代入・乗算を提供する可換環の要素型とする。

要素を部分集合の要素数（ランク）ごとに持ち上げ、ランク付き zeta 変換、ランク方向の畳み込み、ランク付き Möbius 変換を行う。ランク付き配列は 2 本だけ保持し、積の配列には左側の領域を再利用する。

- 時間計算量: $O(N^2 2^N)$
- 空間計算量（追加領域）: $O(N 2^N)$

## 注意点

中間値は要素型 `T` で表現できなければならない。整数型を使う場合のオーバーフローは補償しない。
