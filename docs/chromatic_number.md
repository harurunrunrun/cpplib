---
title: Chromatic Number
documentation_of: ../src/algorithm/graph/chromatic_number.hpp
---

無向グラフの彩色数を求める。

## `chromatic_number`

```cpp
int answer = chromatic_number(graph);
```

`graph[v]`に頂点`v`の隣接頂点を入れる。隣接関係は対称な無向グラフで
なければならない。自己ループは無視する。空グラフでは0を返す。

各頂点部分集合が含む独立集合数をsubset DPで数え、彩色多項式

$$
P_G(k)=\sum_{S\subseteq V}(-1)^{N-|S|}I(S)^k
$$

を3個の素数を法として評価する。$N\leq20$では$0\leq P_G(k)\leq20^{20}$で、
3法の積がこの上限を超えるため、確率的衝突のない決定的判定になる。

## 例外・制約

実装上は$N\leq20$。頂点数超過、範囲外の隣接頂点、非対称な辺では
`std::runtime_error`を送出する。

## 計算量

$N$を頂点数、$M$を隣接リストの総要素数、答えを$K$とする。

| API・操作 | 時間計算量 | 追加領域 |
| --- | --- | --- |
| 入力検査 | $O(N^2+M)$ | $O(N)$ |
| 独立集合数の前計算 | $O(2^N)$ | $O(2^N)$ |
| 彩色多項式の評価 | $O(K2^N)$ | $O(2^N)$ |
| `chromatic_number`全体 | $O(N^2+M+K2^N)$ | $O(2^N+N)$ |
| 戻り値の参照 | $O(1)$ | $O(1)$ |
