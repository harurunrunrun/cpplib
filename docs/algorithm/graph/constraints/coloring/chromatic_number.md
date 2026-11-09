---
title: Chromatic Number (彩色数) [chromatic_number]
documentation_of: ../../../../../src/algorithm/graph/constraints/coloring/chromatic_number.hpp
---

無向グラフの彩色数を求める。

## `chromatic_number`

```cpp
int answer = chromatic_number(graph);
```

`graph[v]` には頂点 `v` に隣接する頂点を入れる。隣接関係は対称（無向グラフ）でなければならない。自己ループは無視する。頂点数 0 のグラフでは 0 を返す。

各頂点部分集合 $S$ に含まれる独立集合の個数 $I(S)$ を部分集合 DP で数え、彩色多項式

$$
P_G(k) = \sum_{S \subseteq V} (-1)^{N - |S|} I(S)^k
$$

を $k = 1, 2, \ldots$ の順に 3 個の素数を法として評価し、初めて 0 でなくなる $k$ を返す。$N \le 20$ では $0 \le P_G(k) \le 20^{20}$ であり、3 つの法の積がこの上限を超えるため、確率的な衝突のない決定的な判定になる。

## API別の時間計算量・空間計算量

$N$ を頂点数、$M$ を隣接リストの総要素数、答えを $K$ とする。

| API・操作 | 時間計算量 | 空間計算量（追加領域） |
| --- | --- | --- |
| 入力検査 | $O(N^2 + M)$ | $O(N)$ |
| 独立集合の個数の前計算 | $O(2^N)$ | $O(2^N)$ |
| 彩色多項式の評価 | $O(K 2^N)$ | $O(2^N)$ |
| `chromatic_number` 全体 | $O(N^2 + M + K 2^N)$ | $O(2^N + N)$ |
| 戻り値の参照 | $O(1)$ | $O(1)$ |

## 注意点

実装上 $N \le 20$ でなければならない。頂点数の超過、範囲外の隣接頂点、非対称な辺に対しては `std::runtime_error` を送出する。
