---
title: Aho Corasick Representation (Aho-Corasick表現) [aho_corasick]
documentation_of: ../src/algorithm/string/aho_corasick_representation.hpp
---

複数patternからAho-Corasick automatonを構築し、trieの親、failure link、
各patternの終端状態をまとめて返す。

## AhoCorasickRepresentation

```cpp
struct AhoCorasickRepresentation {
    vector<int> parent;
    vector<int> failure_link;
    vector<int> terminal_state;
    int node_count() const;
};
```

- `parent[v]`: trie上の親状態。root 0の親は0。
- `failure_link[v]`: 状態 `v` のfailure link。rootでは0。
- `terminal_state[i]`: `patterns[i]` を読み終えた状態。
- `node_count()`: 状態数を返す。

## aho_corasick_representation

```cpp
template<int ALPHABET, int MAX_NODES, char OFFSET = 'a'>
AhoCorasickRepresentation aho_corasick_representation(
    const vector<string>& patterns
)
```

文字範囲 `[OFFSET, OFFSET + ALPHABET)` のpattern列から表現を構築する。
同じpatternを複数回指定した場合、対応する終端状態は同じになる。

## 時間計算量

全pattern長を $P$、使用状態数を $V$、$\Sigma=ALPHABET$ とする。

- `node_count()`: $O(1)$
- `aho_corasick_representation`: $O(MAX\_NODES\cdot\Sigma+P\cdot\Sigma+V\cdot\Sigma)$

新規状態の固定長遷移表初期化を含む。既存経路だけを通る文字は1文字 $O(1)$。

## 空間計算量

構築中のautomatonが $O(MAX\_NODES\cdot\Sigma)$、戻り値が $O(V+K)$。
ここで $K$ はpattern数。automaton本体はheap上に確保する。

## 注意点

- `ALPHABET > 0`、`MAX_NODES > 0` が必要。
- 範囲外の文字、または状態数が `MAX_NODES` を超える場合は
  `runtime_error` を送出する。
- 状態番号はpatternを入力順にtrieへ追加したときの生成順で、rootは0。
