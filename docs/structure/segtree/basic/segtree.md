---
title: Segment Tree (セグメント木) [DSL_2_A]
documentation_of: ../../../../src/structure/segtree/basic/segtree.hpp
---

モノイドの列について、1点更新、区間に対する演算を $O(\log n)$ で行う。

indexはすべて0-indexed。

## コンストラクタ

```cpp
(1) Segtree<auto Monoid, int MAX_SIZE>(int n = MAX_SIZE)
(2) Segtree<auto Monoid, int MAX_SIZE>(const vector<S>& v)
```
- モノイド `Monoid`
- モノイドの型 `S`
- 配列の最大制約 `MAX_SIZE`

1. モノイド `Monoid`、サイズ `n` で初期化する。各要素は単位元 `Monoid.e()`。
2. モノイド `Monoid`、配列 `v` で初期化。

ここで使用する `Monoid` は `constexpr` である必要がある。

例えば Range Minimum Query のモノイドは以下のようになる。

```cpp
int op(int a, int b){
    return min(a,b);
}

int e(){
    return 1<<30;
}

constexpr Monoid<op,e> monoid;
```

## 時間計算量

- $O(\mathtt{MAX\_SIZE})$

固定容量のtree配列全体を単位元で初期化する。

## set

```cpp
void set(int k, const S& x)
```

`k` 番目の要素を `x` に変更する。

## 時間計算量

- $O(\log n)$


## get

```cpp
S get(int k)
```

`k` 番目の要素を取得する。

## 時間計算量

- $O(1)$

## prod

```cpp
S prod(int l, int r)
```

区間 `[l,r)` の演算結果を返す。

## 時間計算量

- $O(\log n)$

## all_prod

```cpp
S all_prod()
```

区間 `[0,n)`の演算結果を返す。

## 時間計算量

- $O(1)$

## max_right

```cpp
int max_right(int l, auto f, auto&&... args)
```

関数 `bool f(S x, auto args)` を定義し、segtree上で二分探索をする。

`args` は `f` で用いる可変引数。

`f` を単調としたとき、 `f(prod(l,r), args...)= true` となる最大の `r` を返す。

`f(Monoid.e(), args...) = true` が必要。

## 時間計算量

- $O(\log n)$

## min_left

```cpp
int min_left(int r, auto f, auto&&... args)
```

関数 `bool f(S x, auto args)` を定義し、segtree上で二分探索をする。

`args` は `f` で用いる可変引数。

`f` を単調としたとき、 `f(prod(l,r), args...)= true` となる最小の `l` を返す。

`f(Monoid.e(), args...) = true` が必要。

## 時間計算量

- $O(\log n)$

## 注意点

実際の列長を (n) とする。コンストラクタの `n` と `v.size()` は
`MAX_SIZE` 以下でなければならない。点 `k` は `0 <= k < n`、
区間は `0 <= l <= r <= n` の半開区間 `[l,r)` で指定する。
境界探索のpredicateは単調で、`f(Monoid.e(), args...) == true` が必要である。
容量、添字、区間またはpredicateの前提違反では `runtime_error` を送出する。

## 容量を省略する場合

容量引数の既定値は `-1`。`Segtree<Monoid> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
