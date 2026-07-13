---
title: Persistent Leftist Heap (完全永続左偏ヒープ)
documentation_of: ../src/structure/heap/persistent_leftist_heap.hpp
---

leftist heapをpath copyingして、過去のversionを保持したままmeld可能なpriority queueを構成する。nodeとversionは固定容量の `std::array` に保存する。

~~~cpp
PersistentLeftistHeap<T, MAX_VERSION, MAX_NODE, Compare> heap;
~~~

version `0` は空heapである。各更新は新しいversionを末尾に1個追加し、そのversion番号を返す。`Compare = std::less<T>` の既定値ではmin-heapとなる。

`MAX_VERSION` はversion数の上限で、初期version `0` も数える。`MAX_NODE` はnull nodeを含むnode配列の容量なので、実nodeとして使えるのは `MAX_NODE - 1` 個である。

# version数・使用node数

~~~cpp
heap.versions()
heap.nodes_used()
~~~

現在存在するversion数と、path copyingで確保済みの実node数を返す。ともに `O(1)` 時間、`O(1)` 追加空間。

# 要素数

~~~cpp
heap.size(version)
heap.empty(version)
~~~

指定versionの論理的な要素数と、空かどうかを返す。同じversion同士をmeldした場合は各要素を2個ずつ含むものとして数える。ともに `O(1)` 時間、`O(1)` 追加空間。

# 先頭

~~~cpp
heap.top(version)
~~~

`Compare` で最優先となる要素への `const T&` を返す。既定の `std::less<T>` では最小値を返す。`O(1)` 時間、`O(1)` 追加空間。

# versionの複製

~~~cpp
heap.fork(version)
~~~

内容を共有する新しいversionを作る。nodeは確保しない。`O(1)` 時間、`O(1)` 追加空間。

# 追加

~~~cpp
heap.push(version, value)
~~~

指定versionへ `value` を1個追加した新しいversionを返す。元のversionは変化しない。要素数を `N` とすると `O(log(N+1))` 時間、`O(log(N+1))` nodeを消費する。

# 削除

~~~cpp
heap.pop(version)
~~~

指定versionの先頭を1個削除した新しいversionを返す。元のversionは変化しない。要素数を `N` とすると `O(log N)` 時間、`O(log N)` nodeを消費する。

# meld

~~~cpp
heap.meld(left_version, right_version)
~~~

2つのversionの全要素を合わせた新しいversionを返す。入力versionは変化しない。同じversionを2回指定した場合も、その要素を2回含むheapを作る。各要素数を `N`, `M` とすると `O(log(N+1) + log(M+1))` 時間、同量のnodeを消費する。

# 時間計算量と空間計算量（公開API別）

各APIの時間計算量と追加空間計算量は各節に記載した通り。`versions`、`nodes_used`、`size`、`empty`、`top`、`fork` はいずれも時間・追加空間 $O(1)$ である。`push`、`pop`、`meld` は各節に示した対数時間と同数の永続nodeに加え、同じorderの再帰stackを使う。constructorは固定arrayをvalue-initializeする時間が $O(\mathtt{MAX\_VERSION}+\mathtt{MAX\_NODE})$、呼び出し中の追加領域は $O(1)$ である。

## 注意点

- `T` は既定構築、copy代入ができ、`Compare` でstrict weak orderingを定める必要がある。
- `top` が返した参照はheap objectが破棄されるまで有効である。
- version番号が範囲外、空heapへの `top` / `pop`、version容量またはnode容量の超過では `runtime_error`。
- 論理的な要素数が `std::size_t` に収まらないmeldでは `overflow_error`。
- 更新が例外を送出した場合、version数と使用node数は更新前に戻る。
- object本体に固定容量配列を持つ。容量を大きくする場合はstatic storageまたはheap上へ配置する。
