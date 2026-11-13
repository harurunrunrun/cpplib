---
title: Range Parallel Union-Find (区間並列Union-Find)
documentation_of: ../../../../src/structure/dsu/range/range_parallel_unionfind.hpp
---

同じ長さの 2 区間を位置ごとに対応させ、対応する頂点どうしを Union-Find で併合する。
長さ $2^k$ の区間ごとに補助の Union-Find を持ち、一度処理した区間の組については再帰しない。

## テンプレート引数・コンストラクタ

```cpp
RangeParallelUnionFind<MAX_SIZE = -1> dsu(n)
```

- `MAX_SIZE`: 頂点数の上限。省略（`-1`）すると、実行時の頂点数に合わせて `std::vector` を確保する可変長版になる
- `n`: 実際の頂点数（可変長版でのデフォルトは $0$）

コピーはできず、ムーブはできる。

```cpp
using Dynamic = RangeParallelUnionFind<>;
```

## 1 組の併合

```cpp
int merge(int u, int v)
int merge(int u, int v, MergeCallback callback)
```

頂点 $u$, $v$ を併合し、新たに起きた頂点成分の併合回数（$0$ または $1$）を返す。
実際に異なる成分を併合したとき、`callback(new_root, absorbed_root)` を 1 回呼ぶ。呼び出し時点で `new_root` は併合後の代表元になっている。

## 区間の並列併合

```cpp
int merge_ranges(int l1, int r1, int l2, int r2)
int merge_ranges(int l1, int r1, int l2, int r2, MergeCallback callback)
```

半開区間 $[l_1, r_1)$ と $[l_2, r_2)$ について、$r_1 - l_1 = r_2 - l_2$ を要求し、各 $0 \le i \lt r_1 - l_1$ について $l_1 + i$ と $l_2 + i$ を併合する。
戻り値は新たに起きた頂点成分の併合回数で、`callback` は `merge` と同様に、異なる頂点成分を併合するたびに呼ばれる。

## 検索

```cpp
int leader(int v)
bool same(int u, int v)
int component_size(int v)
int groups() const
int size() const
```

代表元、同じ成分かどうか、成分の頂点数、成分数、頂点数を返す。

## 時間計算量

$N$ を頂点数とする。

- コンストラクタ: $O(N \log N)$
- $Q$ 回の `merge`・`merge_ranges` 全体で

$$O((N\log N+Q)\alpha(N))$$

  1 回の操作では、新たに展開される補助区間の数に比例する。
- `leader`、`same`、`component_size`: 償却 $O(\alpha(N))$
- `groups`、`size`: $O(1)$

## 空間計算量

- 本体: $O(N \log N)$（固定長版では $O(\texttt{MAX_SIZE} \log \texttt{MAX_SIZE})$）
- `leader` などの検索の追加領域: $O(1)$
- `merge_ranges` の再帰スタック: $O(\log N)$

## 注意点

- $0 \le n \le \texttt{MAX_SIZE}$ で、頂点は $[0, n)$ でなければならない。区間は半開区間で指定し、同じ長さでなければならない。違反すると `std::runtime_error` を送出する。
- `callback` は実際に異なる頂点成分を併合した場合だけ呼ばれる。第 1 引数が新しい代表元、第 2 引数が吸収された代表元である。
