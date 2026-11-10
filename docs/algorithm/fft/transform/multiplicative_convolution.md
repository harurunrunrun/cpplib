---
title: Multiplicative Convolution (乗法を添字とする畳み込み)
documentation_of: ../../../../src/algorithm/fft/transform/multiplicative_convolution.hpp
---

添字の積を法として取る畳み込み $c_k = \sum_{ij \equiv k \pmod{L}} a_i b_j$ を、配列長 $L$ が素数の場合と 2 の冪の場合について計算する。

```cpp
template<int MOD = 998244353>
std::vector<Modint<MOD>> prime_multiplicative_convolution(
    const std::vector<Modint<MOD>>& a, const std::vector<Modint<MOD>>& b);

template<int MOD = 998244353>
std::vector<Modint<MOD>> power_two_multiplicative_convolution(
    const std::vector<Modint<MOD>>& a, const std::vector<Modint<MOD>>& b);
```

- `prime_multiplicative_convolution` は、配列長が素数 $p$ の場合を扱う。添字 $0$ の寄与は総和から直接求め、$1, \ldots, p - 1$ は原始根を底とする離散対数に置き換えて長さ $p - 1$ の巡回畳み込み（`arbitrary_mod_convolution`）を行う。
- `power_two_multiplicative_convolution` は、配列長が $2^n$ の場合を扱う。添字を 2 進付値と奇数部に分け、奇数部を単数群 $\lbrace \pm 1 \rbrace \times \langle 5 \rangle$ の符号と $5$ の冪の指数に分解して NTT で変換する。

## 時間計算量・空間計算量

配列長を $L$ として、時間 $O(L\log L)$、空間 $O(L)$ である。素数版では原始根の探索（$p - 1$ の試し割り）も行う。

## 注意点

- 両配列の長さは等しくなければならない。素数版で長さが素数でない場合、2 の冪版で長さが 2 の冪でない場合、または長さが異なる場合は `std::invalid_argument` を送出する。
- 2 の冪版では、係数の法 `MOD` が奇素数で、必要な長さ（$L/4$ まで）の NTT が存在する必要がある。長さ $1$ も扱える。
- 係数型・変換長には `convolution.hpp` と同じ制約がある。
