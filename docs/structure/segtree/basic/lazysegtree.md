---
title: Lazy Segment Tree (遅延セグメント木)
documentation_of: ../../../../src/structure/segtree/basic/lazysegtree.hpp
---

作用付きモノイドに対する区間作用・区間積。

## 時間計算量

- `LazySegtree(int)`, `LazySegtree(vector)`: $O(\mathtt{MAX\_SIZE})$
- `set`, `get`, 1点`apply`, 区間`apply`, `prod`: $O(\log \mathtt{MAX\_SIZE})$
- `max_right`, `min_left`: $O(\log \mathtt{MAX\_SIZE})$
- `all_prod`: $O(1)$

## API

```cpp
LazySegtree<Monoid, MAX_SIZE>(int n = MAX_SIZE)
LazySegtree<Monoid, MAX_SIZE>(const vector<S>& values)
void set(int k, const S& value)
S get(int k)
S prod(int l, int r)
S all_prod()
void apply(int k, const F& action)
void apply(int l, int r, const F& action)
int max_right(int l, auto predicate, auto&&... args)
int min_left(int r, auto predicate, auto&&... args)
```

`set/get` は1点、`prod` は半開区間 `[l,r)`、`all_prod` は全体を扱う。
`apply` は1点または指定半開区間に作用する。境界探索のpredicateは単調で、
単位元に対してtrueが必要である。`n <= MAX_SIZE`、点・区間が列内であることが前提。
容量・範囲・predicate条件の違反では `runtime_error`。各APIの計算量は上表の通り。

## 注意点

`0 <= n <= MAX_SIZE`、点は `[0,n)`、区間は `[l,r)`。範囲・容量違反では
`runtime_error`。境界探索のpredicateは単調で単位元に対してtrueが必要。
`Monoid` は作用付きモノイドの契約を満たす。保存領域は
$O(\mathtt{MAX\_SIZE})$、各更新・queryの追加空間計算量は $O(1)$。

## 容量を省略する場合

容量引数の既定値は `-1`。`LazySegtree<Monoid_act> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
