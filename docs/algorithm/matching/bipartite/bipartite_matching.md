---
title: Bipartite Matching (二部マッチング) [bipartitematching] [GRL_7_A]
documentation_of: ../../../../src/algorithm/matching/bipartite/bipartite_matching.hpp
---

Hopcroft–Karp 法で二部グラフの最大マッチングを求める。

## コンストラクタ

```cpp
BipartiteMatching graph(left_size, right_size)
```

左側 `left_size` 頂点、右側 `right_size` 頂点の空の二部グラフを作る。

## メンバ関数

```cpp
graph.add_edge(left, right)
graph.solve()
```

`add_edge` は左頂点 `left` と右頂点 `right` を結ぶ辺を追加する。`solve` は最大マッチングを求め、`BipartiteMatchingResult` を返す。

`BipartiteMatchingResult` は次のメンバを持つ。

```cpp
int size;
vector<int> left_match;
vector<int> right_match;
```

`left_match[l]`, `right_match[r]` は対応する相手側の頂点番号、未マッチなら `-1` である。

## 時間計算量

$V = L + R$、$E$ を追加済みの辺数とする。

- `BipartiteMatching(L, R)`: $O(V)$
- `add_edge`: 償却 $O(1)$
- `bfs`: $O(V + E)$
- `dfs(v)`: 1 回あたり最悪 $O(V + E)$
- `solve`: $O(E \sqrt{V} + V)$。長さ $L, R$ の対応列の出力を含む

## 空間計算量

- グラフ、マッチング、戻り値を含めて $O(V + E)$

## 注意点

公開メンバ `bfs()` は層を構築して増加路が存在するかを返し、`dfs(left)` はその層を使って 1 本の増加路で増加させる内部用の補助関数である。通常は全辺を追加した後に `solve()` を 1 回呼ぶ。`solve()` は内部のマッチングを保持したまま増加させるため、辺を追加した後に再度呼ぶと既存のマッチングから続けて計算する。`dfs` は再帰で実装されており、再帰の深さは最大 $O(V)$ になる。

頂点数は非負、各端点は対応する頂点範囲内でなければならず、違反時は `runtime_error` を送出する。記載した計算量には引数検査と戻り値の構築を含む。
