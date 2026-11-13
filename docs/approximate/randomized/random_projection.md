---
title: Random Projection / Johnson–Lindenstrauss Transform (ランダム射影・ジョンソン–リンデンシュトラウス変換)
documentation_of: ../../../src/approximate/randomized/random_projection.hpp
---

各係数を独立な $\pm 1/\sqrt{k}$ とする密な Rademacher 射影を提供する。以下、入力次元を $d$、出力次元を $k$、点数を $n$ とする。

## `rademacher_projection_matrix`

```cpp
auto projection = rademacher_projection_matrix(input_dimension, output_dimension, random);
auto fixed = rademacher_projection_matrix<InputDimension, OutputDimension>(random);
```

$k \times d$ の射影行列を生成する。`vector` 版と固定長 `array` 版がある。
時間計算量・返値の空間計算量はともに $O(kd)$。`output_dimension == 0` なら `std::invalid_argument` を送出する（`array` 版では `static_assert`）。

## `apply_random_projection`

```cpp
auto projected = apply_random_projection(points, projection);
```

各点に射影行列を掛ける。`vector` 版と固定長 `array` 版がある。
時間計算量は $O(nkd)$、返値の空間計算量は $O(nk)$、計算中の追加空間計算量は $O(1)$。
行列が空または矩形でない場合、次元が一致しない場合、非有限値を含む場合は `std::invalid_argument` を、結果が `double` に収まらない場合は `std::overflow_error` を送出する。

## `johnson_lindenstrauss_transform`

```cpp
auto projected = johnson_lindenstrauss_transform(points, output_dimension, random);
```

射影行列を生成し、`vector` の点列へ適用する。時間計算量は $O(kd + nkd)$、返値と一時的な射影行列を合わせた追加空間計算量は $O(kd + nk)$。空の点列も許す。

## `johnson_lindenstrauss_dimension`

```cpp
auto k = johnson_lindenstrauss_dimension(point_count, epsilon, failure_probability);
```

全点対の距離の 2 乗を倍率 $1 \pm \epsilon$ の範囲に保てない確率を `failure_probability`（$\delta$）以下にする、保守的な次元

$$
\left\lceil\frac{\log(n(n-1)/\delta)}
{\epsilon^2/4-\epsilon^3/6}\right\rceil
$$

を返す。時間計算量・追加空間計算量は $O(1)$。`point_count < 2` なら 1 を返す。

## 注意点

- `johnson_lindenstrauss_dimension` には `0 < epsilon < 1` かつ `0 < failure_probability < 1` が必要で、違反時は `std::invalid_argument` を、返す次元がオーバーフローする場合は `std::overflow_error` を送出する。
- 次元の保証は、各符号を独立に生成した場合に成り立つ。
- 同じ結果を再現するには、同じ乱数生成器の状態・入力・標準ライブラリ実装が必要である。
