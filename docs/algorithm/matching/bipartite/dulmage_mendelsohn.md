---
title: Dulmage–Mendelsohn Decomposition (Dulmage–Mendelsohn分解)
documentation_of: ../../../../src/algorithm/matching/bipartite/dulmage_mendelsohn.hpp
---

二部グラフを、最大マッチングに関して左側不足部分・均衡部分・右側不足部分へ標準的に分解する。均衡部分はさらに既約な強連結成分へ分解する。

## API

```cpp
DulmageMendelsohnDecomposition decomposition(left_size, right_size);
```

左右の頂点数を指定して空の二部グラフを構築する。

```cpp
void decomposition.add_edge(int left, int right);
```

辺を追加する。多重辺を許す。

```cpp
DulmageMendelsohnResult result = decomposition.solve();
```

最大マッチングと分解を返す。`left_part` と `right_part` の要素は `left_deficient`、`balanced`、`right_deficient` のいずれかである。均衡頂点について `left_component`、`right_component` は強連結成分番号を持ち、それ以外は `-1` である。

## 時間計算量

左右の頂点数の和を $V$、辺数を $E$ とする。

- `add_edge`: 償却 $O(1)$
- `solve`: Hopcroft--Karp法を含め $O(E\sqrt{V})$

## 空間計算量

$O(V+E)$。

## 注意点

頂点番号が範囲外なら `runtime_error` を送出する。成分番号自体は辺の追加順に依存するが、分割は最大マッチングの選択によらず一意である。
