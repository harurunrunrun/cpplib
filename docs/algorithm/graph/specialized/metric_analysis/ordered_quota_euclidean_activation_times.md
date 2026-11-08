---
title: Ordered Quota Euclidean Activation Times (優先順・上限付きユークリッド活性化時刻) [GONDOR]
documentation_of: ../../../../../src/algorithm/graph/specialized/metric_analysis/ordered_quota_euclidean_activation_times.hpp
---

各頂点が活性化したときに、優先順に従って一定数の未活性頂点を選び、ユークリッド距離だけ遅れて届く活性化イベントを送る系について、各頂点が初めて活性化する時刻を計算する。

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

頂点の 2 次元座標、1 回の活性化で送れるイベント数の上限、および自分以外の全頂点を並べた優先順を保持する。

### `ordered_quota_euclidean_activation_times(activators, initial_vertex)`

```cpp
vector<long double> ordered_quota_euclidean_activation_times(
    const vector<OrderedQuotaEuclideanActivator>& activators,
    int initial_vertex = 0
);
```

`initial_vertex` を時刻 $0$ で活性化する。ある頂点が初めて活性化すると、その時点で未活性の頂点を `priority_order` の先頭から `quota` 個選び、選んだ各頂点へ 2 点間のユークリッド距離を伝播時間とするイベントを送る。

イベントは `(到着時刻, 頂点番号)` の昇順で処理し、各頂点に最初に届いたイベントだけが活性化を起こす。同じ未活性頂点へ複数のイベントが予定されることもある。
到達不能な頂点の戻り値は正の無限大である。空の入力には空の `vector` を返す。

## API別の時間計算量・空間計算量

頂点数を $N$、予定されるイベントの総数を $A$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `OrderedQuotaEuclideanActivator` の構築 | 優先順の長さに比例 | 優先順の長さに比例 |
| `ordered_quota_euclidean_activation_times` | $O(N^2+A\log(A+1))$ | $O(N^2+A)$ |

## 注意点

- 空でない入力では、`initial_vertex` が頂点番号の範囲内でなければならない（範囲外なら `out_of_range`）。
- 座標は有限値、`quota` は非負でなければならない。
- 各 `priority_order` は自分以外の全頂点をちょうど 1 回ずつ含まなければならない。
- 前提違反には `invalid_argument`、`out_of_range` または `length_error` を送出する。
