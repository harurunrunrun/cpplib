---
title: Real-Time Heuristic Search (実時間ヒューリスティック探索)
documentation_of: ../../../src/approximate/search/real_time_heuristic_search.hpp
---

未知または変化する非負の辺を持つグラフで、限定した先読みと学習済みヒューリスティックを使って 1 回の呼び出し内で実行と再計画を繰り返す実時間探索群である。状態型、ゴール判定、近傍生成、ヒューリスティックは汎用の関数オブジェクトで与える。

`neighbors(state)` は `pair<State, Cost>` の範囲、`goal(state)` は真偽値、`heuristic(state)` は `Cost` へ変換できる非負有限値を返す。`Hash` と `Equal` を省略すると `std::hash<State>` と `std::equal_to<State>` を用いる。

## `RealTimeHeuristicSearchResult`

```cpp
template<class State, class Cost>
struct RealTimeHeuristicSearchResult {
    vector<State> path;
    optional<Cost> cost;
    vector<pair<State, Cost>> learned_heuristic;
    size_t expanded;
    bool cutoff;
    explicit operator bool() const noexcept;
};
```

実際に移動した経路、ゴール到達時の実コスト、呼び出し中に参照・学習したヒューリスティック表、展開数、手数上限到達を返す。未到達時は `cost` が空。真偽変換は $O(1)$、結果の保持空間は移動数を $M$、学習状態数を $D$ として $O(M+D)$。

## `lrta_star_search`

```cpp
lrta_star_search(start, goal, neighbors, heuristic,
                 max_moves, hash = {}, equal = {});
```

各実行状態で全隣接辺の $c(s,s')+H(s')$ を評価し、その最小値で $H(s)$ を単調に更新して最良隣接状態へ 1 手進む Learning Real-Time A*。1 手の最大分岐数を $b$ とすると、期待時間計算量は $O(Mb)$、空間計算量は $O(M+D)$。有限グラフ、正の辺、ゴールへの到達可能性、許容的な初期値という標準的な条件の下で、反復学習により収束する。

## `rta_star_search`

```cpp
rta_star_search(start, goal, neighbors, heuristic,
                max_moves, hash = {}, equal = {});
```

最良隣接状態へ進む一方、現在状態を 2 番目に良い $c+H$（候補が 1 個なら最良値）まで引き上げる Real-Time A*。LRTA* より強く直前状態への戻りを抑制する別のバックアップ規則である。時間計算量は $O(Mb)$、空間計算量は $O(M+D)$。

## `rtaa_star_search`

```cpp
rtaa_star_search(start, goal, neighbors, heuristic,
                 lookahead, max_moves, hash = {}, equal = {});
```

各意思決定で最大 `lookahead` 状態の限定 A* を行い、最良のフロンティアの $f^{\ast}$ に対して CLOSED の各状態を $H(s)\gets\max(H(s),f^{\ast}-g(s))$ で更新する Real-Time Adaptive A*。ゴールが先読み内なら残経路をまとめて実行し、それ以外は先頭 1 手を実行する。

1 回の先読みで発見する状態・辺を $S,A$、キュー最大量を $Q$ とすると、最悪時間計算量は $O(M(A+S)\log Q)$、空間計算量は $O(M+S+D)$。実際の意思決定回数は最大 $M$。

## `lss_lrta_star_search`

```cpp
lss_lrta_star_search(start, goal, neighbors, heuristic,
                     lookahead, max_moves, hash = {}, equal = {});
```

限定 A* で Local Search Space を作り、全フロンティアの現在 $H$ を始点とする逆向き Dijkstra バックアップで CLOSED の値を更新する LSS-LRTA*。RTAA* の一様な $f^{\ast}-g$ 更新とは異なり、局所空間内の辺コストと複数のフロンティアをすべて反映する。

局所空間の状態数・辺数を $S$, $A$ とすると、1 回の意思決定あたり限定 A* と逆向き Dijkstra を合わせて時間計算量 $O((S+A)\log S)$・一時空間 $O(S+A)$ で、全体の最悪時間計算量は $O(M(S+A)\log S)$、空間計算量は $O(M+S+A+D)$。

## 注意点

- `lookahead` は正、辺のコストとヒューリスティックは非負の有限値でなければならない。不正な値には `std::invalid_argument` を、整数加算のオーバーフローには `std::overflow_error` を送出する。探索中に `neighbors` の結果が変わったことを検出すると `std::logic_error` を送出する。
- `max_moves` は停止保証を与える実行上限であり、到達不能と上限不足は `cost` だけでは区別せず、`cutoff` も確認する。
- 返す学習表は当該呼び出しのスナップショットである。環境が変化しない場合の収束保証には、呼び出し内で十分な `max_moves` を与えるか、返却値を上位層で利用する設計が必要である。
- ゼロコスト閉路では実時間法が停滞し得る。標準的な完全性・収束性の仮定には有限状態空間、正コスト、およびゴール到達可能性が含まれる。
- `neighbors` は同一状態へ複数回呼ばれるため、探索中に辺集合を変える関数は使用しない。
