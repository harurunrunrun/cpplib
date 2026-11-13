---
title: Tree Isomorphism (木の同型判定)
documentation_of: ../../../../src/algorithm/tree/isomorphism/tree_isomorphism.hpp
---

AHU アルゴリズムにより木の同型性を判定する。
根付き木は根を固定して比較する。根なし木は中心（center）を根にして比較する。

## API

- `tree_centers(graph)`: 木の中心を返す。個数は 1 または 2 で、空木では空の列を返す。
- `rooted_tree_isomorphism_labels(graph, root = 0)`: 各頂点にラベルを付けた列を返す。同じ戻り値の中では、根付き部分木が同型であることとラベルが等しいことは同値である。
- `rooted_tree_isomorphic(a, root_a, b, root_b)`: 根付き木 `a`, `b` が根を対応させて同型かを返す。
- `tree_isomorphic(a, b)`: 根なし木 `a`, `b` が同型かを返す。

## 時間計算量

$N$ を入力に含まれる頂点数の合計とする。

- `tree_centers`: $O(N)$。返す頂点数は高々 2
- `rooted_tree_isomorphism_labels`: 長さ $N$ のラベル列の出力を含めて $O(N)$
- `rooted_tree_isomorphic`: $O(N)$
- `tree_isomorphic`: 中心の候補を高々 2 組比較して $O(N)$

部分木の高さごとに子の同型類を確定し、各同型類に属する子の個数で未確定の頂点の分割を疎に細分化する。
各親子辺は 1 度だけ個数の集計に寄与するため、比較ソート、連想配列、衝突し得るハッシュを使わずに決定的に線形時間となる。

## 空間計算量

根付き木の情報、分割、戻り値を含めて $O(N)$ である。

## 注意点

- 頂点数、`root`、各隣接頂点の番号は `int` の範囲内でなければならない。
- 無向辺 $\lbrace u, v\rbrace$ は、`graph[u]` に `v` をちょうど 1 回、`graph[v]` に `u` をちょうど 1 回格納する。自己ループ、同じ向きの重複した弧、逆向きの弧の欠落は不正である。
- 空でない入力は、無向辺の本数がちょうど $N - 1$ の連結木でなければならない。端点の範囲、表現の非対称性、重複、自己ループ、辺数、閉路、連結性の違反時は `std::runtime_error` を送出する。
- 記載した計算量には、決定的な $O(N)$ の木の検査を含む。
