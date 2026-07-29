---
title: Advanced Beam Search (高度なビーム探索)
documentation_of: ../../../src/approximate/search/advanced_beam_search.hpp
---

幅制限、反復、バックトラックを組み合わせた状態空間探索を提供する。各 API は
`HeuristicSearchResult<State, Cost>` を返し、解を発見できなければ `cost` と
`path` は空になる。

## `stochastic_beam_search`

```cpp
stochastic_beam_search(
    start, goal, neighbors, heuristic, beam_width, maximum_depth,
    generator, temperature = 1, hash = {}, equal = {})
```

各深さの候補を $g+h$ に基づく重み付き非復元抽出で選ぶ。Gumbel key を使うため、
指数重みを直接計算するときの underflow を避ける。`temperature` が小さいほど
低評価値を優先し、大きいほど選択が一様に近づく。深さ
`maximum_depth` に達するか、候補がなくなるか、解を発見すると停止する。

幅を $B$、最大分岐数を $b$、深さ上限を $D$ とすると、時間計算量は候補の
部分ソートを含めて $O(DBb\log(Bb))$、空間計算量は $O(DBb)$。

## `beam_stack_search`

```cpp
beam_stack_search(
    start, goal, neighbors, heuristic, beam_width, maximum_depth,
    maximum_expansions = numeric_limits<size_t>::max(),
    hash = {}, equal = {})
```

評価値順の候補を幅 `beam_width` ごとの区間に分け、深さ別の stack frame に
未探索区間を保存する。子の区間で解が得られなければ同じ深さの次区間へ戻るため、
通常の Beam Search が捨てる候補も探索する。全区間を調べ終えるか、
`maximum_expansions` 回展開するまでに見つかった最小コストの解を返す。
経路上の同一状態は再訪しない。

単純路の最大数を $P$、最大分岐数を $b$ とすると、最悪時間計算量は
$O(Pb\log b)$、空間計算量は生成済み node と frame を含めて $O(Pb)$。
展開上限を $L$ とした場合、展開処理部分は $O(Lb\log b)$。

## `beam_a_star_search`

```cpp
beam_a_star_search(
    start, goal, neighbors, heuristic, beam_width, maximum_depth,
    maximum_expansions = numeric_limits<size_t>::max(),
    hash = {}, equal = {})
```

A* の global open queue と深さごとの beam を併用する。各深さで open な状態を
評価値の小さい `beam_width` 個に制限し、幅外の最悪候補を破棄する。open が
空になるか、解を取り出すか、展開上限に達すると停止する。幅制限があるため、
許容的ヒューリスティックでも完全性と最適性は保証しない。

実際に生成した node 数を $N$ とすると、時間計算量は $O(N\log N)$、
空間計算量は $O(N)$。各深さで open な有効候補は高々 `beam_width` 個。

## `beam_ida_star_search`

```cpp
beam_ida_star_search(
    start, goal, neighbors, heuristic, beam_width, maximum_depth,
    maximum_iterations, hash = {}, equal = {})
```

IDA* の各 node で successor を $g+h$ 順に並べ、上位 `beam_width` 個だけを
次の深さへ進める。反復ごとに超過した最小評価値へ bound を引き上げる。
解、探索不能、または `maximum_iterations` 到達で停止する。幅制限により
解を見落とす場合がある。

分岐数を $b$、深さ上限を $D$、反復回数を $I$ とすると、最悪時間計算量は
$O(I B^D b\log b)$、探索中の空間計算量は $O(D(B+b))$。

## `chokudai_search`

```cpp
chokudai_search(
    start, goal, neighbors, heuristic, beam_width, maximum_depth,
    iterations, hash = {}, equal = {})
```

深さごとの priority queue を反復間で保持する。各反復で浅い深さから順に
各 queue の上位 `beam_width` 個を展開するため、以前の反復で残った候補を
次の反復で探索できる。指定反復数、全 queue の枯渇、または深さ上限で停止し、
その時点までの最小コスト解を返す。

分岐数を $b$、幅を $B$、深さ上限を $D$、反復数を $I$ とすると、
時間計算量は $O(IDBb\log N)$、空間計算量は生成 node 数 $N$ に対して
$O(N)$。

## 注意点

`neighbors(state)` は `pair<State, Cost>` の range、`goal(state)` は真偽値、
`heuristic(state)` は推定残りコストを返す。全 API で辺コストは非負でなければ
ならない。幅 0、Beam IDA* と Chokudai Search の反復数 0、Stochastic Beam
Search の非正または非有限な温度、Beam IDA* の負ヒューリスティックでは
`std::invalid_argument` を送出する。Stochastic Beam Search で評価値を
`long double` に変換した結果が非有限なら `std::overflow_error` を送出する。
コストの加算は `Cost` の表現範囲に収まる必要がある。
