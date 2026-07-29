---
title: Nature-Inspired Population Optimization (自然現象型集団最適化)
documentation_of: ../../../src/approximate/evolutionary/nature_inspired_optimization.hpp
---

`vector<Real>` の box 制約付き連続最小化を行う7種の population heuristic。
全 API は `EvolutionResult<Individual, Score>` を返し、外部 URBG を使う。

## APIs

### `artificial_bee_colony`

```cpp
artificial_bee_colony(
    food_sources, lower, upper, evaluate,
    iterations, abandonment_limit, rng, compare = {}
)
```

employed phase、rank roulette onlooker phase、trial limit に達した scout
再初期化を行う ABC。source は2個以上、limit は正。

### `bees_algorithm`

上位 `selected_sites` のうち `elite_sites` に多く recruit し、patch 内最良と
残り random scout で次世代を作る。patch は `patch_decay` 倍する。

### `harmony_search`

各座標を memory consideration、pitch adjustment、random generation の順で
生成し、memory 最悪 harmony より良ければ置換する。

### `firefly_algorithm`

より良い firefly へ $\beta_0e^{-\gamma r^2}$ で移動し、uniform noise を加える。

### `cuckoo_search`

Cauchy heavy-tailed flight で候補 nest を作り、各世代後に worst nest の
`abandonment_fraction` を再初期化する。

### `bat_algorithm`

周波数による velocity、best 周辺 random walk、loudness/pulse schedule を使う。

### `grey_wolf_optimizer`

score 上位3個体を alpha/beta/delta とし、3 leader が与える位置の平均へ更新する。
wolf は3個以上必要。

## 計算量

population $N$、次元 $D$、反復 $I$、評価 $E$ とする。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `artificial_bee_colony` | $O(NE+I(ND+NE+N\log N))$ | $O(ND+N)$ |
| `bees_algorithm` | $O(NE+I(N\log N+R(D+E)+N(D+E)))$ | $O(ND+N)$ |
| `harmony_search` | $O(NE+I(D+E+N\log N))$ | $O(ND+N)$ |
| `firefly_algorithm` | $O(NE+IN^2(D+E))$ | $O(ND+N)$ |
| `cuckoo_search` | $O(NE+I(N(D+E)+N\log N))$ | $O(ND+N)$ |
| `bat_algorithm` | $O(NE+I(ND+NE+N\log N))$ | $O(ND+N)$ |
| `grey_wolf_optimizer` | $O(NE+I(ND+NE+N\log N))$ | $O(ND+N)$ |

`EvolutionResult` scalar member 参照は $O(1)$、individual copy は $O(D)$。

## 注意点

- 全 population 個体と bounds は同次元で bounds 内、次元は非ゼロ。
- rate/probability は各 API の指定範囲を満たす必要がある。
- Cuckoo Search uses a Cauchy flight as a practical heavy-tailed Lévy-flight
  surrogate; exact stable-law sampling is not implied.
- 同じ seed でも標準ライブラリ実装が違えば乱数列は一致しない。
- いずれも heuristic であり、大域最適性を保証しない。
