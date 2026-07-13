---
title: Random Hyperplane LSH
documentation_of: ../src/approximate/nearest_neighbor/random_hyperplane_lsh.hpp
---

# Random Hyperplane LSH

無作為な超平面のどちら側にあるかをbit列にし、cosine類似度が高いベクトルを同じbucketへ集める。各表の超平面は独立な標準正規乱数から作る。

## `RandomHyperplaneLsh(dimension, table_count, bits_per_table, random)`

`dimension > 0`、`table_count > 0`、`1 <= bits_per_table <= 63` が必要である。乱数エンジンは参照で渡す。同じ標準ライブラリ実装と乱数状態では同じ表を構築する。

- 時間計算量: $O(LBD)$
- 空間計算量: $O(LBD)$

## `insert(point)`

非零ベクトルを正規化して追加し、その添字を返す。

- 時間計算量: $O(LBD)$
- 追加空間計算量: $O(D+L)$

## `reset(points)` / `clear()`

全点を入れ直す、または点とbucketだけを空にする。超平面は保持する。

- 時間計算量: `reset` は $O(NLBD)$、`clear` は格納要素数に比例
- 空間計算量: 点を含め $O(ND+NL+LBD)$

## `nearest(query, max_hamming_distance)`

一致bucketを集め、候補内でcosine類似度が最大の点を返す。`max_hamming_distance == 1` では各表について1bit反転したbucketも調べる。候補がなければ `std::nullopt`。

- 時間計算量: $O(LBD + CD)$、1bit探索ではbucket参照が $O(LB)$ 増える
- 追加空間計算量: $O(N+C)$
- 確率的性質: 角度 $\theta$ の2ベクトルが1bitで一致する確率は $1-\theta/\pi$。1表の全bitが一致する確率は $(1-\theta/\pi)^B$、少なくとも1表で一致する確率は $1-(1-(1-\theta/\pi)^B)^L$。返り値の近似比保証はない。

零ベクトルではcosineが定義できないため拒否する。次元不一致、非有限座標、normや内積のoverflowも例外とする。
