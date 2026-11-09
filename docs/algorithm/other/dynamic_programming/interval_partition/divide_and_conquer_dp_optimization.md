---
title: Divide-and-Conquer DP Optimization (分割統治DP最適化)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/divide_and_conquer_dp_optimization.hpp
---

遷移

$$
\mathrm{next}[i]=\min_{0\le j<i}\{\mathrm{previous}[j]+\mathrm{cost}(j,i)\}
$$

の最適な $j$ が $i$ に関して単調非減少である場合に、1 層分を分割統治で計算する。

## API

```cpp
template<class Value, class Cost>
DivideAndConquerDPResult<Value> divide_and_conquer_dp_layer(
    const std::vector<Value>& previous,
    Cost cost,
    Value infinity
);
```

`value[i]` と、最適な遷移元のうち最も左のもの `optimum[i]` を返す。添字 $0$ は遷移の対象外で、`infinity` と `-1` のままである。

## 時間計算量

$N$ を `previous.size() - 1` として、`cost` の評価が $O(1)$ なら $O(N\log N)$ である。

## 空間計算量

$O(N)$（再帰の深さ $O(\log N)$ を含む）。

## 注意点

- 最適な遷移元の単調性は呼び出し側が保証する。
- 空の `previous` では `std::runtime_error` を送出する。
- `previous[j] + cost(j, i)` の加算が `Value` の範囲に収まることを前提とする（`previous` に `infinity` が含まれる場合も加算される）。
