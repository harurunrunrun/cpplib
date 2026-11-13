---
title: Flow-Shop Scheduling (フローショップスケジューリング)
documentation_of: ../../../src/approximate/scheduling/flow_shop.hpp
---

`processing_time[job][machine]` で表す順列フローショップを扱う。ジョブ数を $N$、機械数を $M$ とする。

## `flow_shop_makespan`

```cpp
Time value = flow_shop_makespan(processing_time, order);
```

重複のないジョブ列 `order` を全機械で同じ順に処理したときの makespan（最大完了時刻）を返す。一部のジョブだけの列も受け付ける。
列の長さを $K$ とすると、入力行列の検査を含めて時間計算量は $O(NM + KM)$、追加空間計算量は $O(N + M)$。

## `johnson_two_machine_order`

```cpp
auto order = johnson_two_machine_order(first_machine, second_machine);
```

2 機械フローショップの最適な順序を Johnson 法で返す。同値ならジョブ番号順である。
時間計算量は $O(N \log N)$、追加空間計算量は $O(N)$。

## `neh_flow_shop_order`

```cpp
auto order = neh_flow_shop_order(processing_time);
```

総処理時間の降順にジョブを取り出し、現在の列のすべての位置へ試しに挿入して、makespan が最小となる位置を選ぶ NEH ヒューリスティックである。同値なら前の位置を選ぶ。
時間計算量は $O(N^3 M)$、追加空間計算量は $O(N + M)$。

## 注意点

- 長方形でない行列、負の処理時間、不正なジョブ列、2 機械入力の長さの不一致に対しては `std::invalid_argument` を送出する。
- 時刻の加算結果は `Time` に収まらなければならない。
