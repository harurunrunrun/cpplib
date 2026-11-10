---
title: Number of Distinct Subsequences (異なる部分列の個数) [number_of_subsequences]
documentation_of: ../../../../src/algorithm/math/combinatorics/number_of_subsequences.hpp
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

`values` の空でない相異なる部分列の個数を `modulus` で割った余りとして返す。要素のハッシュ関数と等価判定は `Hash`、`Equal` で指定できる。

現在位置までの（空列を含む）相異なる部分列の個数を $d$ とし、各要素で $d$ を 2 倍してから、同じ値が前回現れた直前の $d$ を差し引く DP を行う。最後に空列の分の $1$ を引いて返す。

$N$ を `values` の長さ、$K$ を相異なる値の個数とする。

- 平均時間計算量: $O(N)$
- 空間計算量（追加領域）: $O(K)$（ただし `reserve` により $N$ 要素分のバケットを確保する）

計算量は `std::unordered_map` のハッシュ操作が平均 $O(1)$ であることを仮定する。

## 注意点

- `modulus == 0` なら `invalid_argument` を送出する。
- 中間値は `uint64_t` で計算するため、`modulus` が `uint32_t` の全域でもオーバーフローしない。除算は使わないので法は素数でなくてよい。
