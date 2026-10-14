---
title: Partially Persistent Dual Segment Tree (部分永続双対セグメント木)
documentation_of: ../../../../src/structure/segtree/partially_persistent/partially_persistent_dualsegtree.hpp
---

過去の状態を参照でき、最新の状態のみ更新できるdual segment tree。状態は0から始まるversionで指定する。

## テンプレート引数

```cpp
PartiallyPersistentDualSegtree<Monoid_act, MAX_SIZE, MAX_UPDATE>
```

- 作用付きモノイド `Monoid_act`
- 配列長の上限 `MAX_SIZE`
- 更新回数の上限 `MAX_UPDATE`

## コンストラクタ

```cpp
(1) PartiallyPersistentDualSegtree(int n = MAX_SIZE)
(2) PartiallyPersistentDualSegtree(const vector<S>& v)
(3) PartiallyPersistentDualSegtree(const array<S, N>& v)
```

## 時間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$

## 更新

```cpp
int set(int k, const S& x)
int apply(int l, int r, const T& f)
```

最新versionを更新した新しいversionを作り、その番号を返す。

## 時間計算量

- $O(\log \mathrm{MAX\_SIZE})$

## get

```cpp
S get(int version, int k) const
S get(int k) const
```

1. 指定versionの値を返す。
2. 最新versionの値を返す。

## 時間計算量

- (1): $O(\log \mathrm{MAX\_SIZE}\log(\mathrm{MAX\_SIZE}\,\mathrm{MAX\_UPDATE}))$
- (2): $O(\log \mathrm{MAX\_SIZE})$

## version情報

```cpp
int size() const
int versions() const
int latest_version() const
```

## 時間計算量

- $O(1)$

## 空間計算量

- $O(\mathrm{MAX\_SIZE}+\mathrm{MAX\_UPDATE}\log \mathrm{MAX\_SIZE})$

## 注意点

version 0が初期状態で、queryは `0 <= version < versions()` の任意versionを参照できる。
更新は `latest_version()` だけを元に行い、新しいversion番号を返す。過去versionを更新元に
指定すると `runtime_error`。点は列内、区間は半開区間 `[l,r)` である。
境界探索のpredicateは単調かつ単位元に対してtrueが必要。

不正なversion・点・区間・predicate、version/node/change容量超過では `runtime_error`。
失敗した更新はversion・node・履歴使用量を更新前へ戻す。各APIの計算量は上記表の通り。

## 容量を省略する場合

容量引数の既定値は `-1`。`PartiallyPersistentDualSegtree<Monoid_act> tree(n);` のように容量を省略すると、同じヘッダ内の可変長版を使用する。既存の非負容量を指定する固定配列版も利用できる。 モノイド・係数型・剰余の数・分岐数など、容量以外のテンプレート引数は引き続き指定する。

木の幅・高さは実行時の入力から求め、通常の密な木は要素数に比例する記憶領域を確保する。疎な木のノード、永続版の版情報、更新・rollbackの履歴は使用量に応じてvectorを拡張する。固定配列版と同じ更新・照会アルゴリズムを用い、拡張に伴う再配置は償却計算量に含める。整数添字の表現範囲と実メモリ容量は上限となる。
