---
title: Random Projection / Johnson–Lindenstrauss Transform (ランダム射影・ジョンソン–リンデンシュトラウス変換)
documentation_of: ../src/approximate/randomized/random_projection.hpp
---

各係数を独立な $\pm1/\sqrt{k}$ とするdense Rademacher射影を提供する。

## `rademacher_projection_matrix`

```cpp
auto projection = rademacher_projection_matrix(input_dimension, output_dimension, random);
auto fixed = rademacher_projection_matrix<InputDimension, OutputDimension>(random);
```

$k\times d$の射影行列を生成する。`vector`版と固定長`array`版がある。
時間・返却領域ともに$O(kd)$。`output_dimension==0`は`std::invalid_argument`。

## `apply_random_projection`

```cpp
auto projected = apply_random_projection(points, projection);
```

各点に射影行列を掛ける。`vector`版と固定長`array`版がある。点数を$n$とすると
時間計算量は $O(nkd)$、返却領域$O(nk)$、計算中の追加空間計算量は $O(1)$。非矩形行列、次元不一致、
非有限入力は`std::invalid_argument`、結果が`double`に収まらない場合は
`std::overflow_error`となる。

## `johnson_lindenstrauss_transform`

```cpp
auto projected = johnson_lindenstrauss_transform(points, output_dimension, random);
```

射影行列を生成して`vector`の点列へ適用する。時間計算量は $O(kd+nkd)$、
返却値と一時射影行列を合わせた追加空間計算量は $O(kd+nk)$。空の点列も許す。

## `johnson_lindenstrauss_dimension`

```cpp
auto k = johnson_lindenstrauss_dimension(point_count, epsilon, failure_probability);
```

全点対の平方距離を倍率$1\pm\epsilon$に保つ失敗確率を
`failure_probability`以下にする保守的な次元

$$
\left\lceil\frac{\log(n(n-1)/\delta)}
{\epsilon^2/4-\epsilon^3/6}\right\rceil
$$

を返す。時間計算量・追加空間計算量は $O(1)$。`point_count < 2`なら1を返す。
`0 < epsilon < 1`かつ`0 < failure_probability < 1`が必要で、違反は
`std::invalid_argument`、返却次元のoverflowは`std::overflow_error`となる。
この保証は各符号を独立に生成した場合に成り立つ。

## 注意点

確率的保証は各APIで示した独立性と入力条件の下で成り立つ。同じ結果の再現には同じ乱数器状態・入力・標準ライブラリ実装が必要で、中間演算は使用型の表現範囲内でなければならない。
