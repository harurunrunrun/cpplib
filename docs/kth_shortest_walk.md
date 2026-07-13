---
title: K-th Shortest Walk
documentation_of: ../src/algorithm/graph/kth_shortest_walk.hpp
---

非負重み有向グラフで、頂点と辺の再訪を許す walk の距離を小さい順に求める。

# 関数

```cpp
kth_shortest_walks<T>(graph, source, target, k)
```

`graph[v]` は `KthShortestWalkEdge<T>{to, cost}` の列。

戻り値は `source` から `target` への walk の距離列。`source == target` のときは空 walk の距離 `0` も含む。

`k` は求める個数。`k == 0` なら空列を返す。

# 指定順位の値

```cpp
optional<T> kth_shortest_walk<T>(graph, source, target, k)
optional<T> kth_shortest_walk_1indexed<T>(graph, source, target, k)
```

`kth_shortest_walk` の `k` は0-indexedで、`k == 0` が最短walkを表す。

`kth_shortest_walk_1indexed` の `k` は1-indexedで、`k == 1` が最短walkを表す。`k <= 0` は範囲外。

指定順位までwalkが存在しない場合は `nullopt` を返す。全距離列を保持せず、指定順位が確定した時点で終了する。

# walkの数え方

通る辺の列が異なるwalkは別に数える。多重辺を選び分けた場合も別walkであり、距離が同じでもそれぞれ順位を持つ。

重み0の閉路も許す。このとき同じ距離のwalkが複数回、または任意個数現れることがある。`source == target` では空walkが最初の候補になる。

# 制約

walk の距離と辺コストの加算結果は `T` で表現できる必要がある。負辺を含む場合は例外を送出する。

## 時間計算量

逆辺上のDijkstraで各頂点から `target` までの最短距離を求め、その値をheuristicにしたbest-first searchを行う。各頂点は高々 $K$ 回展開する。

- `kth_shortest_walks(..., k)`: $K=k$ として $O((N+M)\log(N+1)+KM\log(KM+1))$。最大 $K$ 個を出力
- `kth_shortest_walk(..., k)`: $K=k+1$ として同じ時間。指定順位確定時に終了し、列全体は保持しない
- `kth_shortest_walk_1indexed(..., k)`: $K=k$ として同じ時間。指定順位確定時に終了し、列全体は保持しない

`k == 0` の列挙でも入力検査に $O(N+M)$ を使う。

## 空間計算量

- 共通探索領域: $O(N+M+KM)$
- `kth_shortest_walks` のみ出力列 $O(K)$ を追加

## API契約・前提・例外

列挙版と0-indexed版は `k >= 0`、1-indexed版は `k > 0` とする。順位、端点、辺の行き先、非負costの違反時は `runtime_error`。walk不足は短い列または `nullopt` で表す。

頂点引数と隣接リストの行き先は、各APIで定めた頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査とResultの構築を含む。
