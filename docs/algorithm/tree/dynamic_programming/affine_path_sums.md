---
title: Affine Tree Path Sums (辺アフィン写像の全頂点経路和) [tree_path_composite_sum]
documentation_of: ../../../../src/algorithm/tree/dynamic_programming/affine_path_sums.hpp
---

各辺にアフィン写像が載った木で、各頂点を根としたときに、全頂点の値を根まで運んだ総和を求める。

## API

```cpp
template<class T>
struct AffineTreeEdge{ int u, v; T multiplier, addend; };

template<class T>
std::vector<T> affine_path_sums(const std::vector<T>& values,
                                const std::vector<AffineTreeEdge<T>>& edges,
                                const std::vector<T>& vertex_mass = {});
```

- `AffineTreeEdge<T>{u, v, multiplier, addend}` は、両方向に同じ写像 $x \mapsto \mathrm{multiplier} \cdot x + \mathrm{addend}$ を適用する辺である。
- `affine_path_sums(values, edges)` は、各根 $r$ について、全頂点 $y$ の値 `values[y]` を $y$ から $r$ へ向かう順に辺の写像を適用して運んだものの総和を返す。乗数が零の辺も許す。
- 第 3 引数 `vertex_mass` を与えると、各始点 $y$ の寄与を `vertex_mass[y]` 倍する（アフィン写像の定数項も含めて `vertex_mass[y]` 倍される）。省略した場合はすべて $1$ である。値の型が表現できれば、零や負の重みも使える。

再帰を使わない全方位木 DP で計算する。

## 時間計算量

頂点数 $N$ に対して $O(N)$ 時間、$O(N)$ 空間である。

## 注意点

- `edges` は頂点集合 $[0, N)$ 上の木でなければならない。空木には空の配列を返す。
- 辺数・連結性・閉路の条件に違反した場合や、`vertex_mass` が空でなく長さが `values` と異なる場合は `std::invalid_argument` を、端点が範囲外の場合は `std::out_of_range` を送出する。
- `T` は整数からの構築と加減乗算を提供し、すべての中間値を表現できる必要がある。
