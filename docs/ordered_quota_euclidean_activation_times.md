---
title: Ordered Quota Euclidean Activation Times (優先順・上限付きユークリッド活性化時刻) [GONDOR]
documentation_of: ../src/algorithm/graph/ordered_quota_euclidean_activation_times.hpp
---

各頂点が活性化時に優先順から一定数の未活性頂点を選び、ユークリッド距離だけ
遅れて活性化eventを送る系の初回活性化時刻を計算します。

## API

### `OrderedQuotaEuclideanActivator`

```cpp
struct OrderedQuotaEuclideanActivator {
    long double x;
    long double y;
    int quota;
    vector<int> priority_order;
};
```

頂点の2次元座標、1回の活性化で送れるevent数の上限、ほかの全頂点を並べた
優先順を保持します。

### `ordered_quota_euclidean_activation_times(activators, initial_vertex)`

```cpp
vector<long double> ordered_quota_euclidean_activation_times(
    const vector<OrderedQuotaEuclideanActivator>& activators,
    int initial_vertex = 0
);
```

`initial_vertex` を時刻0で活性化します。ある頂点が初めて活性化すると、
その時点で未活性の頂点を `priority_order` の先頭から `quota` 個選びます。
選ばれた頂点には、2点間のユークリッド距離を伝播時間とするeventを送ります。

eventは `(到着時刻, 頂点番号)` の昇順で処理し、各頂点の最初のeventだけが
活性化を起こします。同じ未活性頂点へ複数のeventを予定できます。
到達不能な頂点の戻り値は正の無限大です。空入力には空vectorを返します。

## API別の時間計算量・空間計算量

頂点数を $N$、予定されるevent総数を $A$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `OrderedQuotaEuclideanActivator` の構築 | 優先順の長さに比例 | 優先順の長さに比例 |
| `ordered_quota_euclidean_activation_times` | $O(N^2+A\log(A+1))$ | $O(N^2+A)$ |

## 注意点

- 非空入力では `initial_vertex` が頂点範囲内でなければなりません。
- 座標は有限値、`quota` は非負でなければなりません。
- 各 `priority_order` は自分以外の全頂点をちょうど1回ずつ含めます。
- 前提違反には `invalid_argument`、`out_of_range` または
  `length_error` を送出します。
