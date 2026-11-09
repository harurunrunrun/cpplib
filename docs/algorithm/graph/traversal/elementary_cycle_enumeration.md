---
title: Elementary Cycle Enumeration (単純有向閉路列挙)
documentation_of: ../../../../src/algorithm/graph/traversal/elementary_cycle_enumeration.hpp
---

有向グラフの単純閉路を、巡回開始位置の違いによる重複なしですべて列挙する。

## API

```cpp
vector<vector<int>> enumerate_elementary_cycles(
    vector<vector<int>> graph
)
```

各閉路を、最小の頂点から始まる頂点列として返す。末尾に先頭の頂点は重ねない。戻り値全体は辞書順に並ぶ。同じ有向辺が複数ある場合は 1 本にまとめる。

## 時間計算量

頂点数を $N$、重複除去後の辺数を $M$、単純閉路数を $C$ とする。

- 隣接リストのソートと重複除去: $O(M\log(N+1))$
- Johnson 法による列挙: 依存集合のハッシュ操作を償却 $O(1)$ として $O((N+M)(C+1))$
- 出力の辞書順ソート: $O(C\log(C+1)\,N)$

## 空間計算量

出力を除いて $O(N+M)$、出力には閉路の長さの総和に比例する領域が必要である。

## 注意点

- 隣接頂点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。
- 自己ループは長さ 1 の単純閉路として列挙する。
- 逆向きの閉路は有向辺の列が異なるため、別の閉路として扱う。
- 探索は再帰で実装されているため、非常に長い閉路を持つグラフでは再帰の深さが頂点数程度になる。
