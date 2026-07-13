---
title: K-th Shortest Walk (K番目最短歩)
documentation_of: ../src/algorithm/graph/kth_shortest_walk.hpp
---

非負重み有向グラフで、頂点と辺の再訪を許す walk の距離を小さい順に求める。

# 辺

~~~cpp
KthShortestWalkEdge<T>{to, cost}
~~~

`to` は行き先、`cost` は非負の辺重み。

# 列挙

~~~cpp
vector<T> kth_shortest_walks<T>(graph, source, target, k)
~~~

`graph[v]` は `v` から出る `KthShortestWalkEdge<T>` の列。

戻り値は `source` から `target` への walk の距離を小さい順に最大 `k` 個並べた列。walk が `k` 個未満しか存在しない場合は存在する分だけ返す。`source == target` のときは空 walk の距離 `0` も含む。`k == 0` なら空列を返す。

# 指定順位の値

~~~cpp
optional<T> kth_shortest_walk<T>(graph, source, target, k)
optional<T> kth_shortest_walk_1indexed<T>(graph, source, target, k)
~~~

`kth_shortest_walk` の `k` は0-indexedで、`k == 0` が最短walkを表す。

`kth_shortest_walk_1indexed` の `k` は1-indexedで、`k == 1` が最短walkを表す。`k <= 0` は範囲外。

指定順位までwalkが存在しない場合は `nullopt` を返す。全距離列を保持せず、指定順位が確定した時点で終了する。

# walkの数え方

通る辺の列が異なるwalkは別に数える。多重辺を選び分けた場合も別walkであり、距離が同じでもそれぞれ順位を持つ。

重み0の閉路も許す。このとき同じ距離のwalkが複数回、または任意個数現れることがある。`source == target` では空walkが最初の候補になる。

# アルゴリズム

逆グラフ上のDijkstraで各頂点から `target` までの最短距離と最短路木を求める。木に選ばれなかった各辺をsidetrackとし、

\[
\mathrm{cost}(u,v)+\mathrm{dist}[v]-\mathrm{dist}[u]
\]

を追加コストとして永続leftist heapへ格納する。Eppsteinのsidetrack列を優先度付きキューで展開するため、順位ごとに全辺や全頂点を再展開しない。重み0の閉路、多重辺、最短距離が等しい辺も、それぞれ独立したsidetrackとして列挙する。

## API別の時間計算量・空間計算量

$N$ を頂点数、$M$ を辺数、$K$ を実際に要求する順位数とする。

- `kth_shortest_walks(..., k)`: $K=k$ として、時間 $O((N+M)\log(N+M+1)+K\log(K+1))$
- `kth_shortest_walk(..., k)`: $K=k+1$ として、時間 $O((N+M)\log(N+M+1)+K\log(K+1))$
- `kth_shortest_walk_1indexed(..., k)`: $K=k$ として、時間 $O((N+M)\log(N+M+1)+K\log(K+1))$

`kth_shortest_walks(..., 0)` は入力検査だけを行い、時間 $O(N+M)$。

$S$ を `target` へ到達可能な部分にあるsidetrack数とすると、逆辺、距離、最短路木、永続heap、列挙用queueを合わせた作業領域は

\[
O(N+M+S\log(S+1)+K)
\]

である。従来の頂点ごとのrank回展開に必要だった $O(KM)$ の候補領域は使用しない。`kth_shortest_walks` はこれに戻り値の $O(K)$ を含み、指定順位版は距離列を保存しない。

## 注意点

列挙版と0-indexed版は `k >= 0`、1-indexed版は `k > 0` とする。順位、端点、辺の行き先、非負costの違反時は `runtime_error`。walk不足は短い列または `nullopt` で表す。

`T` は `T()` が重み0を表し、コピー、比較、加算、減算を行える型とする。walk距離、Dijkstraの加算、sidetrack追加コスト、および列挙中の加算結果は `T` で表現できる必要がある。

頂点引数と隣接リストの行き先は頂点範囲内でなければならない。違反時は `runtime_error` を送出する。記載した計算量には引数検査と戻り値の構築を含む。
