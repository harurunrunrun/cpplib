---
title: Dynamic Segment Tree (動的セグメント木)
documentation_of: ../../../../src/structure/segtree/dynamic/dynamic_segtree.hpp
---

長さの大きいモノイドの列について、必要な部分だけノードを作成して、1 点の更新と区間積を $O(\log \texttt{MAX_SIZE})$ で行う。

添字はすべて 0-indexed である。

## コンストラクタ

```cpp
template<auto Monoid, long long MAX_SIZE = -1>
struct DynamicSegtree;

explicit DynamicSegtree()
```

- `Monoid`: モノイド
- `MAX_SIZE`: 列の長さ

長さ `MAX_SIZE` の列として初期化する。各要素は単位元 `Monoid.e()` である。

`Monoid` は `constexpr` である必要がある。

例えば区間最小値のモノイドは次のように書ける。

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

- $O(\log \texttt{MAX_SIZE})$

新しく確保するノード数も $O(\log \texttt{MAX_SIZE})$ である。


## get

```cpp
S get(long long k) const
```

`k` 番目の要素を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## prod

```cpp
S prod(long long l, long long r) const
```

区間 `[l, r)` の演算結果を返す。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## all_prod

```cpp
S all_prod() const
```

区間 `[0, MAX_SIZE)` の演算結果を返す。

## 時間計算量

- $O(1)$

## max_right

```cpp
long long max_right(long long l, auto f, auto&&... args) const
```

関数 `bool f(S x, auto... args)` を使って、セグメント木上で二分探索をする。`args` は `f` に渡す可変長引数である。

`f` が単調であるとき、`f(prod(l, r), args...) == true` となる最大の `r` を返す。

`f(Monoid.e(), args...) == true` が必要である。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## min_left

```cpp
long long min_left(long long r, auto f, auto&&... args) const
```

関数 `bool f(S x, auto... args)` を使って、セグメント木上で二分探索をする。`args` は `f` に渡す可変長引数である。

`f` が単調であるとき、`f(prod(l, r), args...) == true` となる最小の `l` を返す。

`f(Monoid.e(), args...) == true` が必要である。

## 時間計算量

- $O(\log \texttt{MAX_SIZE})$

## 注意点

- 添字の範囲は `[0, MAX_SIZE)` で、まだ生成されていない点の値は `Monoid.e()` である。
- 境界探索の述語は単調で、単位元に対して `true` でなければならない。
- 点・区間の範囲違反や述語の前提違反では `std::runtime_error` を、ノードの確保に失敗すると `std::bad_alloc` を送出する。
- コピーはできない。ムーブコンストラクタ・ムーブ代入も宣言されていないので、ムーブもできない。

## 容量を省略する場合

`MAX_SIZE` のデフォルトは `-1` である。`DynamicSegtree<Monoid> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使い、添字の範囲は `[0, n)` になる（コンストラクタは `explicit DynamicSegtree(long long n = 0)`）。上記の計算量の $\texttt{MAX_SIZE}$ は $n$ に置き換わる。
