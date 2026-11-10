---
title: Character Labeled Undirected Maximum Flow (文字ラベル付き無向最大流) [MTOTALF]
documentation_of: ../../../../src/algorithm/graph/flow_cut/character_labeled_undirected_max_flow.hpp
---

1 バイトの文字で頂点を識別する、容量付き無向グラフの最大流量を求める。

## API

### `CharacterLabeledUndirectedFlowEdge<T>`

```cpp
template <class T>
struct CharacterLabeledUndirectedFlowEdge {
    char from;
    char to;
    T capacity;
};
```

文字ラベル `from`, `to` を結ぶ容量 `capacity` の無向辺を表す。

### `character_labeled_undirected_max_flow`

```cpp
template <class T>
T character_labeled_undirected_max_flow(
    const std::vector<CharacterLabeledUndirectedFlowEdge<T>>& edges,
    char source_label,
    char sink_label
);
```

`source_label` から `sink_label` への最大流量を返す。入力の辺と両終端に現れる文字だけを内部の頂点番号に圧縮し、`undirected_max_flow` で解く。すべての `char` 値をラベルとして使え、多重辺と自己ループも入力できる。

## 計算量

相異なるラベルの個数を $V \le 256$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `CharacterLabeledUndirectedFlowEdge` の構築 | $O(1)$ | $O(1)$ |
| `character_labeled_undirected_max_flow` | $O(V^2 M)$ | $O(V + M)$ |

ラベルは 1 バイトに限られるので、$V$ を定数とみなせば時間計算量は $O(M)$ である。

## 注意点

- `source_label` と `sink_label` は異なっていなければならない。同じ場合は `invalid_argument` を送出する。
- 容量は非負でなければならない。負の容量があると、下位の最大流の実装から `runtime_error` を送出する。
- `T` は算術型でなければならない。最大流量と途中の残余容量は `T` で表現できなければならない。
