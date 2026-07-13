---
title: Monoid (作用素付き・長さ付き)
documentation_of: ../src/structure/types/monoid_act_len.hpp
---

区間長に依存する `op` と `mapping` を持つ作用付き monoid。
`DynamicLazySegtree` などに使う。

```cpp
constexpr long long sum(
    long long a, long long,
    long long b, long long
){
    return a + b;
}
constexpr long long zero(){ return 0; }
constexpr long long add_mapping(
    long long f, long long x, long long length
){
    return x + f * length;
}
constexpr long long add_composition(long long f, long long g){
    return f + g;
}
constexpr long long add_id(){ return 0; }

constexpr Monoid_Act_Len<
    sum,
    zero,
    add_mapping,
    add_composition,
    add_id
> action{};
```

`S` は区間値の型、`T` は作用の型である。長さは `long long` で渡される。

## 要件

`op(a, len_a, b, len_b)` は、長さも含めて単位元・結合法則を満たす必要がある。
また、0以上の長さについて次を満たす必要がある。

- `mapping(id(), x, len) == x`
- `mapping(f, op(x, lx, y, ly), lx+ly) == op(mapping(f,x,lx), lx, mapping(f,y,ly), ly)`
- `mapping(composition(f,g), x, len) == mapping(f, mapping(g,x,len), len)`
- `composition(id(), f) == composition(f, id()) == f`
- `composition` は結合的

`composition(f, g)` は「先に `g`、後に `f`」を表す。

## メンバ

| メンバ | 内容 | 時間計算量 |
| --- | --- | --- |
| `using S` | 区間値の型 | - |
| `using T` | 作用の型 | - |
| `op(a, la, b, lb)` | `op_func(a, la, b, lb)` を呼ぶ | `O(op_func の計算量)` |
| `e()` | `e_func()` を呼ぶ | `O(e_func の計算量)` |
| `mapping(f, x, len)` | `mapping_func(f, x, len)` を呼ぶ | `O(mapping_func の計算量)` |
| `composition(f, g)` | `composition_func(f, g)` を呼ぶ | `O(composition_func の計算量)` |
| `id()` | `id_func()` を呼ぶ | `O(id_func の計算量)` |

`Monoid_Act_Len` オブジェクトのデフォルト生成は `O(1)`。
各メンバは対応する関数を1回呼び出すだけで、それ以外の漸近的な追加コストを持たない。


## 注意点

`op(a,alen,b,blen)` は順序と区間長を保って結合的、`e` は長さ0の単位元でなければ
ならない。`composition(f,g)` は `g` の後に `f` を適用する作用を返し、`mapping` は
与えた区間長に対する分配則を満たす必要がある。代数則は実行時に検査しない。
各member呼出しの時間・追加空間計算量は指定した関数のコストであり、通常の定数時間演算なら
ともに $O(1)$。
