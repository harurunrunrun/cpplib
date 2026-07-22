---
title: Tree Isomorphism (木の同型判定)
documentation_of: ../src/algorithm/tree/isomorphism/tree_isomorphism.hpp
---

AHU algorithm による木の同型性判定。

根付き木は根を固定して比較する。根なし木は center を根にして比較する。

## 関数

- `tree_centers(graph)`
    - 木の center を返す。個数は 1 または 2。
- `rooted_tree_isomorphism_labels(graph, root = 0)`
    - 同じ返り値内で、根付き部分木が同型であることと label が等しいことは同値。
- `rooted_tree_isomorphic(a, root_a, b, root_b)`
- `tree_isomorphic(a, b)`

## 時間計算量

$N$ を入力に含まれる頂点数の合計とする。

- `tree_centers`: $O(N)$。返す頂点数は高々2
- `rooted_tree_isomorphism_labels`: $O(N)$。長さ $N$ のlabel列出力を含む
- `rooted_tree_isomorphic`: $O(N)$
- `tree_isomorphic`: center候補を高々2組比較して $O(N)$

部分木の高さごとに子の同型類を確定し、各同型類に属する子の個数で
未確定頂点のpartitionを疎にrefineする。各親子辺は一度だけ個数集計へ
寄与するため、比較sort、連想配列、衝突し得るhashを使わず決定的に線形時間となる。

## 空間計算量

- rooted tree情報、partition、戻り値を含めて $O(N)$

## 注意点

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。
