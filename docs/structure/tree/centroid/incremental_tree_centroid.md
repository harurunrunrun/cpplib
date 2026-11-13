---
title: Incremental Tree Centroid (逐次追加木の重心管理)
documentation_of: ../../../../src/structure/tree/centroid/incremental_tree_centroid.hpp
---

根から始めて、既存の頂点の子として葉を 1 つずつ追加していく木について、重心を管理する。
1 頂点を追加して重心が動く場合、追加した頂点の側にある隣接頂点へ 1 辺だけ動くという性質を使う。
動的なオイラーツアーの括弧列を暗黙の treap で持ち、各部分木の現在のサイズを求める。

## 型

```cpp
IncrementalTreeCentroid<MAX_SIZE>
```

頂点番号は追加順に `0, 1, ...` と付く。構造本体の固定配列はヒープに確保される。
`MAX_SIZE` は省略でき、デフォルト値 `-1` では頂点の追加に合わせて `std::vector` を伸長する可変長版になる。
コピーはできず、ムーブはできる。

## API・時間計算量

$N$ を現在の頂点数、$L$ を祖先表の段数（固定長版は $\log_2(\texttt{MAX_SIZE})+1$ 程度、可変長版は `int` の最大値の半分のビット幅で約 $31$）とする。

- `IncrementalTreeCentroid()`: 空の構造を作る。固定長版の時間・領域は $O(\texttt{MAX_SIZE}\cdot L)$。
- `add_root()`: 空の構造に頂点 0 を根として追加し、`0` を返す。期待時間 $O(1)$、追加領域 $O(1)$。
- `add_leaf(parent)`: `parent` の子として新しい葉を追加し、その頂点番号を返す。期待時間 $O(\log(N+1)+L)$、追加領域 $O(L)$（可変長版では配列の伸長をならした計算量）。
- `size()`: 現在の頂点数を返す。時間・追加領域 $O(1)$。
- `capacity()`: 最大頂点数を返す（可変長版では `int` の最大値の半分）。時間・追加領域 $O(1)$。
- `centroid()`: 空なら `-1`、それ以外は現在の重心を 1 つ返す。時間・追加領域 $O(1)$。
- `parent(vertex)`, `depth(vertex)`: 根付き木での親または深さを返す。根の親は `-1` である。時間・追加領域 $O(1)$。
- `kth_ancestor(vertex, distance)`: `distance` 辺だけ上の祖先を返す。根より上なら `-1` を返す。時間 $O(L)$、追加領域 $O(1)$。
- `is_ancestor(ancestor, vertex)`: `ancestor` が `vertex` の祖先（自身を含む）かを返す。時間 $O(L)$、追加領域 $O(1)$。
- `subtree_size(vertex)`: 最初の根に関する現在の部分木サイズを返す。期待時間 $O(\log(N+1))$、追加領域 $O(1)$。

保存領域は固定長版で $O(\texttt{MAX_SIZE}\cdot L)$、可変長版で $O(N L)$ である。

## 注意点

- `add_root` は最初に一度だけ呼ぶ。根以外の追加は葉の追加に限り、辺の削除・付け替えはできない。
- 重心が 2 頂点ある場合、`centroid()` はその一方を返す。
- 範囲外の頂点は `out_of_range`、容量超過は `length_error`、2 回目の `add_root` は `logic_error` を送出する。
- 暗黙の treap の高さに関する計算量は、固定シードの擬似乱数による優先度に対する期待値である。
- 容量を指定した固定配列版（`IncrementalTreeCentroid<N>`）と可変長版（`IncrementalTreeCentroid<>`）は同じヘッダで利用できる。
