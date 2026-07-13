---
title: Lazy Link Cut Tree (遅延リンクカット木)
documentation_of: ../src/structure/tree/lazy_link_cut_tree.hpp
---

動的森に対して、辺の追加削除、連結判定、パス上の作用、パス積を行う。

# テンプレート引数

```cpp
LazyLinkCutTree<MonoidActLen, MAX_SIZE>
```

- 作用付きモノイド `MonoidActLen`
- 頂点数の上限 `MAX_SIZE`

`MonoidActLen` は次を持つ。

```cpp
S op(S a, long long len_a, S b, long long len_b)
S e()
S mapping(T f, S x, long long len)
T composition(T f, T g)
T id()
```

非可換モノイドにも対応する。

# コンストラクタ

```cpp
LazyLinkCutTree(int n = MAX_SIZE)
LazyLinkCutTree(const vector<S>& values)
```

頂点値を単位元、または `values` で初期化する。

## 時間計算量

- $O(\mathtt{MAX\_SIZE})$

固定容量のnode配列全体を初期化する。

# size / evert

```cpp
int size() const
void evert(int v)
```

`size`は頂点数を返す。`evert`は`v`をその表現木の根にする。

## 時間計算量

- `size`: $O(1)$
- `evert`: amortized $O(\log N)$

頂点引数は`[0,size())`でなければならず、範囲外は例外。
構築時の`n`または`values.size()`が`MAX_SIZE`を超える場合も例外。

# link / cut

```cpp
bool link(int u, int v)
bool cut(int u, int v)
```

`link` は辺 $(u,v)$ を追加する。すでに連結なら `false` を返す。

`cut` は辺 $(u,v)$ を削除する。辺がなければ `false` を返す。

## 時間計算量

- amortized $O(\log N)$

# 連結判定

```cpp
bool connected(int u, int v)
int root(int v)
```

## 時間計算量

- amortized $O(\log N)$

# 頂点値

```cpp
void set(int v, const S& x)
S get(int v)
```

## 時間計算量

- amortized $O(\log N)$

# パス作用

```cpp
void path_apply(int u, int v, const T& f)
```

`u` から `v` へのパス上の頂点値に `f` を作用させる。

## 時間計算量

- amortized $O(\log N)$

# パス積

```cpp
S path_prod(int u, int v)
```

`u` から `v` へのパス上の頂点値のモノイド積を返す。

`path_apply`と`path_prod`は`u`と`v`が連結であることを前提とし、
実装はこの前提を検査しない。
`link`は既に連結なら`false`、`cut`は指定した直接辺がなければ`false`。
その他の頂点引数の範囲違反は例外。

## 時間計算量

- amortized $O(\log N)$

# パス長

```cpp
int path_size(int u, int v)
```

`u` から `v` へのパスに含まれる頂点数を返す。同じ頂点なら 1、非連結なら -1。
頂点番号が `[0,size())` の範囲外なら例外を送出する。

## 時間計算量

- amortized $O(\log N)$

# パス上の k-th 頂点 / jump

```cpp
int kth_on_path(int u, int v, int k)
int jump(int u, int v, int k)
```

`u` から `v` へのパスを 0-indexed に並べたときの `k` 番目の頂点を返す。
したがって `k == 0` なら `u`、`k == path_size(u, v) - 1` なら `v` である。
`jump` は `kth_on_path` の別名である。

`k < 0`、`k >= path_size(u, v)`、または `u` と `v` が非連結なら -1 を返す。
頂点番号自体が範囲外の場合は他 API と同様に例外を送出する。
補助木の部分木 size を使って二分探索する。

## 時間計算量

- amortized $O(\log N)$

# API 別計算量

$N$ を頂点数とする。splay 操作を含む時間は償却計算量である。

| API | 説明 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- | --- |
| constructor | 固定容量を初期化 | $O(\mathtt{MAX\_SIZE})$ | $O(\mathtt{MAX\_SIZE})$ |
| `size` | 頂点数 | $O(1)$ | $O(1)$ |
| `evert` | 表現木の根を変更 | $O(\log N)$ amortized | $O(1)$ |
| `root` | 表現木の根を取得 | $O(\log N)$ amortized | $O(1)$ |
| `connected` | 連結判定 | $O(\log N)$ amortized | $O(1)$ |
| `link` | 辺を追加 | $O(\log N)$ amortized | $O(1)$ |
| `cut` | 直接辺を削除 | $O(\log N)$ amortized | $O(1)$ |
| `set` | 頂点値を更新 | $O(\log N)$ amortized | $O(1)$ |
| `get` | 頂点値を取得 | $O(\log N)$ amortized | $O(1)$ |
| `path_apply` | パス作用 | $O(\log N)$ amortized | $O(1)$ |
| `path_prod` | パス積 | $O(\log N)$ amortized | $O(1)$ |
| `path_size` | パスの頂点数 | $O(\log N)$ amortized | $O(1)$ |
| `kth_on_path`, `jump` | パス上の k-th 頂点 | $O(\log N)$ amortized | $O(1)$ |

# 空間計算量

node 配列、補助木 size、lazy・反転情報、splay 用 stack を含めて
$O(\mathtt{MAX\_SIZE})$。size 更新は各回転・accessごとに $O(1)$ であり、
既存 API の償却計算量を変えない。
