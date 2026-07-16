---
title: Incremental Tree Centroid (逐次追加木の重心管理)
documentation_of: ../src/structure/tree/incremental_tree_centroid.hpp
---

根から始め、既存頂点の子として葉を一つずつ追加する木について重心を保つ。
1頂点の追加で重心が動く場合は追加頂点側の隣接頂点へ1辺だけ動く性質を使う。
動的Euler括弧列をimplicit treapで持ち、各部分木の現在サイズを求める。

## 型

```cpp
IncrementalTreeCentroid<MAX_SIZE>
```

頂点番号は追加順に `0,1,...` と付く。構造本体の固定配列はheapに確保される。

## API・時間計算量

- `IncrementalTreeCentroid()`: 空の構造を作る。時間・領域 $O(MAX\_SIZE\log(MAX\_SIZE+1))$。
- `add_root()`: 空の構造へ頂点0を根として追加する。期待時間 $O(1)$、追加領域 $O(1)$。
- `add_leaf(parent)`: `parent` の子として新しい葉を追加し、その頂点番号を返す。期待時間 $O(\log(N+1)+\log(MAX\_SIZE+1))$、追加領域 $O(\log(N+1))$。
- `size()`: 現在の頂点数を返す。時間・追加領域 $O(1)$。
- `capacity()`: 最大頂点数を返す。時間・追加領域 $O(1)$。
- `centroid()`: 空なら `-1`、それ以外は現在の重心を一つ返す。時間・追加領域 $O(1)$。
- `parent(vertex)`, `depth(vertex)`: 根付き木での親または深さを返す。時間・追加領域 $O(1)$。
- `kth_ancestor(vertex, distance)`: `distance` 辺上の祖先を返す。根より上なら `-1`。時間 $O(\log(MAX\_SIZE+1))$、追加領域 $O(1)$。
- `is_ancestor(ancestor, vertex)`: 祖先関係を返す。時間 $O(\log(MAX\_SIZE+1))$、追加領域 $O(1)$。
- `subtree_size(vertex)`: 構築時の根に対する現在の部分木サイズを返す。期待時間 $O(\log(N+1))$、追加領域 $O(1)$。

保存領域は $O(MAX\_SIZE\log(MAX\_SIZE+1))$。

## 注意点

- `add_root` は最初に一度だけ呼ぶ。根以外の追加は葉の追加に限り、辺の削除・付け替えはできない。
- 重心が2頂点ある場合、`centroid()` はその一方を返す。
- 範囲外頂点は `out_of_range`、容量超過は `length_error`、2回目の `add_root` は `logic_error`。
- implicit treapの高さに関する計算量は固定seedの擬似乱数優先度に対する期待値。
