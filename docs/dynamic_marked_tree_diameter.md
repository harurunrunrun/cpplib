---
title: Dynamic Marked Tree Diameter (動的マーク頂点間直径)
documentation_of: ../src/structure/tree/dynamic_marked_tree_diameter.hpp
---

重み付き木の頂点をmark/unmarkし、marked頂点間の最大距離を管理する。辺重みは負でもよい。
同じ頂点を両端に選べるため、marked頂点が1個以上あれば答えは0以上になる。

重心距離索引の各branchで最大距離を管理し、重心ごとに異なる2 branchの最大値だけを
組み合わせる。これにより、その重心を通らない2頂点を誤って足さない。

# テンプレート引数

```cpp
DynamicMarkedTreeDiameter<Distance, MAX_SIZE>
```

`Distance` は整数型、`MAX_SIZE` は頂点数の上限。

```cpp
using Index = CentroidDistanceIndex<Distance, MAX_SIZE>;
```

内部索引の型を `Index` として公開する。オブジェクトはcopyできず、moveできる。

# 構築

```cpp
DynamicMarkedTreeDiameter(int n = MAX_SIZE)
void add_edge(int u, int v, Distance weight)
void build(bool initially_marked = false)
```

`add_edge` で無向辺を追加してから `build` を呼ぶ。`initially_marked` がtrueなら全頂点、
falseならどの頂点もmarkされていない状態で開始する。`build` を再度呼ぶとmarked状態を
指定した初期状態へ戻す。

# mark / unmark / toggle

```cpp
bool mark(int v)
bool unmark(int v)
bool toggle(int v)
```

`mark` は未markならmarkしてtrue、既にmark済みなら何もせずfalseを返す。
`unmark` はmark済みなら解除してtrue、既に未markなら何もせずfalseを返す。
`toggle` は状態を反転し、操作後にmarkされていればtrueを返す。

# 状態と直径

```cpp
bool is_marked(int v) const
int marked_count() const
std::optional<Distance> diameter() const
```

`diameter` はmarked頂点がなければ `nullopt`、それ以外では最大距離を返す。
同一頂点を両端に選べるため、全ての候補距離が負でも0を返す。

# 共通索引

```cpp
int size() const
const CentroidDistanceIndex<Distance, MAX_SIZE>& index() const
```

`index` は内部で構築した重心距離索引へのconst referenceを返す。

# 時間計算量

$K=O(N\log(N+1))$ を全頂点の重心祖先entry数とする。

- コンストラクタ: $O(\mathtt{MAX\_SIZE}\log(\mathtt{MAX\_SIZE}+1))$
- `add_edge`: $O(1)$
- `build(false)`: $O(N\log(N+1))$
- `build(true)`: $O(N\log^2(N+1))$。$K$ 個の初期entryを順序付き集合へ入れる
- `mark`, `unmark`, `toggle`: $O(\log^2(N+1))$
- `size`, `is_marked`, `marked_count`, `diameter`, `index`: $O(1)$

# 空間計算量

固定容量の重心距離索引とmarked頂点のentryを含めて
$O(\mathtt{MAX\_SIZE}\log(\mathtt{MAX\_SIZE}+1))$。

## 注意点

- `build` 前に更新・query・`index` は呼べない。
- `build` 後に辺は追加できない。
- 頂点、頂点数、辺容量が範囲外なら `runtime_error`。
- 入力が木でない場合は `runtime_error`。
- 木上距離または直径が `Distance` で表現できなければ `overflow_error`。
- 容量・例外条件は `CentroidDistanceIndex` と同じ。
