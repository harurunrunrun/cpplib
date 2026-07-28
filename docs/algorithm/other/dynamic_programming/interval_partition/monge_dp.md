---
title: Monge DP (Monge型DP最適化)
documentation_of: ../../../../../src/algorithm/other/dynamic_programming/interval_partition/monge_dp.hpp
---

遷移行列 `previous[j] + cost(j, i)` がtotally monotoneとなるDPの1層をSMAWK法で計算する。

## API

```cpp
template<class Value, class Cost>
DivideAndConquerDPResult<Value> monge_dp_layer(
    const vector<Value>& previous,
    Cost cost,
    Value infinity
);
```

各終点の最小値と最左の最適遷移元を返す。`j >= i` は `infinity` として扱う。

## 時間計算量

$N=previous.size()-1$ として、`cost` 評価が $O(1)$ ならSMAWK法により $O(N)$ 時間、$O(N)$ 空間。

## 注意点

三角遷移行列を含めてtotally monotoneであることを呼出側が保証する。空入力では `runtime_error` を送出する。
