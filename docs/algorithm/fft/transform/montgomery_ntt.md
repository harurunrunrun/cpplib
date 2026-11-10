---
title: Montgomery NTT Convolution (Montgomery NTT による畳み込み)
documentation_of: ../../../../src/algorithm/fft/transform/montgomery_ntt.hpp
---

```cpp
template<int MOD>
std::vector<Modint<MOD>> fast_convolution_ntt(
    const std::vector<Modint<MOD>>& a,
    const std::vector<Modint<MOD>>& b
);
```

`std::vector<Modint<MOD>>` どうしの通常の畳み込み（長さ `a.size() + b.size() - 1`）を返す。空列との積は空列である。`MOD` は $3 \le \texttt{MOD} \lt 2^{30}$ の奇素数（`static_assert` で範囲と奇数性を検査する）で、使用する 2 の冪の変換長が `MOD - 1` を割り切る必要がある。割り切らなければ `std::length_error` を送出する。

## 時間計算量・空間計算量

内部表現を Montgomery 形式に変換し、bit reversal の後に radix-4 の NTT を行う。入力長を $N, M$ として、時間 $O((N+M)\log(N+M))$、空間 $O(N+M)$ である。短い方の列の長さが $32$ 以下なら、直接 $O(NM)$ で計算する。

通常の `Modint` との変換は関数の内部で行う。例えば、多項式を二乗した後で指定区間の係数を取り出す問題に使える。

## 注意点

- GCC 13 / C++20 を想定する。
- 内部関数 `montgomery_ntt_internal::transform` は、長さが 0、2 の冪でない、または `MOD - 1` を割り切らない場合に `std::invalid_argument` を送出する。
