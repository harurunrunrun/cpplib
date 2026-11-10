---
title: Montmort Number (モンモール数) [montmort_number_mod]
documentation_of: ../../../../src/algorithm/math/combinatorics/montmort_number.hpp
---

## `montmort_numbers`

```cpp
std::vector<std::uint32_t> montmort_numbers(
    std::size_t maximum_size,
    std::uint32_t modulus
)
```

`result[k]` に $k$ 要素の撹乱順列（不動点を持たない順列）の個数 $D_k$ を `modulus` で割った余りを格納して返す。返り値の長さは `maximum_size + 1` で、$D_0 = 1, D_1 = 0$ と漸化式

$$
D_n=(n-1)(D_{n-1}+D_{n-2})
$$

を使う。

$N$ を `maximum_size` とする。

- 時間計算量: $O(N)$
- 空間計算量（追加領域）: 返り値を除いて $O(1)$、返り値を含めて $O(N)$

## 注意点

- `modulus == 0` なら `invalid_argument` を送出する。`modulus == 1` なら全要素が $0$ になる。
- 中間値は `uint64_t` で計算するため、`modulus` が `uint32_t` の全域でもオーバーフローしない。除算は使わないので法は素数でなくてよい。
