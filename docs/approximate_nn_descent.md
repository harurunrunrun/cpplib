---
title: NN-Descent (近似近傍グラフ構築)
documentation_of: ../src/approximate/nearest_neighbor/nn_descent.hpp
---

## NN-Descent (近似近傍グラフ構築)

ランダム近傍グラフを、近傍の近傍と逆辺候補で反復改善する近似k近傍グラフである。点は `std::array<Real, Dimension>`。固定seedの乱数器と距離・添字によるtie-breakにより結果を再現できる。
返値の要素型は `AnnNeighbor` で、`index` は登録時の添字、`squared_distance` は `long double` の二乗距離である。

## API

### `Point`

`NnDescent<Real, Dimension>::Point` は `std::array<Real, Dimension>` の公開型別名である。

### `NnDescent()`

点を持たないgraphを作る。`size()=0`, `neighbor_count()=0` となる。時間計算量・追加空間計算量は `O(1)`。

### `NnDescent(points, neighbor_count, max_iterations, candidate_limit, random)` / `reset(...)`

グラフを構築する。`N>=2` では `1 <= neighbor_count < N`、正の反復上限、`candidate_limit >= neighbor_count` が必要。空または1点では `neighbor_count=0` とする。非有限座標を拒否し、重複点を許す。

`K=neighbor_count`, `L=candidate_limit`, `I=max_iterations` とする。時間計算量は期待 `O(ND+NK(D+log(K+1))+I(1+NK^2 log(N+K^2+1)+N(L+K)(D+log(L+K+1))))`、構築後のgraphを含む追加空間計算量は `O(1+N(D+K)+K^2+L)`。

### `size()`, `neighbor_count()`

属性を返す。時間・追加空間計算量は `O(1)`。

### `point(index)`

点を返す。不正添字では例外。時間・追加空間計算量は `O(1)`。

### `neighbors(index)`

構築済み近傍を真の二乗距離、添字順で返す。添字は重複せず、自身を含まない。不正添字では例外。時間・追加空間計算量は `O(1)`。

### `nearest(query, k, search_budget, exact_fallback=false)`

グラフをbest-firstに辿り、ちょうど `B=min(search_budget,N)` 点との距離を評価する。`1<=k<=N`, `search_budget>=k` が必要。非有限queryを拒否し、距離が `long double` の範囲を超える場合は `std::overflow_error` を送出する。連結成分を使い切った場合は最小未訪問添字から探索を再開する。

近似時の期待時間は `O(B(D+K+log B))`、追加空間は `O(B)`。`B=N` なら全点を評価するのでexactとなる。`exact_fallback=true` も全点走査し、時間 `O(ND+N log N)`、追加空間 `O(N)`。

## 注意点

登録点とqueryの座標は有限で、平方距離を `long double` で表現できなければならない。厳密探索を明示した場合を除き、探索budgetに対するrecallや近似比は保証しない。
