---
title: Metaheuristic Search Result (メタヒューリスティック探索結果)
documentation_of: ../src/approximate/metaheuristic/search_result.hpp
---

`approximate::metaheuristic`の探索アルゴリズムが共通で返す結果型である。

## `SearchResult`

```cpp
template<class State, class Score>
struct SearchResult {
    State state;
    Score score;
    std::size_t iterations;
    std::size_t evaluations;
    std::size_t accepted_moves;
};
```

`state`と`score`は探索中に得た最良解と目的値、`iterations`は実行した反復数、
`evaluations`は目的関数の呼び出し回数、`accepted_moves`は受理した遷移数である。

| API・操作 | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| 構築・コピー・move | `State`と`Score`の同操作に等しい | `State`と`Score`の大きさ |
| 各memberの参照 | $O(1)$ | $O(1)$ |

## 注意点

各メンバは公開されており直接書き換えられる。書き換え時の整合性検査は行われず、反復数・評価回数・最良値の意味はこの値を生成した探索アルゴリズムに従う。
