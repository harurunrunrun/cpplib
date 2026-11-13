---
title: Aho Corasick Representation (Aho-Corasick表現) [aho_corasick]
documentation_of: ../../../../src/algorithm/string/automata/aho_corasick_representation.hpp
---

複数のパターンから Aho--Corasick オートマトンを構築し、トライの親、失敗リンク、
各パターンの終端状態をまとめて返す。

## AhoCorasickRepresentation

```cpp
struct AhoCorasickRepresentation {
    vector<int> parent;
    vector<int> failure_link;
    vector<int> terminal_state;
    int node_count() const;
};
```

- `parent[v]`: トライ上の親状態。根 0 の親は 0。
- `failure_link[v]`: 状態 `v` の失敗リンク。根では 0。
- `terminal_state[i]`: `patterns[i]` を読み終えた状態。
- `node_count()`: 状態数を返す。

## aho_corasick_representation

```cpp
template<int ALPHABET, int MAX_NODES = -1, char OFFSET = 'a'>
AhoCorasickRepresentation aho_corasick_representation(
    const vector<string>& patterns
)
```

文字範囲 `[OFFSET, OFFSET + ALPHABET)` のパターン列から表現を構築する。
同じパターンを複数回指定した場合、対応する終端状態は同じになる。
`MAX_NODES` を省略（`-1`）すると可変長版の `AhoCorasick` を使う。

```cpp
auto result = aho_corasick_representation<26>(patterns);
auto fixed = aho_corasick_representation<26, 100000>(patterns);
```

## 時間計算量

全パターンの長さの合計を $P$、使用状態数を $V$、$\Sigma$ を `ALPHABET` とする。

- `node_count()`: $O(1)$
- `aho_corasick_representation`: 固定長版 $O(\texttt{MAX_NODES}\cdot\Sigma+P+V\Sigma)$、可変長版 $O(P+V\Sigma)$

新規状態の遷移表の初期化を含む。既存の経路だけを通る文字は 1 文字 $O(1)$ である。

## 空間計算量

構築中のオートマトンが固定長版で $O(\texttt{MAX_NODES}\cdot\Sigma)$、可変長版で $O(V\Sigma)$、戻り値が $O(V+K)$ である。
ここで $K$ はパターン数である。オートマトン本体はヒープ上に確保する。

## 注意点

- `ALPHABET > 0` が必要で、`MAX_NODES` は `-1` または正の値である。
- 範囲外の文字を含む場合、または状態数が `MAX_NODES` を超える場合は `std::runtime_error` を送出する。
- 状態番号はパターンを入力順にトライへ追加したときの生成順であり、根は 0 である。
