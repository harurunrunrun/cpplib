---
title: Enumerate Triangles (三角形列挙)
documentation_of: ../src/algorithm/graph/specialized/enumerate_triangles.hpp
---

単純無向グラフのtriangleを1回ずつ列挙する。辺を次数の小さい頂点から大きい頂点へ向け、長さ2の有向pathを調べる。

## triangle列挙

```cpp
long long enumerate_triangles(
    int vertex_count,
    const vector<pair<int, int>>& edges,
    Callback callback
)
```

triangle $(a,b,c)$ごとに`callback(a,b,c)`を1回呼ぶ。callbackへ渡す頂点は`a < b < c`。返り値はtriangle数。

入力は自己loopと多重辺を含まない単純無向グラフでなければならない。頂点番号範囲外と自己loopは例外を送出する。

## 時間計算量

- $O(N+M\sqrt M+T_{callback})$

`T_callback`はcallback全体の実行時間。

## triangle数

```cpp
long long count_triangles(
    int vertex_count,
    const vector<pair<int, int>>& edges
)
```

triangle数だけを返す。

## 時間計算量

- $O(N+M\sqrt M)$

## 空間計算量

- $O(N+M)$

## 注意点

頂点番号は $[0,N)$ に収め、辺の向き、多重辺、自己loop、連結性は各APIで記した入力条件に従う。戻り値の頂点番号も同じ0-indexedである。
