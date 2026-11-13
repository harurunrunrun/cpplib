---
title: Random Hyperplane LSH (ランダム超平面LSH)
documentation_of: ../../../src/approximate/nearest_neighbor/random_hyperplane_lsh.hpp
---

ランダムな超平面のどちら側にあるかをビット列にし、コサイン類似度が高いベクトルを同じバケットに集める局所性鋭敏型ハッシュ（LSH）である。各表の超平面は独立な標準正規乱数から作る。

以下、次元を $D$、表の数を $L$、1 表のビット数を $B$、点数を $N$、厳密に比較した候補数を $C$ とする。

## API

### `CosineLshNeighbor`

選ばれた点の登録時の添字 `index`、コサイン類似度 `cosine_similarity`、厳密に比較した候補数 `candidates_examined` を保持する。各メンバの参照の時間計算量・追加空間計算量は $O(1)$ である。

### `RandomHyperplaneLsh<T = double>(dimension, table_count, bits_per_table, random)`

`dimension > 0`、`table_count > 0`、`1 <= bits_per_table <= 63` でなければならない。乱数エンジンは参照で渡す。同じ標準ライブラリ実装と乱数状態なら同じ表を構築する。

- 時間計算量: $O(LBD)$
- 空間計算量: $O(LBD)$

### `insert(point)`

`std::vector<T>` の非零ベクトルを正規化して追加し、その添字を返す。

- 時間計算量: $O(LBD)$
- 追加空間計算量: $O(D+L)$

### `reset(points)` / `clear()`

全点を入れ直す、または点とバケットだけを空にする。超平面は保持する。

- 時間計算量: `reset` は $O(NLBD)$、`clear` は格納要素数に比例
- 空間計算量: 点を含めて $O(ND+NL+LBD)$

### `nearest(query, max_hamming_distance = 0)`

一致するバケットの点を集め、候補の中でコサイン類似度が最大の点を返す。`max_hamming_distance == 1` では、各表について 1 ビットだけ反転したバケットも調べる。候補がなければ `std::nullopt` を返す。

- 時間計算量: $O(LBD + CD)$。1 ビット反転も調べる場合はバケットの参照が $O(LB)$ 回増える
- 追加空間計算量: $O(N+C)$
- 確率的性質: なす角 $\theta$ の 2 ベクトルが 1 ビットで一致する確率は $1-\theta/\pi$ である。1 表の全ビットが一致する確率は $(1-\theta/\pi)^B$、少なくとも 1 表で一致する確率は $1-(1-(1-\theta/\pi)^B)^L$ である。返り値の近似比は保証しない

### `dimension()`, `size()`, `table_count()`, `bits_per_table()`

次元、登録点数、表の数、1 表のビット数を返す。時間計算量・追加空間計算量は $O(1)$ である。

## 注意点

- 零ベクトルではコサインが定義できないため、`std::invalid_argument` を送出して拒否する
- 次元の不一致、非有限な座標、2 以上の `max_hamming_distance`、不正なコンストラクタ引数には `std::invalid_argument` を送出する
- ノルム・内積・コサイン類似度の計算のオーバーフローには `std::overflow_error` を送出する
- `reset` は先に `clear` してから 1 点ずつ挿入するため、途中で例外が起きると、それまでに挿入した点だけが残る
