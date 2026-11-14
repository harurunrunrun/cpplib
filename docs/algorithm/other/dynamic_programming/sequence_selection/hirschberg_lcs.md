---
title: Hirschberg Algorithm (Hirschberg法)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/sequence_selection/hirschberg_lcs.hpp
---

最長共通部分列（LCS）を、DP 表全体を保持せずに分割統治（Hirschberg 法）で復元する。

## API

```cpp
template<class Value>
std::vector<Value> hirschberg_lcs(
    const std::vector<Value>& first,
    const std::vector<Value>& second
);
```

最長共通部分列を 1 つ返す。最長のものが複数ある場合にどれを返すかは、分割位置の選び方に依存する。

## 時間計算量

`first` の長さを $N$、`second` の長さを $M$、LCS の長さを $L$ として $O(NM)$ 時間である。

## 空間計算量

各再帰段で長さ $O(M)$ の作業列を 2 本確保するが、分割位置を決めた時点で解放してから子の再帰に入るため、同時に保持する作業列は $O(M)$ である。再帰の深さは $O(\log N)$ で、作業領域は $O(M+\log N)$、戻り値を含めると $O(M+\log N+L)$。

## 注意点

- `Value` は等値比較とコピーが可能であることを前提とする。
- 作業列の長さは第 2 引数の長さに比例するので、短い方の列を第 2 引数にすると補助空間を減らせる。
- 長さは `int` で数える。
