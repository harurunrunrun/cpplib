---
title: Persistent Wavelet Matrix (完全永続ウェーブレット行列)
documentation_of: ../src/structure/wavelet_matrix/persistent_wavelet_matrix.hpp
---

完全永続の点更新wavelet matrix。
任意のバージョンから分岐して `set` できる。

## テンプレート引数

```cpp
PersistentWaveletMatrix<T, MAX_SIZE, MAX_VERSION, BIT_WIDTH, BLOCK_SIZE>
```

## 更新

```cpp
int nv = wm.set(version, k, value)
int nv = wm.fork(version)
```

## クエリ

```cpp
wm.access(version, k)
wm.rank(version, value, l, r)
wm.select(version, value, k)
wm.kth_smallest(version, l, r, k)
wm.kth_largest(version, l, r, k)
wm.range_freq(version, l, r, upper)
wm.range_freq(version, l, r, lower, upper)
wm.prev_value(version, l, r, upper)
wm.next_value(version, l, r, lower)
```

## 時間計算量

$B = BLOCK\_SIZE$、$M = \lceil N / B \rceil$ とする。
区間が触れるブロック数を $C$ とする。

- `set`: $O(B\log B + \log M)$
- `fork`: $O(1)$
- `access`: $O(\log M)$
- `rank`, `range_freq`: $O(B + C(\log B + \log M))$
- `kth_smallest`: $O(BIT\_WIDTH \cdot (B + C(\log B + \log M)))$

ブロック参照は永続セグメント木で管理する。
`set` は変更ブロックと参照木の根から葉までだけを複製し、
`fork` は参照木の根を共有する。

更新を行ったバージョン数を $U$、全バージョン数を $V$ とした追加メモリは
$O(N + U(B + \log M) + V)$。

## 計算量（公開操作別）

$D=\mathtt{BIT\_WIDTH}$、$M=\lceil N/B\rceil$、
$M_{max}=\lceil\mathtt{MAX\_SIZE}/B\rceil$ とする。
長さ $L$ の区間が触れるblock数を $C$ とし、
$Q(L)=B+C(\log B+\log(M+1))$ とおく。

- default/vector/array constructor: $O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}(B+\log(M_{max}+1))+N\log B)$
- `size`, `versions`, `latest_version`, `fork`: $O(1)$
- `access`: $O(\log(M+1))$
- `set`: $O(B\log B+\log(M+1))$
- `rank`, `range_freq`: $O(Q(L))$
- `select`: $O(Q(N)\log N)$
- `kth_smallest`, `kth_largest`, `prev_value`, `next_value`: $O(DQ(L))$

## 注意点

version 0が初期列。`set` は指定versionから1点を置換した新version、`fork` は同内容の
新versionを返す。`access` と全queryは先頭にversionを受け取る。`size/versions/latest_version`
は列長・version数・最新番号を返す。

点・出現番号・順序統計は0-indexed。列区間は半開区間 `[l,r)`、値域は
`[lower,upper)`。`select` は該当なしなら `size()`、前後値は `nullopt`。
不正なversion・点・区間・順序・値域、bit幅、列長/version/block node容量超過では
`runtime_error`。失敗した更新は使用量を戻す。各APIの計算量は上記表の通り。

## Constructor signature

```cpp
PersistentWaveletMatrix()
PersistentWaveletMatrix(const vector<T>& values)
PersistentWaveletMatrix(const array<T, N>& values)
```
