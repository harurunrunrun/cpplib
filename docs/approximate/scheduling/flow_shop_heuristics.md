---
title: Constructive Flow-Shop Heuristics (構築型フローショップヒューリスティック)
documentation_of: ../../../src/approximate/scheduling/flow_shop_heuristics.hpp
---

`processing_time[job][machine]` で表す順列フローショップに対する構築型ヒューリスティックである。
返値は全ジョブの順列で、同点はジョブ番号順とする。ジョブ数を $N$、機械数を $M$ とする。

## `cds_flow_shop_order`

```cpp
auto order = cds_flow_shop_order(processing_time);
```

$k = 1, \ldots, M-1$ のそれぞれについて先頭 $k$ 機械と末尾 $k$ 機械を 2 つの仮想機械に集約して Johnson 法の順序を作り、その中で元のフローショップでの makespan が最小のものを返す CDS 法である。
64 bit 以下の整数型では集約値を 128 bit で厳密に計算し、それ以外の型ではオーバーフローを避けるため、順序を変えない平均値で計算する。
時間計算量は $O(NM^2 + MN \log N)$、追加空間計算量は $O(N + M)$。機械が 1 台なら SPT 順、0 台ならジョブ番号順を返す。

## `palmer_flow_shop_order`

```cpp
auto order = palmer_flow_shop_order(processing_time);
```

機械の位置に応じた線形の重みを処理時間に掛けた slope index の降順を返す Palmer 法である。
64 bit 以下の整数型では、機械数が $2^{30}$ 以下なら 128 bit で厳密に比較する。それ以外では、全ジョブ共通の最大処理時間と機械数で評価値を正規化し、積和のオーバーフローを避ける。
時間計算量は $O(NM + N \log N)$、追加空間計算量は $O(N)$。

## `gupta_flow_shop_order`

```cpp
auto order = gupta_flow_shop_order(processing_time);
```

先頭の機械の処理時間が末尾の機械より短いジョブを前群、それ以外を後群とし、隣接する機械の処理時間の和の最小値から Gupta index を作る。前群は index の昇順、後群は降順に並べる。
時間計算量は $O(NM + N \log N)$、追加空間計算量は $O(N)$。

## 注意点

- いずれも一般の $M$ 機械フローショップに対する近似比は保証しない。空の入力も受け付ける。
- 長方形でない行列、負または有限でない処理時間に対しては `std::invalid_argument` を送出する。
- CDS で makespan がオーバーフローする候補は選択対象から除き、すべての候補がオーバーフローする場合は `std::overflow_error` を送出する。
