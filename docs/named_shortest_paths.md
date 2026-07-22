---
title: Named Shortest Paths (名前付き頂点間最短路) [SHPATH]
documentation_of: ../src/algorithm/graph/shortest_path/named_shortest_paths.hpp
---

文字列名で識別される静的な非負重み有向グラフに対し、頂点間の最短距離を問い合わせる。

## NamedShortestPaths

```cpp
template<class T>
class NamedShortestPaths {
public:
    NamedShortestPaths(
        vector<string> names,
        vector<vector<DijkstraEdge<T>>> graph
    );

    int size() const;
    bool contains(const string& name) const;
    optional<T> distance(
        const string& source_name,
        const string& target_name
    ) const;
    void clear_cache();
};
```

## Constructor

`names[v]` を `graph[v]` の頂点名として構築する。名前は相異なる必要がある。

## size

頂点数を返す。

## contains

指定した名前の頂点が存在するなら真を返す。

## distance

指定した2頂点間の最短距離を返す。到達不能なら `nullopt` を返す。
同じ始点に対する計算結果は、`clear_cache` が呼ばれるまで保持される。

## clear_cache

保持している最短距離の計算結果をすべて破棄する。頂点名とグラフは変更しない。

## API別の時間計算量・空間計算量

$N$ を頂点数、$M$ を辺数、$C$ を計算結果が保持されている異なる始点数とする。文字列のhashと比較に要する時間は別途加わる。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| Constructor | $O(N+M)$ | $O(N+M)$ |
| `size()` | $O(1)$ | $O(1)$ |
| `contains(name)` | 平均 $O(1)$ | $O(1)$ |
| `distance(source, target)`（未計算の始点） | $O((N+M)\log(N+1))$ | $O(N)$ |
| `distance(source, target)`（計算済みの始点） | 平均 $O(1)$ | $O(1)$ |
| `clear_cache()` | $O(CN+N)$ | $O(1)$ |

構造体全体では、入力グラフと保持した計算結果を合わせて $O(N+M+CN)$ 空間を使う。

## 注意点

- `names.size()` と `graph.size()` は等しく、名前は相異ならなければならない。
- 各辺の行き先は $[0,N)$、重みは非負でなければならない。
- 未登録の名前を `distance` に渡した場合を含め、前提違反時は `runtime_error` を送出する。
- 距離の加算結果は `T` で表現できる必要がある。
