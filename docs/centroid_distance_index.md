---
title: Centroid Distance Index (重心距離索引)
documentation_of: ../src/structure/tree/centroid_distance_index.hpp
---

重み付き木を重心分解し、各頂点から重心木上の全祖先への元の木での距離を保持する。
重心を取り除いたときの連結成分を `branch` として区別する。重心自身も独立した
branchに属する。

# テンプレート引数

```cpp
CentroidDistanceIndex<Distance, MAX_SIZE>
```

`Distance` は整数型、`MAX_SIZE` は頂点数の上限。

```cpp
static constexpr int MAX_LOG
static constexpr int MAX_BRANCH
```

`MAX_LOG` は1頂点に保存できる重心祖先entry数、`MAX_BRANCH` はbranch容量。
どちらも `MAX_SIZE` からコンパイル時に決まる。

# Entry

```cpp
struct Entry {
    int centroid;
    int branch;
    Distance distance;
};
```

- `centroid`: 重心木上の祖先。
- `branch`: `centroid` を取り除いたときに頂点が属する連結成分の番号。
  頂点自身が重心の場合は重心専用のbranch。
- `distance`: 元の木における頂点と `centroid` の重み付き距離。

# コンストラクタ

```cpp
CentroidDistanceIndex(int n = MAX_SIZE)
```

頂点数 `n` の空のグラフを作る。
copyはできず、moveできる。

# add_edge

```cpp
void add_edge(int u, int v, Distance weight)
```

無向辺を追加する。`build` 後には追加できない。

# build

```cpp
void build()
```

入力が木であることを検査し、重心分解と距離索引を構築する。再度呼ぶと同じ辺集合から
索引を再構築する。

# 重心木の情報

```cpp
int size() const
int root() const
int parent(int v) const
int depth(int v) const
```

`root` は重心木の根を返す。`parent` と `depth` は重心木上の親と深さを返す。
根の親は `-1`。空の木の根は `-1`。

# branch

```cpp
int branch_count() const
int branch_centroid(int branch) const
```

生成されたbranch数と、そのbranchを所有する重心を返す。非空の木では重心自身のbranchを
各頂点に1個、元の木の各辺に対応するbranchを1個生成するため、branch数は `2*N-1`。

# entries

```cpp
std::span<const Entry> entries(int v) const
```

`v` に対応する索引を重心木の根側から順に返す。要素数は
$O(\log(N+1))$。返したspanはオブジェクトの再構築・破棄まで有効。

# 時間計算量

- コンストラクタ: $O(\mathtt{MAX\_SIZE}\log(\mathtt{MAX\_SIZE}+1))$
- `size`: $O(1)$
- `add_edge`: $O(1)$
- `build`: $O(N\log(N+1))$
- `root`, `parent`, `depth`, `branch_count`, `branch_centroid`: $O(1)$
- `entries`: $O(1)$。返すspanの走査は $O(\log(N+1))$

# 空間計算量

固定容量の辺配列と重心祖先索引を含めて
$O(\mathtt{MAX\_SIZE}\log(\mathtt{MAX\_SIZE}+1))$。

# API契約・例外

- `build` 前に `root`、`parent`、`depth`、branch API、`entries` は呼べない。
- 頂点、branch、頂点数、辺容量が範囲外なら `runtime_error`。
- 閉路、非連結、辺数不一致など入力が木でなければ `runtime_error`。
- 距離加算が `Distance` の範囲を外れる場合は `overflow_error`。
- `Distance` は整数型であり、すべての木上距離が表現可能でなければならない。
