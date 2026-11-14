---
title: Partially Persistent 2D Segment Tree (部分永続二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_segtree_2d.hpp
---

過去の version を参照でき、最新 version だけを更新できる二次元セグメント木。完全永続版は使わず、更新された密なノードに時刻付きの履歴を追加する。

```cpp
PartiallyPersistentSegtree2D<Monoid, MAX_HEIGHT, MAX_WIDTH, MAX_UPDATE> seg(values);
```

`MAX_HEIGHT`, `MAX_WIDTH` は高さ・幅の上限、`MAX_UPDATE` は更新回数の上限である。オブジェクトのコピー・ムーブはできない。

容量（3 つのテンプレート引数）をすべて省略すると可変長版になり、`PartiallyPersistentSegtree2D<Monoid> tree(height, width);` のように大きさを実行時に与える。更新回数の上限はなく、履歴は使用量に応じて `std::vector` を拡張する。

## 構築と形

```cpp
PartiallyPersistentSegtree2D()
PartiallyPersistentSegtree2D(int height, int width)
explicit PartiallyPersistentSegtree2D(const std::vector<std::vector<S>>& values)

seg.height();
seg.width();
seg.empty();
```

デフォルトコンストラクタは $0 \times 0$ の空の木を作る。高さ・幅を指定した場合は全要素を `Monoid.e()` にする。`values` は長方形でなければならない。いずれも高さ・幅はテンプレート引数の上限以下である必要がある。

## 最新 version の更新

```cpp
int set(int row, int col, const S& value);
int apply(int row, int col, const S& value);
```

`set` は指定した点の値を置き換え、`apply` は最新の値と `value` の `Monoid.op` を格納する。最新 version から新しい version を作り、その番号を返す。過去の version を更新元にはできない。

## クエリ

```cpp
S get(int version, int row, int col) const;
S get(int row, int col) const;

S prod(int version, int top, int left, int bottom, int right) const;
S prod(int top, int left, int bottom, int right) const;

S all_prod(int version) const;
S all_prod() const;

int versions() const;
int latest_version() const;
```

`version` を省略した版は最新 version を参照する。`prod` は半開矩形 $[top, bottom) \times [left, right)$ の総積を返す。version 0 が初期状態である。

## 実装と保存領域

$R$, $C$ をそれぞれ高さ・幅の上限以上の最小の 2 冪とする。1 回の更新で $(\log_2 R+1)(\log_2 C+1)$ 個の密なノードに履歴を追加する。履歴の総容量は

$$
\texttt{MAX_UPDATE} \cdot (\log_2 R+1)(\log_2 C+1)
$$

であり、完全永続版のような外側・列方向の分岐ノードや version ごとの根は持たない。

## 時間計算量

| API | 時間計算量 |
| --- | --- |
| 各コンストラクタ | $O(RC+\texttt{MAX_UPDATE}\log R\log C)$ |
| `height`, `width`, `empty`, `versions`, `latest_version` | $O(1)$ |
| `set`, `apply` | $O(\log R\log C)$（履歴の追加はノードあたり償却 $O(1)$） |
| 最新 version の `get` | $O(1)$ |
| 過去 version の `get` | $O(\log(\texttt{MAX_UPDATE}+1))$ |
| 最新 version の `prod` | $O(\log R\log C)$ |
| 過去 version の `prod` | $O(\log R\log C\log(\texttt{MAX_UPDATE}+1))$ |
| 最新 version の `all_prod` | $O(1)$ |
| 過去 version の `all_prod` | $O(\log(\texttt{MAX_UPDATE}+1))$ |

保存領域は $O(RC+\texttt{MAX_UPDATE}\log R\log C)$ である。

## 注意点

- `Monoid.op` は結合的かつ可換で、`Monoid.e()` を単位元としなければならない。
- 不正な形、長方形でない `values`、不正な version・点・半開矩形、更新回数の超過では `std::runtime_error` を送出する。
- 失敗した更新では version 数と履歴の使用量は増えない。
