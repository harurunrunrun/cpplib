---
title: Monoid (モノイド)
documentation_of: ../src/structure/types/monoid.hpp
---


`op_func` と `e_func` から monoid を作る軽量な型。

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

| メンバ | 内容 | 計算量 |
| --- | --- | --- |
| `using S` | 値の型 | - |
| `op(a, b)` | `op_func(a, b)` を呼ぶ | `O(op_func の計算量)` |
| `e()` | `e_func()` を呼ぶ | `O(e_func の計算量)` |

`Monoid` オブジェクトのデフォルト生成は状態を持たないため `O(1)`。
`op` と `e` は渡した関数を1回呼び出すだけであり、それ以外の漸近的な追加コストを持たない。

## 使用例

```cpp
constexpr Monoid<add, zero> add_monoid{};
Segtree<add_monoid, 200000> seg(n);
```
