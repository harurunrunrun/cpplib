---
title: Divide-and-Conquer DP Optimization (分割統治DP最適化)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/divide_and_conquer_dp_optimization.hpp
---

遷移

$$
\mathrm{next}[i]=\min_{0\le j<i}\{\mathrm{previous}[j]+\mathrm{cost}(j,i)\}
$$

の最適な $j$ が $i$ に関して単調な場合に、1層を分割統治で計算する。

## API

```cpp
template<class Value, class Cost>
DivideAndConquerDPResult<Value> divide_and_conquer_dp_layer(
    const vector<Value>& previous,
    Cost cost,
    Value infinity
);
```

`value[i]` と最左の最適遷移元 `optimum[i]` を返す。添字 `0` は遷移対象外で、`infinity` と `-1` のままである。

## 時間計算量

$N=previous.size()-1$ として、`cost` 評価が $O(1)$ なら $O(N\log N)$ 時間、$O(N+\log N)$ 空間。

## 注意点

最適遷移元の単調性を呼出側が保証する。空の `previous` では `runtime_error` を送出する。加算が `Value` の範囲内であることを前提とする。
