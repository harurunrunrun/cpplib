---
title: Fully Persistent 2D Segment Tree
documentation_of: ../src/structure/segtree/persistent_segtree_2d.hpp
---

点更新ごとに新しいversionを作り、任意の過去versionから分岐できる固定容量二次元segment tree。

```cpp
PersistentSegtree2D<Monoid, MAX_HEIGHT, MAX_WIDTH, MAX_VERSION> seg(values);
```

`MAX_VERSION` は初期versionを除いて生成できるversion数。objectのcopy/moveはできない。

## 構築と形

```cpp
PersistentSegtree2D()
PersistentSegtree2D(height, width)
PersistentSegtree2D(values)

seg.height();
seg.width();
seg.empty();
```

整数指定とデフォルト構築では全要素を `Monoid.e()` にする。`values` は長方形でなければならない。実際の高さ・幅はtemplate上限以下とする。

## version生成

```cpp
int set(int version, int row, int col, const S& value);
int apply(int version, int row, int col, const S& value);
int fork(int version);
```

`set` は指定点を置換し、`apply` は現在値と `value` の `Monoid.op` を保存する。どちらも指定versionから新しいversionを作る。`fork` は要素を変更せず同じ状態の新versionを作る。返り値は生成したversion番号。

## query

```cpp
S get(int version, int row, int col) const;
S prod(
    int version,
    int row_begin,
    int col_begin,
    int row_end,
    int col_end
) const;
S all_prod(int version) const;

int versions() const;
int latest_version() const;
```

`prod` は半開矩形 `[row_begin,row_end) x [col_begin,col_end)` の積。version 0が初期状態で、有効versionは `[0,versions())`。

## 実装と容量

初期状態は密な二次元segment treeに保持する。更新ではouter row pathをcopyし、各row nodeに属するcolumn pathだけをcopyする。1更新あたりrow nodeを `log2(R)+1` 個、column nodeを `(log2(R)+1)(log2(C)+1)` 個使う。`fork` はnodeを使わない。

`R`, `C` をそれぞれ `MAX_HEIGHT`, `MAX_WIDTH` 以上の最小の2冪とする。保存容量は次の通り。

- 初期node: `4RC`
- row更新node: `MAX_VERSION * (log2(R)+1)`
- column更新node: `MAX_VERSION * (log2(R)+1)(log2(C)+1)`

## 計算量

| API | 時間計算量 |
| --- | --- |
| 各constructor | $O(RC+MAX\_VERSION\log R\log C)$ |
| `height`, `width`, `empty`, `versions`, `latest_version` | $O(1)$ |
| `set`, `apply` | $O(\log R\log C)$ |
| `fork` | $O(1)$ |
| `get` | $O(\log R+\log C)$ |
| `prod` | $O(\log R\log C)$ |
| `all_prod` | $O(1)$ |

保存領域は $O(RC+MAX\_VERSION\log R\log C)$。

## API契約・例外

二次元分割の結合順に依存しないよう、`Monoid.op` は結合的かつ可換で、`Monoid.e()` を単位元としなければならない。

不正な形、非長方形vector、version、点、半開矩形、version/node容量超過では `runtime_error`。失敗した更新でversion数とnode使用量は増えない。
