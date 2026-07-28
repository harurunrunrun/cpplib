---
title: Tree Path XOR Basis (木パスXOR基底)
documentation_of: ../../../../src/algorithm/tree/query/tree_path_xor_basis.hpp
---

木の頂点値について、任意の単純パス上の線形XOR基底を二分累乗で取得する。

## API

- `TreePathXorBasis<Bits>(graph, values, root)`: 前処理する。
- `path_basis(u, v)`: パス上の値が張る基底を返す。
- `maximum_xor(u, v, seed)`: `seed` とパス上の値から作れる最大XORを返す。

## 時間計算量

- 構築: $O(N\,Bits\log N)$。
- `path_basis`, `maximum_xor`: $O(Bits\log N)$。

## 注意点

- `1 <= Bits <= 64`。入力は連結な無向木で、値数は頂点数と一致させる。
