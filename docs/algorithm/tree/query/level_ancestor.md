---
title: Level Ancestor (レベル祖先)
documentation_of: ../../../../src/algorithm/tree/query/level_ancestor.hpp
---

静的な根付き木で、頂点から指定した距離だけ根側にある祖先を最悪 $O(1)$ で返す。
Euler tour 上の lowbit ジャンプと、最長の下向きパスを延長した ladder を組み合わせる。

## API

### `LevelAncestor()`

頂点を持たない構造を構築する。

### `LevelAncestor(graph, root = 0)`

無向木 `graph` を `root` で根付き木にして前処理する。`graph[v]` には頂点 `v` の隣接頂点を格納する。

### `int size()`

頂点数を返す。

### `int depth(v)`

根の深さを $0$ としたときの、頂点 `v` の深さを返す。

### `int kth_ancestor(v, distance)`

`v` から辺を `distance` 本だけ根側へ進んだ頂点を返す。
`distance < 0` または `distance > depth(v)` なら `-1` を返す。

### `int ancestor_at_depth(v, target_depth)`

`v` の祖先のうち、深さが `target_depth` の頂点を返す。
`target_depth` が $[0, \mathrm{depth}(v)]$ の外なら `-1` を返す。

## 時間計算量

頂点数を $N$、入力された隣接リストの要素数を $M$ とする。

- 入力の検査: $O(N+M)$ 時間。
- 正しい無向木に対する `LevelAncestor(graph, root)`: $O(N)$ 時間、$O(N)$ の追加空間。
- デフォルト構築、`size`, `depth`: $O(1)$ 時間。
- `kth_ancestor`, `ancestor_at_depth`: word-RAM 上で最悪 $O(1)$ 時間。

Euler tour の長さは $2N-1$ であり、各位置にジャンプを 1 個だけ格納する。
最長の下向きパスは互いに素で総長が $N$ であり、長さ $L$ の各パスに追加する祖先は高々 $2L$ 個なので、ladder の要素数の合計は高々 $3N$ である。
したがって、ダブリングの表のような $N\log N$ 個の隠れた表を持たない。

距離 $k\ge4$ のクエリでは、$t$ を $\lfloor k/4 \rfloor$ 以下の最大の 2 冪（`std::bit_floor(k / 4)`）とする。
頂点の Euler 位置から距離 $t$ 以内にある `lowbit = t` の位置を 1 回のワード演算で選び、そこから $3t$ のジャンプを 1 回行う。
残りは、ジャンプ先が属する ladder 内の配列参照 1 回で求める。$k<4$ では高々 3 回だけ親をたどる。

## 注意点

- `graph` は、各無向辺を両端の隣接リストにちょうど 1 回ずつ含む連結な単純木でなければならない。自己ループ、平行辺、片方向だけの辺、閉路、非連結な成分を検出すると `std::invalid_argument` を送出する。
- `root`、隣接頂点、各クエリの `v` が頂点の範囲外なら `std::out_of_range` を送出する。空の `graph` は `root == 0` の場合だけ受理する。
- 頂点数が `INT_MAX` を超える場合は `std::length_error` を送出する。
- word-RAM の 1 語に頂点番号、深さ、Euler 位置と、それらに対するビット演算が収まることを仮定する。実装は C++20 の `std::bit_floor` を使う。
