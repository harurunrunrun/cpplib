---
title: Subset Convolution
documentation_of: ../src/algorithm/math/subset_convolution.hpp
---

## `subset_convolution`

```cpp
template<class T>
std::vector<T> subset_convolution(
    const std::vector<T>& left,
    const std::vector<T>& right
)
```

長さ $2^N$ の集合関数 $f,g$ に対するsubset convolution

$$
h(S)=\sum_{T\subseteq S}f(T)g(S\setminus T)
$$

を返す。bitmaskを部分集合として扱う。

入力長が異なる、空、または2冪でない場合は `invalid_argument` を投げる。
`T` はデフォルト構築した0、加算代入、減算代入、乗算を提供する可換環要素型とする。

要素を部分集合の要素数ごとに持ち上げ、ranked subset zeta transform、rank方向の
畳み込み、ranked Möbius transformを行う。2本のrank配列だけを保持し、積の配列には
左側の領域を再利用する。

- 時間計算量: $O(N^2 2^N)$
- 追加領域: $O(N2^N)$
