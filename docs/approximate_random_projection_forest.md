---
title: Random Projection Forest (ランダム射影森)
documentation_of: ../src/approximate/nearest_neighbor/random_projection_forest.hpp
---

# Random Projection Forest (ランダム射影森)

ランダムな超平面で点集合を再帰的に二分した木を複数構築し、同じ葉に到達した点を近傍候補にする。座標型と次元はテンプレート引数で、点は `std::array<Real, Dimension>` で渡す。法線の各座標には乱数器出力の下位bitから得た `-1` または `1` を使う。乱数器の状態が同じなら結果と同距離時の添字順は決定的である。

近似探索には精度保証がない。葉候補が `k` 個未満のときだけ全点走査へ切り替え、常に `k` 個返す。`exact_fallback=true` は木を使用せず全点を走査し、真の二乗距離、添字の順で `k` 個返す。
返値の要素型は `AnnNeighbor` で、`index` は登録時の添字、`squared_distance` は `long double` の二乗距離である。

## API

### `RandomProjectionForest()`

点と木を持たないforestを作る。`size()=0`, `tree_count()=0`, `leaf_size()=0` となる。時間・空間計算量は `O(1)`。

### `RandomProjectionForest(points, tree_count, leaf_size, random)` / `reset(...)`

forestを構築する。`tree_count` と `leaf_size` は正でなければならない。非有限座標を拒否する。空集合は構築できる。構築中に例外が発生した場合、既存forestは変更しない。

時間計算量は平均 `O(ND+T+TN(D+log(N+1))log(N+1))`、空間計算量は `O(ND+T+TN(D+1))`。ここで `T=tree_count`、`D=Dimension`。`N=0` でも木の配列を作るため `O(T)` を要する。

### `size()`, `empty()`, `tree_count()`, `leaf_size()`

各属性を返す。時間・追加空間計算量は `O(1)`。

### `point(index)`

元の点を返す。不正添字では例外。時間・追加空間計算量は `O(1)`。

### `nearest(query, k, exact_fallback=false)`

各木で到達した葉の和集合を真の二乗距離で順位付けする。`k=0`、`k>N`、非有限queryを拒否し、距離または射影が `long double` の範囲を超える場合は `std::overflow_error` を送出する。

各葉から列挙した延べ候補数を `C` として、近似時の平均時間は `O(TD log N+CD+C log C)`、追加空間は `O(C)`。候補不足時とexact時は時間 `O(ND+N log N)`、追加空間 `O(N+C)`。

### `neighbors_of(index, k, exact_fallback=false)`

登録点自身を除外して検索する。不正添字、`k=0`、`k>=N` を拒否する。計算量は `nearest` と同じ。
