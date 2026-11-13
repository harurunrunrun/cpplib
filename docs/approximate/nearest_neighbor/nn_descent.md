---
title: NN-Descent (近似近傍グラフ構築)
documentation_of: ../../../src/approximate/nearest_neighbor/nn_descent.hpp
---

ランダムな近傍グラフを、近傍の近傍と逆辺の候補で反復的に改善して作る近似 $k$ 近傍グラフである。点は `std::array<Real, Dimension>` である。シードを固定した乱数生成器と、距離・添字による同点の扱いにより、結果を再現できる。返り値の要素型は `AnnNeighbor` で、`index` は登録時の添字、`squared_distance` は `long double` の二乗距離である。

以下、点数を $N$、次元を $D$ とする。

## API

### `Point`

`NnDescent<Real, Dimension>::Point` は `std::array<Real, Dimension>` の公開型別名である。

### `NnDescent()`

点を持たないグラフを作る。`size() == 0`, `neighbor_count() == 0` となる。時間計算量・追加空間計算量は $O(1)$ である。

### `NnDescent(points, neighbor_count, iterations, candidate_limit, random)` / `reset(...)`

グラフを構築する。`iterations` と `candidate_limit` は正、`candidate_limit >= neighbor_count` でなければならない。$N \ge 2$ では `1 <= neighbor_count < N`、空または 1 点では `neighbor_count == 0` でなければならない。非有限な座標は拒否し、重複点は許す。

$K = \texttt{neighbor_count}$, $L = \texttt{candidate_limit}$, $I = \texttt{iterations}$ とする。時間計算量は期待 $O(ND+NK(D+\log(K+1))+I(1+NK^2 \log(N+K^2+1)+N(L+K)(D+\log(L+K+1))))$、構築後のグラフを含む追加空間計算量は $O(1+N(D+K)+K^2+L)$ である。

### `size()`, `neighbor_count()`

属性を返す。時間計算量・追加空間計算量は $O(1)$ である。

### `point(index)`

点を返す。時間計算量・追加空間計算量は $O(1)$ である。

### `neighbors(index)`

構築済みの近傍を、真の二乗距離・添字の順で返す。添字は重複せず、自身を含まない。時間計算量・追加空間計算量は $O(1)$ である。

### `nearest(query, k, search_budget, exact_fallback = false)`

グラフを best-first にたどり、ちょうど $B=\min(\texttt{search_budget}, N)$ 点との距離を評価する。`1 <= k <= N` かつ `search_budget >= k` でなければならない。連結成分を使い切った場合は、未訪問の最小の添字から探索を再開する。

近似探索時の期待時間計算量は $O(B(D+K+\log B))$、追加空間計算量は $O(B)$ である。$B=N$ なら全点を評価するので厳密になる。`exact_fallback = true` も全点を走査し、時間 $O(ND+N \log N)$、追加空間 $O(N)$ である。

## 注意点

- パラメータ・`k`・`search_budget` が上記の条件を満たさない場合や、非有限な座標・クエリには `std::invalid_argument` を送出する
- 二乗距離が `long double` の範囲を超える場合は `std::overflow_error` を送出する
- `point` と `neighbors` は範囲外の添字に例外を送出する
- 厳密探索になる場合を除き、探索予算に対する再現率や近似比は保証しない
