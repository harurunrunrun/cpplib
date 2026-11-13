---
title: Random Projection Forest (ランダム射影森)
documentation_of: ../../../src/approximate/nearest_neighbor/random_projection_forest.hpp
---

ランダムな超平面で点集合を再帰的に二分した木を複数構築し、同じ葉に到達した点を近傍候補にする。座標型と次元はテンプレート引数で指定し、点は `std::array<Real, Dimension>` で渡す。法線の各座標には、乱数生成器の出力の下位ビットから得た $-1$ または $1$ を使う。乱数生成器の状態が同じなら、結果と距離が等しいときの添字順は決定的である。

近似探索には精度保証がない。葉の候補が `k` 個未満のときだけ全点走査に切り替え、常に `k` 個返す。`exact_fallback = true` では木を使わず全点を走査し、真の二乗距離・添字の順で `k` 個返す。返り値の要素型は `AnnNeighbor` で、`index` は登録時の添字、`squared_distance` は `long double` の二乗距離である。

以下、点数を $N$、$D = \texttt{Dimension}$、$T = \texttt{tree_count}$ とする。

## API

### `Point`

`RandomProjectionForest<Real, Dimension>::Point` は `std::array<Real, Dimension>` の公開型別名である。

### `RandomProjectionForest()`

点と木を持たない森を作る。`size() == 0`, `tree_count() == 0`, `leaf_size() == 0` となる。時間計算量・追加空間計算量は $O(1)$ である。

### `RandomProjectionForest(points, tree_count, leaf_size, random)` / `reset(...)`

森を構築する。`tree_count` と `leaf_size` は正でなければならない。非有限な座標は拒否する。空集合でも構築できる。構築中に例外が発生した場合、既存の森は変更しない。

時間計算量は平均 $O(ND+T+TN(D+\log(N+1))\log(N+1))$、構築後の森を含む追加空間計算量は $O(ND+T+TN(D+1))$ である。$N=0$ でも木の配列を作るため $O(T)$ かかる。

### `size()`, `empty()`, `tree_count()`, `leaf_size()`

各属性を返す。時間計算量・追加空間計算量は $O(1)$ である。

### `point(index)`

元の点を返す。範囲外の添字には `std::out_of_range` を送出する。時間計算量・追加空間計算量は $O(1)$ である。

### `nearest(query, k, exact_fallback = false)`

各木で到達した葉の点の和集合を、真の二乗距離で順位付けする。`1 <= k <= N` でなければならない。

各葉から列挙した延べ候補数を $C$ として、近似探索時の平均時間計算量は $O(TD \log N+CD+C \log C)$、追加空間計算量は $O(C)$ である。候補不足時と厳密探索時は時間 $O(ND+N \log N)$、追加空間 $O(N+C)$ である。

### `neighbors_of(index, k, exact_fallback = false)`

登録点 `index` 自身を除いて検索する。`1 <= k < N` でなければならない。時間計算量・追加空間計算量は `nearest` と同じである。

## 注意点

- `tree_count == 0`、`leaf_size == 0`、非有限な座標・クエリ、範囲外の `k` には `std::invalid_argument`、`neighbors_of` の不正な添字には `std::out_of_range` を送出する
- 距離または射影が `long double` の範囲を超える場合は `std::overflow_error` を送出する
- 厳密探索を指定した場合を除き、探索予算に対する再現率や近似比は保証しない
