---
title: Monoid (作用素付き)
documentation_of: ../src/structure/types/monoid_act.hpp
---

`Monoid` に、区間値へ適用する作用を加えた型。`LazySegtree` などに使う。

```cpp
constexpr long long minimum(long long a, long long b){
    return std::min(a, b);
}
constexpr long long infinity(){ return (1LL << 60); }
constexpr long long add_mapping(long long f, long long x){
    return x == infinity() ? x : x + f;
}
constexpr long long add_composition(long long f, long long g){
    return f + g;
}
constexpr long long add_id(){ return 0; }

constexpr Monoid_Act<
    minimum,
    infinity,
    add_mapping,
    add_composition,
    add_id
> action{};
```

`S` は区間値の型、`T` は作用の型である。

## 要件

`op` と `e` は `Monoid` の法則を満たし、作用について次を満たす必要がある。

- `mapping(id(), x) == x`
- `mapping(f, op(x, y)) == op(mapping(f, x), mapping(f, y))`
- `mapping(composition(f, g), x) == mapping(f, mapping(g, x))`
- `composition(id(), f) == composition(f, id()) == f`
- `composition` は結合的

`composition(f, g)` は「先に `g`、後に `f`」を表す。

## メンバ

| メンバ | 内容 | 計算量 |
| --- | --- | --- |
| `using S` | 区間値の型 | - |
| `using T` | 作用の型 | - |
| `op(a, b)` | `op_func(a, b)` を呼ぶ | `O(op_func の計算量)` |
| `e()` | `e_func()` を呼ぶ | `O(e_func の計算量)` |
| `mapping(f, x)` | `mapping_func(f, x)` を呼ぶ | `O(mapping_func の計算量)` |
| `composition(f, g)` | `composition_func(f, g)` を呼ぶ | `O(composition_func の計算量)` |
| `id()` | `id_func()` を呼ぶ | `O(id_func の計算量)` |

`Monoid_Act` オブジェクトのデフォルト生成は `O(1)`。
各メンバは対応する関数を1回呼び出すだけで、それ以外の漸近的な追加コストを持たない。

## 使用例

```cpp
constexpr AddMinMonoidAct<long long> add_min{};
LazySegtree<add_min, 200000> seg(n);
```

