---
title: Link Cut Tree (リンクカット木)
documentation_of: ../src/structure/tree/link_cut_tree.hpp
---

動的森に対して、辺の追加削除、連結判定、パス上のモノイド積を行う。

# テンプレート引数

```cpp
LinkCutTree<Monoid, MAX_SIZE>
```

- モノイド `Monoid`
- 頂点数の上限 `MAX_SIZE`

`Monoid` は `S op(S, S)` と `S e()` を持つ。非可換モノイドにも対応する。

# コンストラクタ

```cpp
LinkCutTree(int n = MAX_SIZE)
LinkCutTree(const vector<S>& values)
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

# パス積

```cpp
S path_prod(int u, int v)
```

`u` から `v` へのパス上の頂点値のモノイド積を返す。

`u`と`v`が連結であることを前提とする。実装はこの前提を検査しない。
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
| `path_prod` | パス積 | $O(\log N)$ amortized | $O(1)$ |
| `path_size` | パスの頂点数 | $O(\log N)$ amortized | $O(1)$ |
| `kth_on_path`, `jump` | パス上の k-th 頂点 | $O(\log N)$ amortized | $O(1)$ |

# 空間計算量

node 配列、補助木 size、splay 用 stack を含めて $O(\mathtt{MAX\_SIZE})$。
既存操作に追加される size の更新は各回転・accessごとに $O(1)$ であり、
既存 API の償却計算量を変えない。

## 注意点

`0 <= n <= MAX_SIZE`、全頂点引数は `[0,n)`。範囲外、非連結なpath query、無効な
`k` / `jump` 距離では `runtime_error`。`link` は既に連結ならfalse、`cut` は直接辺が
なければfalseを返す。`evert` により非可換積の向きが変わる。
保存領域は $O(\mathtt{MAX\_SIZE})$。各公開操作は反復splayで、返却値以外の追加領域は
$O(1)$。
