---
title: Tree Edge Point-Set Path Maximum (木の辺一点更新・パス最大値) [QTREE]
documentation_of: ../../../../src/structure/tree/query/tree_edge_point_set_path_maximum.hpp
---

辺に値を持つ形の変わらない木を構築し、辺の値の一点更新と 2 頂点間のパス上の最大値の取得を処理する。
辺は追加順の整数 ID で参照する。

## API

### `TreeEdgePointSetPathMaximum<Weight, MAX_SIZE>(vertex_count)`

頂点数 `vertex_count` の空の構造を作る。
頂点番号は `[0, vertex_count)` である。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。

### `size()`

頂点数を返す。

### `add_edge(left, right, weight)`

無向辺 `(left, right)` と初期値 `weight` を追加し、その辺 ID を返す。
`build` より前に呼ぶ。

### `build(root = 0)`

追加済みの辺が成す木を `root` を根として前処理する。
以後は辺を追加できない。

### `set_edge(edge_index, weight)`

辺 ID `edge_index` の値を `weight` に置き換える。

### `path_maximum(left, right)`

`left` から `right` への単純パスに含まれる辺の値の最大値を返す。
`left == right` の空パスでは `MaxMonoid<Weight>::e()`、通常の数値型では
`numeric_limits<Weight>::lowest()` を返す。

## 時間計算量

$N$ を `vertex_count` とする。可変長版では以下の $\texttt{MAX_SIZE}$ を $N$ と読み替える。

- コンストラクタ: $O(N + \texttt{MAX_SIZE})$
- `size`: $O(1)$
- `add_edge`: 償却 $O(1)$
- `build`: $O(N \log \texttt{MAX_SIZE})$
- `set_edge`: $O(\log \texttt{MAX_SIZE})$
- `path_maximum`: $O(\log N \log \texttt{MAX_SIZE})$。$\texttt{MAX_SIZE} = O(N)$ なら $O(\log^2 N)$

使用メモリは $O(N + \texttt{MAX_SIZE})$ である。

## 注意点

- `0 <= vertex_count` が必要で、固定長版では `vertex_count <= MAX_SIZE` が必要である。
- `Weight` は `MaxMonoid<Weight>` で扱える比較可能な型であり、
  `numeric_limits<Weight>::lowest()` が最大値演算の単位元でなければならない。
- `vertex_count > 0` では、追加辺は連結で閉路のない木でなければならない。
- 頂点範囲違反、木でない入力、構築前の更新・取得、構築後の辺追加、
  不正な辺 ID では `runtime_error` を送出する。
- 容量を指定した固定配列版（`TreeEdgePointSetPathMaximum<Weight, N>`）と可変長版（`TreeEdgePointSetPathMaximum<Weight>`）は同じヘッダで利用できる。
