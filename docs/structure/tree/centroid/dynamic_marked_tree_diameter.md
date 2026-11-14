---
title: Dynamic Marked Tree Diameter (動的マーク頂点間直径)
documentation_of: ../../../../src/structure/tree/centroid/dynamic_marked_tree_diameter.hpp
---

重み付き木の頂点にマークを付け外しし、マークされた頂点間の最大距離を管理する。辺の重みは負でもよい。
同じ頂点を両端に選べるため、マークされた頂点が 1 個以上あれば答えは 0 以上になる。

重心距離索引の各ブランチで重心からの最大距離を管理し、重心ごとに異なる 2 つのブランチの最大値だけを
組み合わせる。これにより、その重心を通らない 2 頂点の距離を誤って足し合わせることはない。

## テンプレート引数

```cpp
DynamicMarkedTreeDiameter<Distance, MAX_SIZE>
```

`Distance` は整数型、`MAX_SIZE` は頂点数の上限である。`MAX_SIZE` は省略でき、デフォルト値 `-1` では可変長版になる。

```cpp
using Index = CentroidDistanceIndex<Distance, MAX_SIZE>;
```

内部索引の型を `Index` として公開する。オブジェクトはコピーできず、ムーブできる。

## 構築

```cpp
DynamicMarkedTreeDiameter(int n = MAX_SIZE)
void add_edge(int u, int v, Distance weight)
void build(bool initially_marked = false)
```

`add_edge` で無向辺を追加してから `build` を呼ぶ。`initially_marked` が `true` なら全頂点がマークされた状態、
`false` ならどの頂点もマークされていない状態で開始する。`build` を再度呼ぶと、マーク状態を
指定した初期状態に戻す。可変長版ではコンストラクタの `n` のデフォルト値は `0` である。

## `mark` / `unmark` / `toggle`

```cpp
bool mark(int v)
bool unmark(int v)
bool toggle(int v)
```

`mark` は未マークならマークして `true` を、既にマーク済みなら何もせず `false` を返す。
`unmark` はマーク済みなら解除して `true` を、未マークなら何もせず `false` を返す。
`toggle` は状態を反転し、操作後にマークされていれば `true` を返す。

## 状態と直径

```cpp
bool is_marked(int v) const
int marked_count() const
std::optional<Distance> diameter() const
```

`diameter` はマークされた頂点がなければ `nullopt` を、それ以外では最大距離を返す。
同一頂点を両端に選べるため、異なる 2 頂点間の距離がすべて負でも 0 を返す。

## 共通索引

```cpp
int size() const
const CentroidDistanceIndex<Distance, MAX_SIZE>& index() const
```

`size` は頂点数を返す。`index` は内部で構築した重心距離索引への `const` 参照を返す。

## 時間計算量

$N$ を頂点数、$K=O(N\log(N+1))$ を全頂点の重心祖先のエントリ数とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE}\log(\texttt{MAX_SIZE}+1))$、可変長版は $O(N)$
- `add_edge`: $O(1)$
- `build(false)`: $O(N\log(N+1))$
- `build(true)`: $O(N\log^2(N+1))$。$K$ 個の初期エントリを順序付き集合に入れる
- `mark`, `unmark`, `toggle`: $O(\log^2(N+1))$
- `size`, `is_marked`, `marked_count`, `diameter`, `index`: $O(1)$

## 空間計算量

固定長版は、固定容量の重心距離索引とマークされた頂点のエントリを含めて
$O(\texttt{MAX_SIZE}\log(\texttt{MAX_SIZE}+1))$ である。可変長版は実行時の頂点数に応じて確保する。

## 注意点

- `build` 前に更新・クエリ・`index` は呼べない。
- `build` 後に辺は追加できない。
- 頂点、頂点数、辺の容量が範囲外なら `runtime_error` を送出する。
- 入力が木でない場合は `runtime_error` を送出する。
- 木上の距離または直径が `Distance` で表現できなければ `overflow_error` を送出する。
- 容量・例外の条件は `CentroidDistanceIndex` と同じである。
- 容量を指定した固定配列版（`DynamicMarkedTreeDiameter<Distance, N>`）と可変長版（`DynamicMarkedTreeDiameter<Distance>`）は同じヘッダで利用できる。
