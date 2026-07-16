---
title: Character Labeled Undirected Maximum Flow (文字ラベル付き無向最大流) [MTOTALF]
documentation_of: ../src/algorithm/graph/character_labeled_undirected_max_flow.hpp
---

1 byteの文字で頂点を識別する無向容量グラフの最大流を求めます。

## API

### `CharacterLabeledUndirectedFlowEdge<T>`

```cpp
template<class T>
struct CharacterLabeledUndirectedFlowEdge {
    char from;
    char to;
    T capacity;
};
```

文字ラベル `from`, `to` を結ぶ容量 `capacity` の無向辺を表します。

### `character_labeled_undirected_max_flow(edges, source_label, sink_label)`

```cpp
template<class T>
T character_labeled_undirected_max_flow(
    const vector<CharacterLabeledUndirectedFlowEdge<T>>& edges,
    char source_label,
    char sink_label
);
```

`source_label` から `sink_label` への最大流量を返します。
入力辺と両終端に現れる文字だけを内部頂点番号へ圧縮します。
すべての `char` 値をラベルとして使用でき、多重辺と自己loopも入力できます。

## API別の時間計算量・空間計算量

異なるラベル数を $V\leq256$、辺数を $M$ とします。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `CharacterLabeledUndirectedFlowEdge` の構築 | $O(1)$ | $O(1)$ |
| `character_labeled_undirected_max_flow` | $O(V^2M)$ | $O(V+M)$ |

文字ラベルが1 byteに限定されるため、$V$ を定数とみなせば時間計算量は
$O(M)$ です。

## 注意点

- `source_label` と `sink_label` は異なる必要があります。
- 容量は非負で、最大流量と中間残余容量を `T` で表現できる必要があります。
- `T` は算術型でなければなりません。
- 同一終端には `invalid_argument`、不正容量には下位の最大流実装から
  `runtime_error` を送出します。
