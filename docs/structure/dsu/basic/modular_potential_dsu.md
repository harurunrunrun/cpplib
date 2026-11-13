---
title: Modular Potential DSU (剰余ポテンシャル付き素集合データ構造) [unionfind_with_potential]
documentation_of: ../../../../src/structure/dsu/basic/modular_potential_dsu.hpp
---

ポテンシャルの差を法 `MOD` で管理する重み付き素集合データ構造。剰余を表す値型と、頂点の順序の変換を内部に持つ（内部では `WeightedDSU` を使う）。

## テンプレート引数

```cpp
ModularPotentialDSU<MOD, MAX_SIZE = -1>
```

- `MOD`: 法（正の `int`）
- `MAX_SIZE`: 頂点数の上限。省略（`-1`）すると、実行時の頂点数に合わせて `std::vector` を確保する可変長版になる。

```cpp
ModularPotentialDSU<3> dsu(n);  // 可変長版
```

## コンストラクタ

```cpp
explicit ModularPotentialDSU(int n = MAX_SIZE)
```

$n$ 個の頂点を作る。可変長版では `n` のデフォルトは $0$ である。

## merge

```cpp
bool merge(int u, int v, long long difference)
```

制約 $\mathrm{potential}(u) - \mathrm{potential}(v) \equiv \mathrm{difference} \pmod{\mathrm{MOD}}$ を追加する。既存の制約と整合する場合は `true`、矛盾する場合は `false` を返す。
`difference` は負でもよい。

## difference

```cpp
optional<int> difference(int u, int v)
```

連結なら $\mathrm{potential}(u) - \mathrm{potential}(v)$ を $[0, \mathrm{MOD})$ に正規化した値を返し、非連結なら `nullopt` を返す。

## その他

```cpp
int size() const
int groups() const
```

頂点数と連結成分数を返す。

## 時間計算量

- コンストラクタ: $O(n)$。固定長版では本体の配列の初期化に $O(\texttt{MAX_SIZE})$ かかる
- `merge`、`difference`: 償却 $O(\alpha(n))$
- `size`、`groups`: $O(1)$

## 空間計算量

- 固定長版: $O(\texttt{MAX_SIZE})$、可変長版: $O(n)$
- `merge`、`difference` の追加領域: 代表探索の再帰スタックとして最悪 $O(\log n)$

## 注意点

- $0 \lt \mathrm{MOD}$、$0 \le n \le \texttt{MAX_SIZE}$ で、頂点は $[0, n)$ でなければならない。
- 剰余の加減算は `long long` の中間値で行うため、`MOD` が `int` の上限付近でもオーバーフローしない。
- 範囲違反は `std::runtime_error` を送出する。`difference` は非連結を例外ではなく `nullopt` で表す。
