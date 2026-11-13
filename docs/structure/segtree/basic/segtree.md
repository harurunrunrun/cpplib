---
title: Segment Tree (セグメント木) [DSL_2_A]
documentation_of: ../../../../src/structure/segtree/basic/segtree.hpp
---

モノイドの列について、1 点の更新と区間積を $O(\log n)$ で行う。

添字はすべて 0-indexed である。

## コンストラクタ

```cpp
template<auto Monoid, int MAX_SIZE = -1>
struct Segtree;

(1) explicit Segtree(int n = MAX_SIZE)
(2) explicit Segtree(const vector<S>& v)
```

- `Monoid`: モノイド
- `S`: モノイドの値の型
- `MAX_SIZE`: 列の長さの上限

1. 長さ `n` で初期化する。各要素は単位元 `Monoid.e()` である。
2. 配列 `v` で初期化する。

`Monoid` は `constexpr` である必要がある。

例えば区間最小値のモノイドは次のように書ける。

```cpp
int op(int a, int b){
    return min(a,b);
}

int e(){
    return 1<<30;
}

constexpr Monoid<op,e> monoid;
```

### 時間計算量

- $O(\texttt{MAX_SIZE})$

固定容量の木の配列全体を単位元で初期化する。

以下、$M$ を `MAX_SIZE` 以上の最小の 2 冪とする（容量省略時は $n$ 以上の最小の 2 冪）。

## set

```cpp
void set(int k, const S& x)
```

`k` 番目の要素を `x` に変更する。

時間計算量: $O(\log M)$

## get

```cpp
S get(int k) const
```

`k` 番目の要素を返す。

時間計算量: $O(1)$

## prod

```cpp
S prod(int l, int r) const
```

区間 `[l, r)` の演算結果を返す。

時間計算量: $O(\log M)$

## all_prod

```cpp
S all_prod() const
```

区間 `[0, n)` の演算結果を返す。

時間計算量: $O(1)$

## max_right

```cpp
int max_right(int l, auto f, auto&&... args) const
```

関数 `bool f(S x, auto... args)` を使って、セグメント木上で二分探索をする。`args` は `f` に渡す可変長引数である。

`f` が単調であるとき、`f(prod(l, r), args...) == true` となる最大の `r` を返す。

`f(Monoid.e(), args...) == true` が必要である。

時間計算量: $O(\log M)$

## min_left

```cpp
int min_left(int r, auto f, auto&&... args) const
```

関数 `bool f(S x, auto... args)` を使って、セグメント木上で二分探索をする。`args` は `f` に渡す可変長引数である。

`f` が単調であるとき、`f(prod(l, r), args...) == true` となる最小の `l` を返す。

`f(Monoid.e(), args...) == true` が必要である。

時間計算量: $O(\log M)$

## 注意点

- 実際の列の長さを $n$ とする。コンストラクタの `n` と `v.size()` は `MAX_SIZE` 以下でなければならない。
- 点 `k` は `0 <= k < n`、区間は `0 <= l <= r <= n` の半開区間 `[l, r)` で指定する。
- 境界探索の述語は単調で、`f(Monoid.e(), args...) == true` を満たす必要がある。
- 容量、添字、区間、または述語の前提に違反すると `std::runtime_error` を送出する。

## 容量を省略する場合

容量のテンプレート引数のデフォルトは `-1` である。`Segtree<Monoid> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使う。非負の容量を指定する固定長配列版も利用できる。容量以外のテンプレート引数（モノイドなど）は引き続き指定する。

可変長版では木の幅・高さを実行時の入力から求め、要素数に比例する記憶領域を確保する。更新・クエリのアルゴリズムは固定長配列版と同じであり、上記の計算量の $\texttt{MAX_SIZE}$ は実際の要素数 $n$ に置き換わる。添字に使う整数型の表現範囲と実メモリの容量が上限となる。
