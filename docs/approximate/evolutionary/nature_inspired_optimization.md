---
title: Nature-Inspired Population Optimization (自然現象型集団最適化)
documentation_of: ../../../src/approximate/evolutionary/nature_inspired_optimization.hpp
---

`std::vector<Real>` の個体に対する箱型制約付きの連続最小化を行う、7 種類の集団型ヒューリスティックである。全ての API は `EvolutionResult<Individual, Score>` を返し、外部から渡された乱数生成器を使う。

## API

### `artificial_bee_colony`

```cpp
artificial_bee_colony(
    food_sources, lower, upper, evaluate,
    iterations, abandonment_limit, rng, compare = {}
)
```

雇用蜂フェーズ、順位に基づくルーレット選択による追従蜂フェーズ、試行回数が上限に達した食料源を再初期化する偵察蜂フェーズを行う ABC である。食料源は 2 個以上、`abandonment_limit` は正でなければならない。

### `bees_algorithm`

```cpp
bees_algorithm(
    scouts, lower, upper, evaluate, iterations,
    selected_sites, elite_sites, recruits_per_selected, recruits_per_elite,
    initial_patch_size, patch_decay, rng, compare = {}
)
```

上位 `selected_sites` 個の地点のうち、上位 `elite_sites` 個により多くの蜂を割り当て、各パッチ内の最良点と残りのランダムな偵察蜂で次世代を作る。パッチの大きさは毎回 `patch_decay` 倍になる。`elite_sites <= selected_sites`、割り当て数は正、`initial_patch_size` は正、`patch_decay` は $(0,1]$ でなければならない。

### `harmony_search`

```cpp
harmony_search(
    harmony_memory, lower, upper, evaluate, iterations,
    memory_consideration_rate, pitch_adjustment_rate, bandwidth,
    rng, compare = {}
)
```

各座標を、メモリからの選択、ピッチ調整、ランダム生成の順で決めて新しいハーモニーを作り、メモリ内の最悪のハーモニーより良ければ置き換える。2 つの率は $[0,1]$、`bandwidth` は非負でなければならない。

### `firefly_algorithm`

```cpp
firefly_algorithm(
    population, lower, upper, evaluate, iterations,
    attractiveness, absorption, randomness, rng, compare = {}
)
```

より良いホタルへ $\beta_0e^{-\gamma r^2}$ の強さで移動し、一様ノイズを加える。3 つの係数は非負でなければならない。

### `cuckoo_search`

```cpp
cuckoo_search(
    nests, lower, upper, evaluate, iterations,
    levy_scale, abandonment_fraction, rng, compare = {}
)
```

コーシー分布による裾の重い飛行で候補の巣を作り、各世代の後に悪い巣のうち割合 `abandonment_fraction` を再初期化する。`levy_scale` は正、`abandonment_fraction` は $[0,1]$ でなければならない。

### `bat_algorithm`

```cpp
bat_algorithm(
    bats, lower, upper, evaluate, iterations,
    minimum_frequency, maximum_frequency, initial_loudness, initial_pulse_rate,
    loudness_decay, pulse_growth, rng, compare = {}
)
```

周波数に基づく速度、最良解の周辺でのランダムウォーク、音量とパルス率のスケジュールを使う。`initial_loudness` と `pulse_growth` は非負、`initial_pulse_rate` と `loudness_decay` は $[0,1]$ でなければならない。

### `grey_wolf_optimizer`

```cpp
grey_wolf_optimizer(wolves, lower, upper, evaluate, iterations, rng, compare = {})
```

評価値の上位 3 個体を alpha, beta, delta とし、3 体のリーダーが与える位置の平均へ各個体を更新する。個体は 3 個以上必要である。

## 計算量

集団サイズを $N$、次元を $D$、反復回数を $I$、評価 1 回の時間を $E$ とする。`bees_algorithm` の $R$ は 1 反復あたりの割り当て蜂数の合計である。

| API | 時間計算量 | 追加空間計算量 |
| --- | --- | --- |
| `artificial_bee_colony` | $O(NE+I(ND+NE+N\log N))$ | $O(ND+N)$ |
| `bees_algorithm` | $O(NE+I(N\log N+R(D+E)+N(D+E)))$ | $O(ND+N)$ |
| `harmony_search` | $O(NE+I(D+E+N\log N))$ | $O(ND+N)$ |
| `firefly_algorithm` | $O(NE+IN^2(D+E))$ | $O(ND+N)$ |
| `cuckoo_search` | $O(NE+I(N(D+E)+N\log N))$ | $O(ND+N)$ |
| `bat_algorithm` | $O(NE+I(ND+NE+N\log N))$ | $O(ND+N)$ |
| `grey_wolf_optimizer` | $O(NE+I(ND+NE+N\log N))$ | $O(ND+N)$ |

`EvolutionResult` のスカラーのメンバの参照は $O(1)$、個体のコピーは $O(D)$ である。

## 注意点

- 集団の全個体と境界は同じ正の次元を持ち、全個体は境界内になければならない。
- 率・確率などのパラメータは各 API で示した範囲を満たさなければならず、違反時は `std::invalid_argument` を送出する。
- Cuckoo Search は、裾の重い Lévy 飛行の実用的な代用としてコーシー分布による飛行を使う。厳密な安定分布からの標本化ではない。
- 同じシードでも、標準ライブラリの実装が異なれば乱数列は一致しない。
- いずれもヒューリスティックであり、大域最適性は保証しない。
