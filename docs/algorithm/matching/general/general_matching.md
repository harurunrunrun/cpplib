---
title: General Matching (一般グラフのマッチング) [general_matching]
documentation_of: ../../../../src/algorithm/matching/general/general_matching.hpp
---

一般グラフの最大マッチングを Edmonds の blossom 法で求める。

## コンストラクタ

```cpp
GeneralMatching graph(n)
```

$n$ 頂点の空のグラフを作る。

## メンバ関数

```cpp
graph.add_edge(u, v)
graph.solve()
```

`add_edge` は無向辺 `{u, v}` を追加する。`solve` は最大マッチングを求め、`GeneralMatchingResult` を返す。

`GeneralMatchingResult` は次のメンバを持つ。

```cpp
int size;
vector<int> match;
```

`match[v]` は `v` と対応する頂点、未マッチなら `-1` である。

## 時間計算量

$N$ を頂点数、$M$ を追加済みの無向辺数とする。

- `GeneralMatching(n)`: $O(N)$
- `add_edge`: 償却 $O(1)$
- `lca`, `mark_path`: $O(N)$
- `find_path`: 最悪 $O(M + N^2)$
- `solve`: 最悪 $O(NM + N^3)$。長さ $N$ の `match` の出力を含む

単純グラフでは $M = O(N^2)$ なので、通常の $O(N^3)$ の上界になる。多重辺を重複して追加した場合は、その走査時間 $O(NM)$ を含む。

## 空間計算量

- グラフ、blossom の作業領域、戻り値を含めて $O(N + M)$

## 注意点

公開メンバ関数 `lca`, `mark_path`, `find_path` は blossom 探索用の補助関数で、`find_path` が作る内部状態を前提とする。通常は `solve` を使う。自己ループは無視し、多重辺を許す。`solve` は内部のマッチングを保持したまま増加させるため、辺を追加した後に再度呼ぶと既存のマッチングから続けて計算する。

頂点数は非負、各端点は頂点範囲内でなければならず、違反時は `runtime_error` を送出する。記載した計算量には引数検査と戻り値の構築を含む。
