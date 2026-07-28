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

各閉路を、最小頂点から始まる頂点列として返す。末尾に先頭頂点は重ねない。戻り値全体は辞書順。同じ有向辺が複数ある場合は1本へまとめる。

## 時間計算量

頂点数を $N$、重複除去後の辺数を $M$、単純閉路数を $C$ とする。

- 隣接リストの整列と重複除去: $O(M\log(N+1))$
- Johnson法による列挙: 依存集合のハッシュ操作を償却 $O(1)$ として $O((N+M)(C+1))$
- 出力の辞書順整列: $O(C\log(C+1)\,N)$

## 空間計算量

出力を除いて $O(N+M)$、出力に閉路長の総和だけ必要。

## 注意点

隣接頂点は $[0,N)$ でなければならない。違反時は `runtime_error` を送出する。自己ループは長さ1の単純閉路として列挙する。逆向きの閉路は有向辺列が異なるため別の閉路である。
