---
title: Lazy Link Cut Tree (遅延リンクカット木)
documentation_of: ../../../../src/structure/tree/dynamic_forest/lazy_link_cut_tree.hpp
---

動的な森に対して、辺の追加・削除、連結判定、パス上の頂点値への作用、パス上のモノイド積の取得を行う。

## テンプレート引数

```cpp
LazyLinkCutTree<MonoidActLen, MAX_SIZE>
```

- `MonoidActLen`: 長さ付き作用モノイドのオブジェクト（非型テンプレート引数）
- `MAX_SIZE`: 頂点数の上限。省略するとデフォルト値 `-1` の可変長版になる

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

非可換モノイドにも対応する。結合・写像・合成は長さ付き作用モノイドの規約を満たす必要がある。

## コンストラクタ

```cpp
LazyLinkCutTree(int n = MAX_SIZE)
LazyLinkCutTree(const vector<S>& values)
```

`n` 個の孤立頂点を作り、頂点値を単位元、または `values` で初期化する。可変長版では `n` のデフォルト値は `0` である。
固定長版は容量分のノード配列全体を初期化する。

## `size` / `evert`

```cpp
int size() const
void evert(int v)
```

`size` は頂点数を返す。`evert` は `v` をその表現木の根にする。

## `link` / `cut`

```cpp
bool link(int u, int v)
bool cut(int u, int v)
```

`link` は辺 $(u,v)$ を追加して `true` を返す。既に連結なら何もせず `false` を返す。

`cut` は辺 $(u,v)$ を削除して `true` を返す。直接の辺がなければ `false` を返す。

## 連結判定

```cpp
bool connected(int u, int v)
int root(int v)
```

`connected` は 2 頂点が同じ木に属するかを返す。`root` は `v` を含む表現木の根を返す。

## 頂点値

```cpp
void set(int v, const S& x)
S get(int v)
```

## パス作用

```cpp
void path_apply(int u, int v, const T& f)
```

`u` から `v` へのパス上の頂点値に `f` を作用させる。

## パス積

```cpp
S path_prod(int u, int v)
```

`u` から `v` へのパス上の頂点値のモノイド積を（`u` 側から順に）返す。

## パス長

```cpp
int path_size(int u, int v)
```

`u` から `v` へのパスに含まれる頂点数を返す。同じ頂点なら `1`、非連結なら `-1` を返す。

## パス上の $k$ 番目の頂点 / `jump`

```cpp
int kth_on_path(int u, int v, int k)
int jump(int u, int v, int k)
```

`u` から `v` へのパスを 0-indexed で並べたときの `k` 番目の頂点を返す。
したがって `k == 0` なら `u`、`k == path_size(u, v) - 1` なら `v` である。
`jump` は `kth_on_path` の別名である。

`k < 0`、`k >= path_size(u, v)`、または `u` と `v` が非連結なら `-1` を返す。
補助木の部分木サイズを使って二分探索する。

## 時間計算量

$N$ を頂点数とする。splay 操作を含む時間はならし計算量である。

| API | 説明 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- | --- |
| コンストラクタ | 配列を初期化 | 固定長版 $O(\texttt{MAX_SIZE})$、可変長版 $O(N)$ | 同左 |
| `size` | 頂点数 | $O(1)$ | $O(1)$ |
| `evert` | 表現木の根を変更 | ならし $O(\log N)$ | $O(1)$ |
| `root` | 表現木の根を取得 | ならし $O(\log N)$ | $O(1)$ |
| `connected` | 連結判定 | ならし $O(\log N)$ | $O(1)$ |
| `link` | 辺を追加 | ならし $O(\log N)$ | $O(1)$ |
| `cut` | 直接の辺を削除 | ならし $O(\log N)$ | $O(1)$ |
| `set` | 頂点値を更新 | ならし $O(\log N)$ | $O(1)$ |
| `get` | 頂点値を取得 | ならし $O(\log N)$ | $O(1)$ |
| `path_apply` | パス作用 | ならし $O(\log N)$ | $O(1)$ |
| `path_prod` | パス積 | ならし $O(\log N)$ | $O(1)$ |
| `path_size` | パスの頂点数 | ならし $O(\log N)$ | $O(1)$ |
| `kth_on_path`, `jump` | パス上の $k$ 番目の頂点 | ならし $O(\log N)$ | $O(1)$ |
| `add_vertex`（可変長版のみ） | 孤立頂点を追加 | ならし $O(1)$ | ならし $O(1)$ |

## 空間計算量

ノード配列、補助木のサイズ、遅延作用・反転の情報、splay 用のスタックを含めて、固定長版は $O(\texttt{MAX_SIZE})$、可変長版は $O(N)$ である。
各公開操作は反復的な splay で実装され、戻り値以外の追加領域は $O(1)$ である。

## 注意点

- `0 <= n <= MAX_SIZE` で、全頂点引数は `[0, n)` でなければならない。範囲外なら `runtime_error` を送出する（構築時の `n` や `values.size()` が `MAX_SIZE` を超える場合も同様）。
- `path_apply` と `path_prod` は `u` と `v` が連結であることを前提とし、実装はこの前提を検査しない。非連結な 2 頂点を渡した場合の結果は未定義である（`path_size`, `kth_on_path`, `jump` は非連結なら `-1` を返す）。
- `evert` により非可換な積の向きが変わる。
- 可変長版（`MAX_SIZE` 省略、デフォルト値 `-1`）だけが `add_vertex(value = e())` を提供し、新しい孤立頂点の番号を返す。既存頂点間の辺と値は保持される。
- コピーはできず、ムーブはできる。容量を指定した固定配列版（`LazyLinkCutTree<..., N>`）と可変長版は同じヘッダで利用できる。
