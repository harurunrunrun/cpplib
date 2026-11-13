---
title: Tree Path XOR Basis (木パス XOR 基底)
documentation_of: ../../../../src/algorithm/tree/query/tree_path_xor_basis.hpp
---

木の頂点の値について、任意の単純パス上の値が張る XOR の線形基底をダブリングで求める。

## API

```cpp
template<int Bits = 64>
class TreePathXorBasis;
using Basis = std::array<std::uint64_t, Bits>;
```

- `TreePathXorBasis<Bits>(graph, values, root = 0)`: 隣接リスト `graph` と頂点の値 `values`（`std::vector<std::uint64_t>`）から前処理する。デフォルトコンストラクタは空の構造を作る。
- `path_basis(u, v)`: `u` と `v` を結ぶパス上（両端を含む）の値が張る基底を `Basis` で返す。`basis[bit]` は最上位ビットが `bit` の基底ベクトル（なければ `0`）である。
- `maximum_xor(u, v, seed = 0)`: `seed` とパス上の値の XOR で作れる最大値を返す。

## 時間計算量

基底どうしのマージが $O(\mathrm{Bits}^2)$ かかるため、次のとおりである。

- 構築: $O(N \cdot \mathrm{Bits}^2 \log N)$ 時間、$O(N \cdot \mathrm{Bits} \log N)$ 空間
- `path_basis`, `maximum_xor`: $O(\mathrm{Bits}^2 \log N)$

## 注意点

- `1 <= Bits <= 64` でなければならない（`static_assert`）。各値は $2^{\mathrm{Bits}}$ 未満でなければならず、そうでない値があるとコンストラクタが `std::invalid_argument` を送出する。
- 入力は連結な無向木とし、`values` の長さは頂点数と一致させる。
- 長さの不一致・非連結では `std::invalid_argument`、`root` や頂点が範囲外では `std::out_of_range` を送出する。
