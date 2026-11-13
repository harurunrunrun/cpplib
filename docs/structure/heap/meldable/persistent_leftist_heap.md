---
title: Persistent Leftist Heap (完全永続左偏ヒープ)
documentation_of: ../../../../src/structure/heap/meldable/persistent_leftist_heap.hpp
---

左偏ヒープ（leftist heap）を経路コピーで永続化し、過去のバージョンを保持したまま併合（meld）できる優先度付きキュー。

```cpp
PersistentLeftistHeap<T, MAX_VERSION, MAX_NODE, Compare> heap;
PersistentLeftistHeap<T, MAX_VERSION, MAX_NODE, Compare> heap(compare);
```

テンプレート引数は `PersistentLeftistHeap<T, MAX_VERSION = SIZE_MAX, MAX_NODE = SIZE_MAX, Compare = std::less<T>>` である。

- `MAX_VERSION`: バージョン数の上限（初期バージョン $0$ も数える）
- `MAX_NODE`: 空ノードを含むノード配列の容量。実際のノードとして使えるのは `MAX_NODE - 1` 個である（`INT_MAX` 以下）
- `Compare`: 優先順位。デフォルトの `std::less<T>` では最小値が先頭の min-heap となる

`MAX_VERSION`、`MAX_NODE` を省略（`SIZE_MAX`）すると、ノードとバージョンを `std::vector` に使用量に合わせて確保する可変長版になる。指定した場合は固定容量の `std::array` を本体に持つ。

```cpp
using Dynamic = PersistentLeftistHeap<long long>;
```

バージョン $0$ は空のヒープである。各更新は新しいバージョンを末尾に 1 個追加し、その番号を返す。バージョン番号の型は `std::size_t` である。

## versions / nodes_used

```cpp
size_t versions() const
size_t nodes_used() const
```

現在存在するバージョン数と、経路コピーで確保済みの実ノード数を返す。

## size / empty

```cpp
size_t size(size_t version) const
bool empty(size_t version) const
```

指定したバージョンの論理的な要素数と、空かどうかを返す。同じバージョンどうしを併合した場合は、各要素を 2 個ずつ含むものとして数える。

## top

```cpp
const T& top(size_t version) const
```

`Compare` で最も優先される要素への参照を返す。デフォルトの `std::less<T>` では最小値を返す。

## fork

```cpp
size_t fork(size_t version)
```

内容を共有する新しいバージョンを作る。ノードは確保しない。

## push

```cpp
size_t push(size_t version, const T& value)
```

指定したバージョンに `value` を 1 個追加した新しいバージョンを返す。元のバージョンは変化しない。

## pop

```cpp
size_t pop(size_t version)
```

指定したバージョンの先頭を 1 個削除した新しいバージョンを返す。元のバージョンは変化しない。

## meld

```cpp
size_t meld(size_t left_version, size_t right_version)
```

2 つのバージョンの全要素を合わせた新しいバージョンを返す。入力のバージョンは変化しない。同じバージョンを 2 回指定した場合も、その要素を 2 回ずつ含むヒープを作る。

## 時間計算量

対象のバージョンの要素数を $N$（`meld` では 2 つのバージョンの要素数を $N$, $M$）とする。

- `versions`、`nodes_used`、`size`、`empty`、`top`、`fork`: $O(1)$
- `push`: $O(\log(N+1))$。ノードを $O(\log(N+1))$ 個消費する
- `pop`: $O(\log N)$。ノードを $O(\log N)$ 個消費する
- `meld`: $O(\log(N+1) + \log(M+1))$。同じ量のノードを消費する
- コンストラクタ: 固定長版は配列を値初期化するので $O(\texttt{MAX_VERSION}+\texttt{MAX_NODE})$、可変長版は $O(1)$

## 空間計算量

- 本体: 固定長版は $O(\texttt{MAX_VERSION}+\texttt{MAX_NODE})$、可変長版は作成したバージョン数とノード数に比例する
- `push`、`pop`、`meld` は、上記の対数個の永続ノードに加えて、同じオーダーの再帰スタックを使う
- それ以外の関数の追加領域は $O(1)$

## 注意点

- `T` はデフォルト構築とコピー代入ができ、`Compare` は狭義の弱順序を定める必要がある。
- `top` が返した参照は、固定長版ではヒープのオブジェクトが破棄されるまで有効である。可変長版では、後続の更新に伴う再確保で無効になることがあるので、必要な値は更新前にコピーする。
- バージョン番号が範囲外の場合、空のヒープへの `top`・`pop`、バージョン容量またはノード容量の超過では `std::runtime_error` を送出する。
- 論理的な要素数が `std::size_t` に収まらない `push`・`meld` では `std::overflow_error` を送出する。
- 更新が例外を送出した場合、バージョン数と使用ノード数は更新前に戻る。
- 固定長版はオブジェクト本体に固定容量の配列を持つ。容量を大きくする場合は、静的記憶域またはヒープ上に配置する。
