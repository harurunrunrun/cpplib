---
title: Dulmage–Mendelsohn Decomposition (Dulmage–Mendelsohn分解)
documentation_of: ../../../../src/algorithm/matching/bipartite/dulmage_mendelsohn.hpp
---

二部グラフを、最大マッチングに関して左側不足部分・均衡部分・右側不足部分に標準的に分解する。均衡部分はさらに既約な強連結成分に分解する。

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

最大マッチングと分解を返す。結果は次のメンバを持つ。

- `matching_size`: 最大マッチングの大きさ
- `left_match`, `right_match`: 最大マッチングの対応。未マッチなら `-1`
- `left_part`, `right_part`: 各頂点の属する部分。`DulmageMendelsohnPart::left_deficient`、`balanced`、`right_deficient` のいずれか
- `balanced_component_count`: 均衡部分の強連結成分の個数
- `left_component`, `right_component`: 均衡部分の頂点については強連結成分の番号、それ以外は `-1`

左側不足部分は未マッチの左頂点から交互路で到達できる頂点、右側不足部分は未マッチの右頂点へ交互路で到達できる頂点であり、残りが均衡部分である。

## 時間計算量

左右の頂点数の和を $V$、辺数を $E$ とする。

- `add_edge`: 償却 $O(1)$
- `solve`: Hopcroft–Karp 法を含めて $O(E \sqrt{V} + V)$

## 空間計算量

$O(V + E)$。

## 注意点

頂点数が負の場合や頂点番号が範囲外の場合は `runtime_error` を送出する。成分番号自体は辺の追加順に依存するが、分割は最大マッチングの選び方によらず一意である。
