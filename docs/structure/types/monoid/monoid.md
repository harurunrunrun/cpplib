---
title: Monoid (モノイド)
documentation_of: ../../../../src/structure/types/monoid/monoid.hpp
---

`op_func` と `e_func` からモノイドを作る軽量な型である。`op_func` と `e_func` は非型テンプレート引数（関数ポインタなど）として渡す。

```cpp
constexpr long long add(long long a, long long b){ return a + b; }
constexpr long long zero(){ return 0; }

constexpr Monoid<add, zero> monoid{};
```

`S` は `e_func()` の戻り値から決まる。

## 要件

- `op_func(a, b)` は `S` 上で結合的である。
- `e_func()` は左右の単位元を返す。

すなわち、すべての有効な `a, b, c` について次を満たす必要がある。

```cpp
op_func(op_func(a, b), c) == op_func(a, op_func(b, c))
op_func(e_func(), a) == a
op_func(a, e_func()) == a
```

## メンバ

| メンバ | 内容 | 時間計算量 |
| --- | --- | --- |
| `using S` | 値の型 | - |
| `op(a, b)` | `op_func(a, b)` を呼ぶ | `op_func` の計算量 |
| `e()` | `e_func()` を呼ぶ | `e_func` の計算量 |

`Monoid` オブジェクトは状態を持たないため、デフォルト構築の時間計算量は $O(1)$ である。
`op` と `e` は渡した関数を 1 回呼び出すだけであり、それ以外の漸近的な追加コストを持たない。

## 使用例

```cpp
constexpr Monoid<add, zero> add_monoid{};
Segtree<add_monoid, 200000> seg(n);
```

## 注意点

- `op` は結合的、`e` は左右の単位元でなければならない。ライブラリはこれらの代数則を実行時に検査しない。
- `Monoid` の構築、`op`、`e`、型エイリアスの取得の追加空間計算量は $O(1)$ である。
- 時間計算量は指定した `op_func` / `e_func` の呼び出し時間であり、通常の定数時間演算なら $O(1)$ である。
- `op_func` と `e_func` は非型テンプレート引数として使用できる必要がある。
