---
title: Modular Potential DSU (剰余ポテンシャル付き素集合データ構造) [unionfind_with_potential]
documentation_of: ../src/structure/dsu/modular_potential_dsu.hpp
---

ポテンシャル差を法 `MOD` で管理する重み付きDSU。剰余型と頂点順序の変換を構造体内に持つ。

# 構築

```cpp
ModularPotentialDSU<MOD, MAX_SIZE> dsu(n)
```

`n` 個の頂点を作る。

- 時間計算量: $O(MAX\_SIZE)$
- 保存領域: $O(MAX\_SIZE)$

# merge

```cpp
bool merge(int u, int v, long long difference)
```

`potential(u) - potential(v) = difference (mod MOD)` を追加する。既存制約と整合する場合は `true`、矛盾する場合は `false`。

- 時間計算量: 償却 $O(\alpha(N))$
- 追加空間計算量: 代表探索のstackを含め最悪 $O(\log N)$

# difference

```cpp
optional<int> difference(int u, int v)
```

連結なら `potential(u) - potential(v)` の `[0, MOD)` に正規化した値を返し、非連結なら `nullopt`。

- 時間計算量: 償却 $O(\alpha(N))$
- 追加空間計算量: 最悪 $O(\log N)$

# その他

```cpp
int size() const
int groups() const
```

いずれも $O(1)$。

## 注意点

- `0 < MOD <= INT_MAX`、`0 <= n <= MAX_SIZE`、頂点は `[0,n)`。
- 剰余加減算は `long long` の中間値を使うため、`MOD` が32-bit signed整数の上限付近でもoverflowしない。
- 範囲違反は `runtime_error`。`difference` は非連結を例外ではなく `nullopt` で表す。
