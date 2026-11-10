---
title: Monge DP (Monge型DP最適化)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/monge_dp.hpp
---

遷移行列 `previous[j] + cost(j, i)` が totally monotone となる DP の 1 層分を、SMAWK 法で計算する。

## API

```cpp
template<class Value, class Cost>
DivideAndConquerDPResult<Value> monge_dp_layer(
    const std::vector<Value>& previous,
    Cost cost,
    Value infinity
);
```

各終点 $i \ge 1$ について、最小値 `value[i]` と最も左の最適な遷移元 `optimum[i]` を返す（戻り値の型は `divide_and_conquer_dp_optimization.hpp` の `DivideAndConquerDPResult`）。`j >= i` の要素は `infinity` として扱う。添字 $0$ は `infinity` と `-1` のままである。

## 時間計算量

$N$ を `previous.size() - 1` として、`cost` の評価が $O(1)$ なら SMAWK 法により $O(N)$ 時間、$O(N)$ 空間である。

## 注意点

- `infinity` で埋めた三角部分を含めて、遷移行列が totally monotone であることを呼び出し側が保証する。
- 空の `previous` では `std::runtime_error` を送出する。
