---
title: Centroid Distance Index (重心距離索引)
documentation_of: ../../../../src/structure/tree/centroid/centroid_distance_index.hpp
---

重み付き木を重心分解し、各頂点について、重心木上の全祖先までの元の木での距離を保持する。
重心を取り除いたときの連結成分を「ブランチ」（`branch`）として区別する。重心自身も独立した
ブランチに属する。

## テンプレート引数

```cpp
CentroidDistanceIndex<Distance, MAX_SIZE>
```

`Distance` は整数型、`MAX_SIZE` は頂点数の上限である。`MAX_SIZE` は省略でき、デフォルト値 `-1` では `std::vector` を使う可変長版になる。

```cpp
static constexpr int MAX_LOG
static constexpr int MAX_BRANCH
```

`MAX_LOG` は 1 頂点に保存できる重心祖先のエントリ数、`MAX_BRANCH` はブランチ数の容量である。
どちらも `MAX_SIZE` からコンパイル時に決まる。

## `Entry`

```cpp
struct Entry {
    int centroid;
    int branch;
    Distance distance;
};
```

- `centroid`: 重心木上の祖先。
- `branch`: `centroid` を取り除いたときに頂点が属する連結成分の番号。
  頂点自身が `centroid` である場合は、その重心専用のブランチ。
- `distance`: 元の木における頂点と `centroid` の重み付き距離。

## コンストラクタ

```cpp
CentroidDistanceIndex(int n = MAX_SIZE)
```

頂点数 `n` の辺のないグラフを作る。可変長版では `n` のデフォルト値は `0` である。
コピーはできず、ムーブはできる。

## `add_edge`

```cpp
void add_edge(int u, int v, Distance weight)
```

重み `weight` の無向辺を追加する。`build` 後には追加できない。

## `build`

```cpp
void build()
```

入力が木であることを検査し、重心分解と距離索引を構築する。再度呼ぶと、同じ辺集合から
索引を構築し直す。

## 重心木の情報

```cpp
int size() const
int root() const
int parent(int v) const
int depth(int v) const
```

`size` は頂点数を返す。`root` は重心木の根を返し、`parent` と `depth` は重心木上の親と深さを返す。
根の親は `-1` である。空の木の根は `-1` である。

## ブランチ

```cpp
int branch_count() const
int branch_centroid(int branch) const
```

生成されたブランチ数と、指定したブランチを所有する重心を返す。空でない木では、各頂点に重心自身のブランチを
1 個、元の木の各辺に対応するブランチを 1 個生成するため、ブランチ数は $2N-1$ である。

## `entries`

```cpp
std::span<const Entry> entries(int v) const
```

`v` の索引を重心木の根側から順に返す。要素数は
$O(\log(N+1))$ である。返した `span` は、オブジェクトの再構築・破棄まで有効である。

## 時間計算量

$N$ を頂点数とする。

- コンストラクタ: 固定長版は $O(\texttt{MAX_SIZE}\log(\texttt{MAX_SIZE}+1))$、可変長版は $O(N)$
- `size`: $O(1)$
- `add_edge`: $O(1)$
- `build`: $O(N\log(N+1))$
- `root`, `parent`, `depth`, `branch_count`, `branch_centroid`: $O(1)$
- `entries`: $O(1)$。返した `span` の走査は $O(\log(N+1))$

## 空間計算量

固定長版は、固定容量の辺配列と重心祖先の索引を含めて
$O(\texttt{MAX_SIZE}\log(\texttt{MAX_SIZE}+1))$ である。
可変長版は実行時の頂点数に応じて配列を確保する（頂点ごとの祖先配列は `MAX_LOG` 要素の固定長である）。

## 注意点

- `build` 前に `root`、`parent`、`depth`、ブランチ関連の API、`entries` は呼べない。
- 頂点、ブランチ、頂点数、辺の容量が範囲外なら `runtime_error` を送出する。
- 閉路、非連結、辺数の不一致など、入力が木でなければ `runtime_error` を送出する。
- 距離の加算が `Distance` の範囲を外れる場合は `overflow_error` を送出する。
- `Distance` は整数型であり、木上のすべての距離が表現可能でなければならない。
- 容量を指定した固定配列版（`CentroidDistanceIndex<Distance, N>`）と可変長版（`CentroidDistanceIndex<Distance>`）は同じヘッダで利用できる。
