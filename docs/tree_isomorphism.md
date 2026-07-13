---
title: Tree Isomorphism (木の同型判定)
documentation_of: ../src/algorithm/tree/tree_isomorphism.hpp
---

AHU algorithm による木の同型性判定。

根付き木は根を固定して比較する。根なし木は center を根にして比較する。

## 関数

- `tree_centers(graph)`
    - 木の center を返す。個数は 1 または 2。
- `rooted_tree_isomorphism_labels(graph, root = 0)`
    - 同じ返り値内で、根付き部分木が同型なら同じ label。
- `rooted_tree_isomorphic(a, root_a, b, root_b)`
- `tree_isomorphic(a, b)`

## 時間計算量

$N$ を各木の頂点数とする。

- `tree_centers`: $O(N)$。返す頂点数は高々2
- `rooted_tree_isomorphism_labels`: $O(N\log(N+1))$。長さ $N$ のlabel列出力を含む
- `rooted_tree_isomorphic`: $O(N\log(N+1))$
- `tree_isomorphic`: center候補を高々2組比較して $O(N\log(N+1))$

子label列の整列と `map<vector<int>, int>` の辞書操作が対数因子を持つ。

## 空間計算量

- rooted tree情報、signature辞書、戻り値を含めて $O(N)$

## API契約・前提・例外

頂点とrootは範囲内でなければならない。空木を明記した場合を除き、入力は無向の連結木とする。端点範囲、閉路、辺数、連結性の違反時は `runtime_error` を送出する。記載した計算量には木の検査とResultの構築を含む。
