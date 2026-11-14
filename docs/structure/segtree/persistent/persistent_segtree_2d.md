---
title: Fully Persistent 2D Segment Tree (完全永続二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/persistent/persistent_segtree_2d.hpp
---

点更新ごとに新しい version を作り、任意の過去の version から分岐できる二次元セグメント木。

```cpp
PersistentSegtree2D<Monoid, MAX_HEIGHT, MAX_WIDTH, MAX_VERSION> seg(values);
```

`MAX_HEIGHT`, `MAX_WIDTH` は高さ・幅の上限、`MAX_VERSION` は初期 version を除いて生成できる version 数である。オブジェクトのコピー・ムーブはできない。

容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`PersistentSegtree2D<Monoid> tree(height, width);` のように大きさを実行時に与える。ノードと version は使用量に応じて `std::vector` を拡張する。

## 構築と形

```cpp
PersistentSegtree2D()
PersistentSegtree2D(int height, int width)
explicit PersistentSegtree2D(const std::vector<std::vector<S>>& values)

seg.height();
seg.width();
seg.empty();
```

デフォルトコンストラクタは $0 \times 0$ の空の木を作る。高さ・幅を指定した場合は全要素を `Monoid.e()` にする。`values` は長方形でなければならない。いずれも高さ・幅はテンプレート引数の上限以下とする。

## version の生成

```cpp
int set(int version, int row, int col, const S& value);
int apply(int version, int row, int col, const S& value);
int fork(int version);
```

`set` は指定した点の値を置き換え、`apply` は現在の値と `value` の `Monoid.op` を格納する。どちらも指定 version から新しい version を作る。`fork` は要素を変更せず、同じ状態の新しい version を作る。戻り値は生成した version 番号である。

## クエリ

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

`prod` は半開矩形 `[row_begin, row_end) x [col_begin, col_end)` の総積を返す。version 0 が初期状態で、有効な version は $[0, \texttt{versions()})$ である。

## 実装と保存領域

初期状態は密な二次元セグメント木に保持する。更新では外側（行方向）の経路を複製し、各行ノードに属する列方向の経路だけを複製する。$R$, $C$ をそれぞれ `MAX_HEIGHT`, `MAX_WIDTH` 以上の最小の 2 冪とすると、1 回の更新で行ノードを $\log_2 R+1$ 個、列ノードを $(\log_2 R+1)(\log_2 C+1)$ 個使う。`fork` はノードを使わない。保存容量は次の通りである。

- 初期ノード: $4RC$
- 行方向の更新ノード: $\texttt{MAX_VERSION} \cdot (\log_2 R+1)$
- 列方向の更新ノード: $\texttt{MAX_VERSION} \cdot (\log_2 R+1)(\log_2 C+1)$

## 時間計算量

| API | 時間計算量 |
| --- | --- |
| 各コンストラクタ | $O(RC+\texttt{MAX_VERSION}\log R\log C)$ |
| `height`, `width`, `empty`, `versions`, `latest_version` | $O(1)$ |
| `set`, `apply` | $O(\log R\log C)$ |
| `fork` | $O(1)$ |
| `get` | $O(\log R+\log C)$ |
| `prod` | $O(\log R\log C)$ |
| `all_prod` | $O(1)$ |

保存領域は $O(RC+\texttt{MAX_VERSION}\log R\log C)$ である。

## 注意点

- 二次元分割の結合順に依存しないよう、`Monoid.op` は結合的かつ可換で、`Monoid.e()` を単位元としなければならない。
- 不正な形、長方形でない `values`、不正な version・点・半開矩形、version・ノードの容量超過では `std::runtime_error` を送出する。
- 失敗した更新では version 数とノード使用量は増えない。
