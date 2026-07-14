---
title: Enterprise Network (企業ネットワーク距離) [CORNET]
documentation_of: ../src/structure/dsu/enterprise_network.hpp
---

各頂点が高々1つの親を持つnetworkで、中心までの重み付き距離を管理する。

## API

### `EnterpriseNetwork<MAX_SIZE>(n)`

全頂点が自身を中心とするnetworkを作る。時間計算量は $O(n)$。

### `connect(enterprise, destination)`

親を `destination` にし、辺重みを `abs(enterprise-destination)%1000` にする。時間計算量は $O(1)$。

### `distance_to_center(enterprise)`

親を辿った中心までの距離を返し、経路圧縮する。経路長を $h$ として時間計算量は $O(h)$、一連の取得では償却 $O(\alpha(n))$。

## 注意点

- `0 <= n <= MAX_SIZE` と範囲内頂点が必要である。
- `connect` の `enterprise` は現在の連結成分の中心で、`destination` は別成分に属する必要がある。
- この前提により親pointerはcycleを作らず、経路圧縮後も距離が保たれる。
