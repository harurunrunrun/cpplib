---
title: Approximate State-Space Dynamic Programming (状態空間の近似動的計画法)
documentation_of: ../../../src/approximate/dp/approximate_state_dp.hpp
---

層ごとに状態を展開する汎用 DP の制御器である。コールバック `expand(state, layer)` は次の状態の range を返し、`score(state)` は値が大きいほど良いものとする。

層ごとに処理する各関数は `StateDpResult<State>` を返す。これは保持している状態 `states`、完了した層の数 `layers_completed`、中断したかを表す `interrupted` を持つ。コールバックは決定的でなくてもよいが、比較関数は strict weak ordering と整合していなければならない。

以下、1 層で生成された状態数を $N$、保持数の上限を $K$、使われているバケット数を $B$ とする。遷移結果を一度全て展開するため、層の処理中の空間は $O(N)$、保持する空間は $O(K)$ である。

## 状態数の制限

```cpp
beam_dynamic_programming(initial_states, layer_count, expand, score, beam_width);
top_k_dynamic_programming(initial_states, layer_count, expand, score, k);
hash_beam_dynamic_programming(initial_states, layer_count, expand, score,
                              bucket_count, beam_width, hash = {});
state_count_limited_dynamic_programming(initial_states, layer_count, expand, maximum_states);
stochastic_beam_dynamic_programming(initial_states, layer_count, expand, beam_width, random);
```

| API | 保持の規則 | 層ごとの制御の時間計算量 |
| --- | --- | ---: |
| `beam_dynamic_programming` | 線形時間の選択の後、上位をビーム幅だけ残す | $O(N+K\log K)$ |
| `top_k_dynamic_programming` | 部分ソートで上位 $K$ 個を残す | $O(N\log K)$ |
| `hash_beam_dynamic_programming` | ハッシュのバケットごとに最良を残した後、ビームで絞る | 期待 $O(N+B\log B)$ |
| `state_count_limited_dynamic_programming` | 遷移順で先頭の状態を残す | $O(N)$ |
| `stochastic_beam_dynamic_programming` | 一様にシャッフルした後に打ち切る | $O(N)$ |

## 統合と近似

- `sparse_dynamic_programming(initial_states, layer_count, expand, key, score, hash = {})`: 厳密なキーごとにスコア最大の状態を残す。期待 $O(N)$。
- `value_rounding_dynamic_programming(initial_states, layer_count, expand, value, quantum)`: 値を幅 `quantum` の加法的なバケットに丸めて統合する。期待 $O(N)$。
- `state_compression_dynamic_programming(initial_states, layer_count, expand, compress, hash = {}, equal = {})`: 状態を正規形に変換し、完全に一致する重複を除く。期待 $O(N)$。
- `state_aggregation_dynamic_programming(initial_states, layer_count, expand, key, aggregate, hash = {})`: 同じキーの状態を結合的なコールバックで集約する。期待 $O(N)$。
- `dominance_pruned_dynamic_programming(initial_states, layer_count, expand, dominates)`: 支配されない状態の集合（フロンティア）を残す。支配判定は $O(N^2)$ 回。
- `epsilon_dominance_dynamic_programming(initial_states, layer_count, expand, objectives, epsilon)`: 目的値ベクトルを乗法的な $\varepsilon$ の箱に分け、箱ごとに 1 状態を残す。目的の次元を $D$ として期待 $O(ND)$。

ハッシュを使う関数の期待計算量は、呼び出し側が与えるハッシュが十分に分散することを仮定する。

## フロンティアと中断

`approximate_pareto_frontier(candidates, objectives, epsilon)` は、非負の最大化目的を乗法的な $\varepsilon$ の箱に圧縮した後、代表どうしの厳密な支配関係で支配されるものを除く。箱の代表の個数を $F$ とすると、時間計算量は $O(ND+F^2D)$、追加空間計算量は $O(FD)$ である。

`interrupted_dynamic_programming(initial_states, layer_count, expand, stop)` は、各層の処理前に `stop(layer, states)` を評価する。停止が要求されると、その時点の状態を `interrupted == true` として返す。時間やメモリの予算、外部の停止フラグを安全に接続できる。

## 注意点

- 保持数の上限は正でなければならない。$\varepsilon$ と丸め幅 `quantum` は正の有限値でなければならない。$\varepsilon$ 支配で使う目的値は有限・非負で、次元が一定（かつ空でない）でなければならない。
- 不正な値には `std::invalid_argument`、バケット番号のオーバーフローには `std::overflow_error` を送出する。
- いずれもヒューリスティックであり、状態を捨てるため最適解は保証しない。
