---
title: Color Coding
documentation_of: ../src/approximate/randomized/color_coding.hpp
---

有向グラフ中の頂点数 $K$ の単純パスをColor Codingで探索する。
無向グラフでは各辺を両方向に入れる。乱数生成器を参照で受けるため、seedを
固定すれば結果も再現できる。

## `ColorCodingPathResult`

`path`、実行した試行数 `trials`、発見したかを示す `found` を保持する。
`found == false`は指定試行内で未発見という意味で、パスが存在しない証明ではない。

各メンバの参照は $O(1)$、`path` のコピーは $O(K)$ 時間・領域である。

## `color_coding_simple_path`

```cpp
auto result = approximate::randomized::color_coding_simple_path(
    graph, vertex_count, trial_limit, rng
);
```

各試行で頂点を $K$ 色に一様独立に彩色し、部分集合DPで全色が異なるパスを
探索・復元する。隣接先が頂点範囲外なら `std::invalid_argument`、部分集合表を
表現・確保できない大きさなら `std::length_error` を送出する。自己辺と多重辺は
許す。

`vertex_count == 0` は空パス、`vertex_count == 1` かつ頂点があれば頂点0を
試行なしで返す。`vertex_count` が頂点数より大きい場合も試行せず失敗を返す。

頂点数を $N$、辺数を $M$、実行試行数を $R$ とすると、時間計算量は
$O(N+M+R2^K(N+M))$、追加領域は $O(N2^K)$ である。

固定した $K$ 頂点の単純パスが1試行でcolorfulになる確率は
$K!/K^K$ である。従ってそのパスを見逃す確率は独立な $R$ 試行後に
$(1-K!/K^K)^R$ 以下である。見つけたパスは常に実在するため偽陽性はない。

## `is_valid_color_coding_path`

返却パスの長さ、頂点の相異性、各有向辺、`found`との整合を検査する。
不正なグラフに対しても例外を送出せずfalseを返す。

時間計算量は $O(N+M)$、追加領域は $O(N)$ である。

## `validate_color_coding_path`

同じ検査を行い、不整合なら `std::invalid_argument` を送出する。

時間計算量は $O(N+M)$、追加領域は $O(N)$ である。
