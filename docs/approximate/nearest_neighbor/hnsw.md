---
title: HNSW (階層的近似近傍探索)
documentation_of: ../../../src/approximate/nearest_neighbor/hnsw.hpp
---

確率的な階層近傍グラフ（Hierarchical Navigable Small World）で近似最近傍を探索する。点は `std::array<Real, Dimension>` である。同じシード・挿入順・引数なら結果は決定的で、距離が等しい場合は添字順に処理する。重複点を許す。

近似探索に精度保証はない。`ef_search` を増やすと通常は再現率と計算量が増える。`exact_fallback = true` では階層を使わず全点を走査する。返り値の要素型は `AnnNeighbor` で、`index` は登録時の添字、`squared_distance` は `long double` の二乗距離である。

以下、点数を $N$、次元を $D$、$M = \texttt{max_neighbors}$ とする。

## API

### `Point`

`HnswIndex<Real, Dimension>::Point` は `std::array<Real, Dimension>` の公開型別名である。

### `HnswIndex(max_neighbors = 16, ef_construction = 200, seed = 0)`

空のインデックスを作る。`max_neighbors >= 2` かつ `ef_construction >= max_neighbors` でなければならない。時間計算量・追加空間計算量は $O(1)$ である。

### `insert(point)`

点を動的に挿入し、その添字を返す。非有限な座標と距離のオーバーフローを拒否し、失敗時は点数と既存の辺を変更しない。探索中に距離を評価した点の延べ数を $Q$、処理したレベル数を $H \le 33$ とすると、期待時間計算量は $O(D+H+Q(D+M+\log(Q+1))+HM^2(D+\log M))$、追加空間計算量は $O(D+H+Q+HM^2)$ である。インデックス全体の期待空間計算量は $O(N(D+M))$ である。

### `nearest(query, k, ef_search, exact_fallback = false)`

階層を貪欲に降りたあと、レベル 0 を best-first 探索する。`1 <= k <= N` かつ `ef_search >= k` でなければならない。返り値は真の二乗距離・添字の順に並ぶ。レベル 0 から得られる候補が `k` 個未満の場合は全点走査に切り替え、常に `k` 個返す。次数制限で削った辺の逆向きの辺は削除しないため、各レベルの辺は有向であり、対称とは限らない。

全レベルで距離を評価した点の延べ数を $Q$ とすると、近似探索時の期待時間計算量は $O(Q(D+M+\log Q))$、追加空間計算量は $O(Q)$ である。最悪時は $Q=O(NH)$ である。厳密探索時の時間計算量は $O(ND+N \log N)$、追加空間計算量は $O(N)$ である。

### `neighbors_of(index, k, ef_search, exact_fallback = false)`

登録点自身を除いて常に `k` 個返す。`1 <= k < N` かつ `ef_search > k` でなければならない。時間計算量・追加空間計算量は `nearest` と同じである。

### `point(index)`

登録点を const 参照で返す。時間計算量・追加空間計算量は $O(1)$ である。

### `size()`, `empty()`

要素数、または空かどうかを返す。時間計算量・追加空間計算量は $O(1)$ である。

### `clear()`

全点と辺を消す。乱数生成器の状態は維持するため、以後のレベル列はそれまでの挿入回数にも依存する。時間計算量は $O(NM)$、追加空間計算量は $O(1)$ である。

## 注意点

- パラメータ・`k`・`ef_search` が上記の条件を満たさない場合や、非有限な座標・クエリには `std::invalid_argument` を送出する
- 二乗距離が `long double` の範囲を超える場合は `std::overflow_error` を送出する
- `point` と `neighbors_of` は範囲外の添字に `std::out_of_range` を送出する
- 厳密探索を指定した場合を除き、探索予算に対する再現率や近似比は保証しない
