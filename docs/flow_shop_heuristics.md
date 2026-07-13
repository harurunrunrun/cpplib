---
title: Constructive Flow-Shop Heuristics (構築型フローショップヒューリスティック)
documentation_of: ../src/approximate/scheduling/flow_shop_heuristics.hpp
---

`processing_time[job][machine]` で表す permutation flow shop の構成 heuristic。
返り値は全 job の置換であり、同点は job 番号順である。

## `cds_flow_shop_order`

```cpp
auto order = cds_flow_shop_order(processing_time);
```

$k=1,\ldots,M-1$ ごとに先頭 $k$ 機械と末尾 $k$ 機械を二つの仮想機械へ集約し、
Johnson 法の順序のうち元の flow shop で makespan が最小のものを返す CDS 法。
64-bit 以下の整数では集約値を128-bitで厳密に計算し、それ以外では overflow を
避けるため順序を変えない平均値で計算する。
時間 $O(NM^2+MN\log N)$、追加領域 $O(N+M)$。1機械では SPT 順、0機械では
job 番号順を返す。

## `palmer_flow_shop_order`

```cpp
auto order = palmer_flow_shop_order(processing_time);
```

機械位置の線形重みを処理時間へ掛けた slope index の降順を返す Palmer 法。
64-bit 以下の整数では、機械数が $2^{30}$ 以下なら128-bitで厳密に比較する。
それ以外では全 job 共通の最大処理時間と機械数で score を正規化し、積和 overflow を
避ける。
時間 $O(NM+N\log N)$、追加領域 $O(N)$。

## `gupta_flow_shop_order`

```cpp
auto order = gupta_flow_shop_order(processing_time);
```

先頭機械が末尾機械より短い job を前群、それ以外を後群とし、隣接機械の処理時間和の
最小値から Gupta index を構成する。前群は最小値の昇順、後群は降順で並べる。
時間 $O(NM+N\log N)$、追加領域 $O(N)$。

いずれも一般の $M$ 機械 flow shop に対する近似比を保証しない。空入力を受け付ける。
非長方形行列、負または非有限な処理時間では `std::invalid_argument`。CDS の makespan
が全候補で `Time` に収まらない場合は `std::overflow_error`。溢れる候補は選択対象から
除外する。
