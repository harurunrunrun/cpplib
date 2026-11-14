---
title: Monochromatic Component Maximum Tree (同色連結成分最大値) [QTREE7]
documentation_of: ../../../../src/structure/tree/query/monochromatic_component_max_tree.hpp
---

2 色で塗られた木で、色の反転・頂点値の代入と、同色連結成分の最大値の取得を扱う。
`DynamicMonochromaticTree` に `MaxMonoid<Value>` を与えた薄いラッパーである。

## API

### `MonochromaticComponentMaxTree<Value, MAX_SIZE>(n)`

頂点数 `n` の構造を作る。初期色はすべて `false`、初期値は `MaxMonoid<Value>` の単位元である。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。
時間計算量は固定長版で $O(\texttt{MAX_SIZE})$、可変長版で $O(n)$ である。

### `add_edge(left, right)`

無向辺を追加する。時間計算量は $O(1)$ である。

### `set_color(vertex, color)` / `set_value(vertex, value)`

頂点の色または値を代入する。`set_color` は色が変化したとき `true` を返す。時間計算量は構築前は $O(1)$、構築後は $O(\log n)$ である。

### `build()`

索引を構築する。時間計算量は $O(n)$ である。

### `toggle(vertex)`

色を反転し、反転後の色を返す。時間計算量は構築前は $O(1)$、構築後は $O(\log n)$ である。

### `component_maximum(vertex)`

`vertex` を含む同色連結成分の頂点値の最大値を返す。時間計算量は $O(\log^2 n)$ である。

## 注意点

- 頂点数は `MAX_SIZE` 以下で、辺集合は連結な木でなければならない。
- `build` 前の `component_maximum`、範囲外の頂点、木でない入力では `runtime_error` を送出する。
- `Value` は `MaxMonoid` で比較できる必要がある。
- 容量を指定した固定配列版（`MonochromaticComponentMaxTree<Value, N>`）と可変長版（`MonochromaticComponentMaxTree<Value>`）は同じヘッダで利用できる。
