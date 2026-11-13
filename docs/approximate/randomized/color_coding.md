---
title: Color Coding (色分け法)
documentation_of: ../../../src/approximate/randomized/color_coding.hpp
---

有向グラフ中の頂点数 $K$ の単純パスを Color Coding で探索する。
無向グラフでは各辺を両方向に入れる。乱数生成器を参照で受け取るため、シードを固定すれば結果を再現できる。

## `ColorCodingPathResult`

パス `path`、実行した試行回数 `trials`、発見できたかどうか `found` を保持する。
`found == false` は指定回数の試行では見つからなかったという意味であり、パスが存在しないことの証明ではない。

各メンバの参照は $O(1)$、`path` のコピーは時間計算量・追加空間計算量ともに $O(K)$ である。

## `color_coding_simple_path`

```cpp
auto result = approximate::randomized::color_coding_simple_path(
    graph, vertex_count, trial_limit, rng
);
```

各試行で頂点を $K$ 色に一様かつ独立に彩色し、部分集合 DP で全頂点の色が相異なるパスを探索・復元する。$K$ は `vertex_count` である。
隣接先が頂点番号の範囲外なら `std::invalid_argument` を、部分集合の表を表現・確保できない大きさなら `std::length_error` を送出する。自己辺と多重辺は許す。

`vertex_count == 0` なら空パスを、`vertex_count == 1` で頂点が 1 つ以上あれば頂点 0 だけのパスを、試行せずに返す（`found == true`）。`vertex_count` がグラフの頂点数より大きい場合も、試行せずに失敗を返す。

頂点数を $N$、辺数を $M$、実行した試行回数を $R$ とすると、時間計算量は $O(N + M + R \cdot 2^K (N + M))$、追加空間計算量は $O(N 2^K)$ である。

固定した $K$ 頂点の単純パスが 1 回の試行で colorful（全頂点の色が相異なる）になる確率は $K!/K^K$ である。したがって、独立な $R$ 回の試行の後にそのパスを見逃す確率は $(1 - K!/K^K)^R$ 以下である。見つけたパスは必ず実在するため、偽陽性はない。

## `is_valid_color_coding_path`

返されたパスの長さ、頂点が相異なること、各有向辺の存在、`found` との整合性を検査する。
不正なグラフに対しても例外を送出せず、`false` を返す。

時間計算量は $O(N + M)$、追加空間計算量は $O(N)$ である。

## `validate_color_coding_path`

同じ検査を行い、不整合があれば `std::invalid_argument` を送出する。

時間計算量は $O(N + M)$、追加空間計算量は $O(N)$ である。

## 注意点

確率的保証は、各 API で述べた独立性と入力条件の下で成り立つ。同じ結果を再現するには、同じ乱数生成器の状態・入力・標準ライブラリ実装が必要である。中間演算は使用する型の表現範囲に収まらなければならない。
