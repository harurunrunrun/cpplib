---
title: Number of Distinct Subsequences (異なる部分列の個数)
documentation_of: ../src/algorithm/math/number_of_subsequences.hpp
---

## `number_of_distinct_subsequences`

```cpp
template<class T, class Hash = std::hash<T>, class Equal = std::equal_to<T>>
std::uint32_t number_of_distinct_subsequences(
    const std::vector<T>& values,
    std::uint32_t modulus,
    const Hash& hash = Hash(),
    const Equal& equal = Equal()
)
```

`values` の空でない相異なる部分列の個数を `modulus` で割った余りとして返す。
要素の等価性とhashは `Equal`、`Hash` で指定できる。`modulus == 0` なら
`invalid_argument` を投げる。

現在位置までの空列を含む個数を $d$ とし、同じ値が前回現れる直前の $d$ を
差し引くDPを行う。

- 平均時間計算量: $O(N)$
- 追加領域: $O(K)$（$K$ は相異なる値の個数）

計算量は `std::unordered_map` のhash操作が平均 $O(1)$ であることを仮定する。
