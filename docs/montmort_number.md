---
title: Montmort Number (モンモール数) [montmort_number_mod]
documentation_of: ../src/algorithm/math/combinatorics/montmort_number.hpp
---

## `montmort_numbers`

```cpp
std::vector<std::uint32_t> montmort_numbers(
    std::size_t maximum_size,
    std::uint32_t modulus
)
```

`result[k]` に、$k$ 要素の撹乱順列（不動点を持たない順列）の個数を
`modulus` で割った余りを格納して返す。返却列の長さは `maximum_size + 1` で、
$D_0=1,D_1=0$ と

$$
D_n=(n-1)(D_{n-1}+D_{n-2})
$$

を使う。`modulus == 0` なら `invalid_argument` を投げる。

- 時間計算量: $O(N)$
- 空間計算量（追加領域）: 返却列を除いて $O(1)$、返却列を含めて $O(N)$

## 注意点

法0、非可逆な除算、入力size、整数overflowの扱いは各APIの説明に従う。中間値は明記した内部拡張型または要素型で表現できなければならない。
