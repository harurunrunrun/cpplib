---
title: DBSCAN / Mean Shift
documentation_of: ../src/approximate/clustering/density_clustering.hpp
---

`approximate::clustering`名前空間で、密な実数ベクトルに対する密度ベースの
クラスタリングを提供する。全点の次元は正で等しく、座標は有限でなければならない。

## `DBSCANResult`

- `labels[i]`: 点`i`のcluster番号。noiseは`-1`。
- `cluster_count`: 発見したcluster数。

## `dbscan`

```cpp
auto result = dbscan(points, epsilon, minimum_points);
```

点自身を含む閉球（距離`<= epsilon`）内に`minimum_points`個以上ある点を
core pointとしてDBSCANを行う。点を添字順、近傍も添字順に調べるため結果は
決定的で、cluster番号は最初のcore pointの発見順になる。空の点集合も受理する。
点数を$n$、次元を$d$とすると、時間$O(n^2d)$、追加領域$O(n^2)$。

## `MeanShiftResult`

- `modes`: 収束位置を`merge_radius`で連結成分に併合したmode。
- `labels[i]`: 点`i`のmode番号。
- `iterations[i]`: 点`i`のtrajectoryで実行した更新回数。

## `mean_shift`

```cpp
auto result = mean_shift(points, bandwidth, maximum_iterations,
                         tolerance, merge_radius);
```

一様kernelを使い、各入力点から独立にmean shiftを行う。移動距離が
`tolerance`以下になると停止する。負の`merge_radius`なら`tolerance`を使う。
点数$n$、次元$d$、最大反復数$I$に対し、時間$O(In^2d+n^2d)$、
追加領域$O(nd+n)$。

非有限値、負の`epsilon`・`tolerance`、`minimum_points == 0`、不揃いの次元では
`std::invalid_argument`を送出する。Mean Shiftの`bandwidth`は正でなければならず、
負の`merge_radius`だけは`tolerance`を指定したものとして扱う。点数が`int`の
cluster labelで表せない場合、DBSCANは`std::length_error`を送出する。
