---
title: Dynamic Segment Tree (動的セグメント木)
documentation_of: ../src/structure/segtree/dynamic_segtree.hpp
---

巨大なモノイドの列について、必要なところだけノードを作成して、1点更新、区間に対する演算を $O(\log {MAX}\_{SIZE})$ で行う。

indexはすべて0-indexed。

## コンストラクタ

```cpp
DynamicSegtree<auto Monoid, long long MAX_SIZE>()
```
- モノイド `Monoid`
- 配列の最大制約 `MAX_SIZE`

モノイド `Monoid`、サイズ `MAX_SIZE` で初期化する。各要素は単位元 `Monoid.e()`。

ここで使用する `Monoid` は `constexpr` である必要がある。

例えば Range Minimum Query のモノイドは以下のようになる。

```cpp
long long op(long long a, long long b){
    return min(a,b);
}

long long e(){
    return 1LL<<60;
}

constexpr Monoid<op,e> monoid;
```

## 時間計算量

- $O(1)$

## set

```cpp
void set(long long k, const S& x)
```

`k` 番目の要素を `x` に変更する。

## 時間計算量

- $O(\log {MAX}\_{SIZE})$

新規に確保するnode数も $O(\log {MAX}\_{SIZE})$。


## get

```cpp
S get(long long k)
```

`k` 番目の要素を取得する。

## 時間計算量

- $O(\log {MAX}\_{SIZE})$

## prod

```cpp
S prod(long long l, long long r)
```

区間 `[l,r)` の演算結果を返す。

## 時間計算量

- $O(\log {MAX}\_{SIZE})$

## all_prod

```cpp
S all_prod()
```

区間 `[0,MAX_SIZE)`の演算結果を返す。

## 時間計算量

- $O(1)$

## max_right

```cpp
long long max_right(long long l, auto f, auto&&... args)
```

関数 `bool f(S x, auto args)` を定義し、segtree上で二分探索をする。

`args` は `f` で用いる可変引数。

`f` を単調としたとき、 `f(prod(l,r), args...)= true` となる最大の `r` を返す。

`f(Monoid.e(), args...) = true` が必要。

## 時間計算量

- $O(\log {MAX}\_{SIZE})$

## min_left

```cpp
long long min_left(long long r, auto f, auto&&... args)
```

関数 `bool f(S x, auto args)` を定義し、segtree上で二分探索をする。

`args` は `f` で用いる可変引数。

`f` を単調としたとき、 `f(prod(l,r), args...)= true` となる最小の `l` を返す。

`f(Monoid.e(), args...) = true` が必要。

## 時間計算量

- $O(\log {MAX}\_{SIZE})$

## 注意点

領域は `[0,MAX_SIZE)`、未生成点は `Monoid.e()`。`set/get` は1点、
`prod` は半開区間、`all_prod` は全域積を返す。境界探索のpredicateは単調かつ
単位元に対してtrueが必要である。点・区間・predicate違反は `runtime_error`、
node確保失敗は `bad_alloc`。copyは禁止、moveは可能。各APIの計算量は上記の通り。
