---
title: Named Shortest Paths (名前付き頂点間最短路) [SHPATH]
documentation_of: ../../../../../src/algorithm/graph/shortest_path/specialized/named_shortest_paths.hpp
---

文字列の名前で頂点を識別する静的な非負重みの有向グラフについて、頂点間の最短距離を問い合わせる。

## `NamedShortestPaths`

```cpp
template <class T>
class NamedShortestPaths {
public:
    NamedShortestPaths(
        std::vector<std::string> names,
        std::vector<std::vector<DijkstraEdge<T>>> graph
    );

    int size() const;
    bool contains(const std::string& name) const;
    std::optional<T> distance(
        const std::string& source_name,
        const std::string& target_name
    ) const;
    void clear_cache();
};
```

## コンストラクタ

`names[v]` を頂点 `v`（隣接リスト `graph[v]`）の名前として構築する。名前は相異なっていなければならない。

## `size`

頂点数を返す。

## `contains`

指定した名前の頂点が存在するなら真を返す。

## `distance`

指定した 2 頂点間の最短距離を返す。到達不能なら `std::nullopt` を返す。始点ごとに Dijkstra 法の結果をキャッシュし、`clear_cache` が呼ばれるまで保持する。

## `clear_cache`

保持している最短距離の計算結果をすべて破棄する。頂点名とグラフは変更しない。

## 計算量

$N$ を頂点数、$M$ を辺数、$C$ を計算結果を保持している相異なる始点の個数とする。文字列のハッシュ計算と比較にかかる時間は別途加わる。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| コンストラクタ | $O(N + M)$ | $O(N + M)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `contains(name)` | 平均 $O(1)$ | $O(1)$ |
| `distance(source, target)`（未計算の始点） | $O((N + M)\log(N + M + 1))$ | $O(N + M)$ |
| `distance(source, target)`（計算済みの始点） | 平均 $O(1)$ | $O(1)$ |
| `clear_cache()` | $O(CN + N)$ | $O(1)$ |

オブジェクト全体では、入力グラフと保持した計算結果を合わせて $O(N + M + CN)$ 空間を使う。

## 注意点

- `names.size()` と `graph.size()` は等しく、名前は相異なっていなければならない。
- 各辺の行き先は $[0, N)$ の範囲内、重みは非負でなければならない。
- 未登録の名前を `distance` に渡した場合を含め、前提に違反すると `runtime_error` を送出する。
- 距離の加算結果は `T` で表現できなければならない。
- `distance` は `const` メンバ関数だが内部のキャッシュを更新する（`mutable`）ため、複数スレッドから同時に呼んではならない。
