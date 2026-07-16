---
title: Letter Pipe Maximum Flow (英字頂点配管最大流) [MTOTALF]
documentation_of: ../src/algorithm/graph/letter_pipe_max_flow.hpp
---

英大文字・英小文字で識別される無向配管網について、`A` から `Z` への最大流量を返す。

## LetterPipe

```cpp
template<class T>
struct LetterPipe {
    char from;
    char to;
    T capacity;
};
```

英字名 `from` と `to` を結ぶ容量 `capacity` の無向配管を表す。

## letter_pipe_max_flow

```cpp
template<class T>
T letter_pipe_max_flow(const vector<LetterPipe<T>>& pipes);
```

`A-Z` と `a-z` を相異なる52頂点として、`A` から `Z` への最大流量を返す。多重配管を入力できる。

## API別の時間計算量・空間計算量

$M=pipes.size()$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `letter_pipe_max_flow` | $O(M)$（頂点数52を定数とした場合） | $O(M)$ |

## 注意点

- 頂点名は `A-Z` または `a-z`、容量は非負でなければならない。
- 前提違反時は `runtime_error` を送出する。
- 最大流量と中間計算は `T` で表現できる必要がある。
