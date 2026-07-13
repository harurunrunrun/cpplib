---
title: HNSW (階層的近似近傍探索)
documentation_of: ../src/approximate/nearest_neighbor/hnsw.hpp
---

# HNSW (階層的近似近傍探索)

確率的な階層近傍グラフで近似最近傍を探索する。点は `std::array<Real, Dimension>`。同じseed、挿入順、引数なら結果は決定的で、同距離は添字順に処理する。重複点を許す。

近似探索に精度保証はない。`ef_search` を増やすと通常recallと計算量が増える。`exact_fallback=true` は階層を使わず全点を走査する。
返値の要素型は `AnnNeighbor` で、`index` は登録時の添字、`squared_distance` は `long double` の二乗距離である。

## API

### `HnswIndex(max_neighbors=16, ef_construction=200, seed=0)`

空indexを作る。`max_neighbors>=2`、`ef_construction>=max_neighbors` が必要。時間・空間計算量は `O(1)`。

### `insert(point)`

点を動的挿入し添字を返す。非有限座標と距離overflowを拒否し、失敗時は点数と既存の辺を変更しない。`M=max_neighbors`、探索中に距離を評価した点の延べ数を `Q`、処理したlevel数を `H<=33` とすると、期待時間は `O(D+H+Q(D+M+log(Q+1))+HM^2(D+log M))`、追加空間は `O(D+H+Q+HM^2)`。index全体の期待空間は `O(N(D+M))`。

### `nearest(query, k, ef_search, exact_fallback=false)`

階層を貪欲降下後、level 0をbest-first探索する。`1<=k<=N`、`ef_search>=k` が必要。非有限queryを拒否し、距離が `long double` の範囲を超える場合は `std::overflow_error` を送出する。返値は真の二乗距離、添字順。
level 0から得られる候補が `k` 個未満の場合は全点走査へ切り替え、常に `k` 個返す。内部の次数制限で削除された逆向きの辺までは削除しないため、level内の辺は有向であり対称とは限らない。

全levelで距離を評価した点の延べ数を `Q` とすると、近似時の期待時間は `O(Q(D+M+log Q))`、追加空間は `O(Q)`。最悪時は `Q=O(NH)`。exact時は時間 `O(ND+N log N)`、追加空間 `O(N)`。

### `neighbors_of(index, k, ef_search, exact_fallback=false)`

登録点自身を除外して常に `k` 個返す。`1<=k<N`、`ef_search>k` が必要。計算量は `nearest` と同じ。

### `point(index)`

登録点を返す。不正添字では例外。時間・追加空間計算量は `O(1)`。

### `size()`, `empty()`

要素数または空判定を返す。時間・追加空間計算量は `O(1)`。

### `clear()`

全点と辺を消す。乱数器の状態は維持するため、以後のlevel列はそれまでの挿入回数にも依存する。時間 `O(NM)`、追加空間 `O(1)`。
