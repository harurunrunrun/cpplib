---
title: Level Ancestor (レベル祖先)
documentation_of: ../../../../src/algorithm/tree/query/level_ancestor.hpp
---

静的な根付き木で、頂点から指定距離だけ根側にある祖先を最悪 $O(1)$ で返す。
Euler tour上のlowbit jumpと、最長下向きパスを延長したladderを組み合わせる。

## API

### `LevelAncestor()`

頂点を持たない構造を構築する。

### `LevelAncestor(graph, root = 0)`

無向木 `graph` を `root` で根付けて前処理する。`graph[v]` は頂点 `v` の
隣接頂点を格納する。

### `int size()`

頂点数を返す。

### `int depth(v)`

根を深さ0とした頂点 `v` の深さを返す。

### `int kth_ancestor(v, distance)`

`v` から辺を `distance` 本だけ根側へ進んだ頂点を返す。
`distance < 0` または `distance > depth(v)` なら `-1` を返す。

### `int ancestor_at_depth(v, target_depth)`

`v` の祖先のうち、深さが `target_depth` の頂点を返す。
`target_depth` が $[0,\mathtt{depth(v)}]$ の外なら `-1` を返す。

## 時間計算量

頂点数を $N$、入力された隣接リスト要素数を $M$ とする。

- 入力検査: $O(N+M)$ 時間。
- 正しい無向木に対する `LevelAncestor(graph, root)`: $O(N)$ 時間、
  $O(N)$ 追加空間。
- デフォルト構築、`size`, `depth`: $O(1)$ 時間。
- `kth_ancestor`, `ancestor_at_depth`: word-RAM上で最悪 $O(1)$ 時間。

Euler tourの長さは $2N-1$ であり、各位置にjumpを1個だけ格納する。最長下向き
パスは互いに素で総長が $N$、長さ $L$ の各パスへ追加する祖先は高々 $2L$
なので、全ladder要素数は高々 $3N$ である。したがって二分累乗表のような
$N\log N$ 個の隠れた表を持たない。

距離 $k\ge4$ のqueryでは $t=\operatorname{bit\_floor}(k/4)$ とする。頂点の
Euler位置から距離高々 $t$ にある `lowbit = t` の位置を1回のword演算で選び、
そこから $3t$ のjumpを1回行う。残りはjump先が属するladder内の配列参照1回で
求める。$k<4$ は高々3本だけ親をたどる。

## 注意点

- `graph` は各無向辺を両端の隣接リストにちょうど1回ずつ含む連結な単純木で
  なければならない。自己ループ、平行辺、片方向だけの辺、閉路、非連結成分を
  検出すると `std::invalid_argument` を送出する。
- `root`、隣接頂点、各queryの `v` が頂点範囲外なら
  `std::out_of_range` を送出する。空の `graph` は `root == 0` の場合だけ
  受理する。
- 頂点数が `INT_MAX` を超える場合は `std::length_error` を送出する。
- word-RAMの1語に頂点番号、深さ、Euler位置とそれらに対するbit演算が収まる
  ことを仮定する。実装はGCC 13のC++20で利用できる `std::bit_floor` を使う。
