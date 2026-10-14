---
title: Partially Persistent 2D Segment Tree (部分永続二次元セグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_segtree_2d.hpp
---

過去versionを参照でき、最新versionだけを更新できる固定容量二次元segment tree。完全永続版を使用せず、更新されたdense nodeに時刻付き履歴を追加する。

```cpp
PartiallyPersistentSegtree2D<Monoid, MAX_HEIGHT, MAX_WIDTH, MAX_UPDATE> seg(values);
```

`MAX_UPDATE` は更新回数上限。objectのcopy/moveはできない。

## 構築と形

```cpp
PartiallyPersistentSegtree2D()
PartiallyPersistentSegtree2D(height, width)
PartiallyPersistentSegtree2D(values)

seg.height();
seg.width();
seg.empty();
```

整数指定とデフォルト構築では全要素を `Monoid.e()` にする。`values` は長方形で、実際の高さ・幅はtemplate上限以下でなければならない。

## 最新versionの更新

```cpp
int set(int row, int col, const S& value);
int apply(int row, int col, const S& value);
```

`set` は指定点を置換し、`apply` は最新値と `value` の `Monoid.op` を保存する。最新versionから新versionを作り、その番号を返す。過去versionを更新元にはできない。

## query

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

versionを省略したoverloadは最新versionを参照する。`prod` は半開矩形 `[top,bottom) x [left,right)` の積。version 0が初期状態。

## 実装と保存領域

`R`, `C` をtemplate上限以上の最小の2冪とする。1更新は `(log2(R)+1)(log2(C)+1)` 個のdense node履歴を追加する。履歴総容量は

```text
MAX_UPDATE * (log2(R)+1) * (log2(C)+1)
```

であり、完全永続版のouter/column分岐nodeやversion rootは持たない。

## 計算量

| API | 時間計算量 |
| --- | --- |
| 各constructor | $O(RC+MAX\_UPDATE\log R\log C)$ |
| `height`, `width`, `empty`, `versions`, `latest_version` | $O(1)$ |
| `set`, `apply` | $O(\log R\log C)$、履歴追加は償却 $O(1)$ / node |
| 最新versionの `get` | $O(1)$ |
| 過去versionの `get` | $O(\log(MAX\_UPDATE+1))$ |
| 最新versionの `prod` | $O(\log R\log C)$ |
| 過去versionの `prod` | $O(\log R\log C\log(MAX\_UPDATE+1))$ |
| 最新versionの `all_prod` | $O(1)$ |
| 過去versionの `all_prod` | $O(\log(MAX\_UPDATE+1))$ |

保存領域は $O(RC+MAX\_UPDATE\log R\log C)$。

## 注意点

`Monoid.op` は結合的かつ可換で、`Monoid.e()` を単位元としなければならない。

不正な形、非長方形vector、version、点、半開矩形、version/change容量超過では `runtime_error`。失敗した更新でversion数と履歴使用量は増えない。

## 容量を省略する場合

容量引数の既定値は `-1`。`PartiallyPersistentSegtree2D<Monoid> tree(height, width);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
