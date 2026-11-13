---
title: Partially Persistent Dual Segment Tree (部分永続双対セグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_dualsegtree.hpp
---

過去の状態を参照でき、最新の状態だけを更新できる双対セグメント木である。状態は 0 から始まるバージョン番号で指定する。

## テンプレート引数

```cpp
template<auto Monoid_act, int MAX_SIZE = -1, int MAX_UPDATE = -1>
struct PartiallyPersistentDualSegtree;
```

- `Monoid_act`: 作用付きモノイド
- `MAX_SIZE`: 配列の長さの上限
- `MAX_UPDATE`: 更新回数の上限

## コンストラクタ

```cpp
(1) explicit PartiallyPersistentDualSegtree(int n = MAX_SIZE)
(2) explicit PartiallyPersistentDualSegtree(const vector<S>& v)
(3) explicit PartiallyPersistentDualSegtree(const array<S, N>& v)
```

1. 長さ `n` の単位元の列で初期化する。
2. `v` で初期化する。
3. 固定長配列 `v` で初期化する。`N <= MAX_SIZE` は `static_assert` で検査する。

初期状態がバージョン 0 になる。

時間計算量: $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE}\log \texttt{MAX_SIZE})$

## 更新

```cpp
int set(int k, const S& x)
int apply(int l, int r, const T& f)
```

`set` は `k` 番目の要素を `x` に置き換え、`apply` は半開区間 `[l, r)` に作用 `f` を適用する。どちらも最新のバージョンを更新した新しいバージョンを作り、その番号を返す。

時間計算量: $O(\log \texttt{MAX_SIZE})$

## get

```cpp
(1) S get(int version, int k) const
(2) S get(int k) const
```

1. 指定したバージョンの `k` 番目の値を返す。
2. 最新のバージョンの `k` 番目の値を返す。

時間計算量:

- (1): $O(\log \texttt{MAX_SIZE}\log(\texttt{MAX_SIZE}\,\texttt{MAX_UPDATE}))$
- (2): $O(\log \texttt{MAX_SIZE})$

## バージョン情報

```cpp
int size() const
int versions() const
int latest_version() const
```

列の長さ、バージョン数、最新のバージョン番号を返す。時間計算量は $O(1)$。

## 空間計算量

- $O(\texttt{MAX_SIZE}+\texttt{MAX_UPDATE}\log \texttt{MAX_SIZE})$

## 注意点

- バージョン 0 が初期状態で、`get` は `0 <= version < versions()` の任意のバージョンを参照できる。
- 更新は常に `latest_version()` を元に行い、新しいバージョン番号を返す。
- 点は `0 <= k < size()`、区間は半開区間 `[l, r)` で `0 <= l <= r <= size()` を満たす必要がある。
- 不正なバージョン・点・区間や、バージョン・ノード・変更履歴の容量超過では `std::runtime_error` を送出する。失敗した更新は、バージョン・ノード・履歴の使用量を更新前に戻す。
- コピー・ムーブはできない。

## 容量を省略する場合

容量のテンプレート引数のデフォルトは `-1` である。`PartiallyPersistentDualSegtree<Monoid_act> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使い、変更履歴などを使用量に応じて `std::vector` で拡張する。上記の計算量の $\texttt{MAX_SIZE}$ は $n$ に、$\texttt{MAX_UPDATE}$ は実際の更新回数に置き換わる。
