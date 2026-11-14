---
title: LLL Lattice Basis Reduction (LLL格子基底簡約)
documentation_of: ../../../../../src/algorithm/math/linear_algebra/lattice/lll_lattice_reduction.hpp
---

整数格子の基底を、size-reduced かつ Lovász 条件を満たす基底（LLL 簡約基底）に変換する。API は名前空間 `linear_algebra` にある。

## `LllLatticeReductionResult`

```cpp
struct LllLatticeReductionResult {
    std::vector<std::vector<long long>> basis;
    std::vector<std::vector<long long>> transformation;
    std::size_t swaps = 0;
};
```

`basis` は簡約後の基底、`transformation` は `basis = transformation * input` を満たす整数行列、`swaps` は Lovász 条件による交換の回数である。

## `lll_lattice_reduction(basis, delta = 0.75)`

```cpp
LllLatticeReductionResult lll_lattice_reduction(
    std::vector<std::vector<long long>> basis, long double delta = 0.75L);
```

各行を基底ベクトルとして LLL 簡約を行う。Gram-Schmidt 直交化は `long double` で計算する。

## 時間計算量

基底の本数を $b$、次元を $d$、size reduction と交換の実行回数を $I$ とする。この実装は size reduction・交換のたびに Gram-Schmidt 直交化を明示的に再計算する（変更した行より前の行は結果が変わらないので、変更した行以降だけを再計算する）ため、時間計算量は $O(Ib^2d)$、空間計算量は $O(b^2+bd)$ である。固定した `delta < 1` では、$I$ は入力の bit 長に対して多項式で抑えられる。

## 注意点

- 基底は一次独立で、全行の次元が等しくなければならない。`delta` は $(1/4, 1)$ の範囲になければならない。不正な入力には `std::invalid_argument` を送出する。
- 整数の基本変形が `long long` の範囲を超える場合は `std::overflow_error` を送出する。
- Gram-Schmidt 係数は `long double` の浮動小数点数で計算するため、成分が非常に大きい（おおよそ $2^{32}$ を超える）入力や高次元では丸め誤差により結果が LLL 条件を厳密に満たさないことがある。
