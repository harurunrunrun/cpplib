---
title: Persistent Point Add Range Sum (完全永続一点加算区間和) [PSEGTREE]
documentation_of: ../../../../src/structure/segtree/persistent/persistent_point_add_range_sum.hpp
---

任意の過去の version を残したまま、1 点に値を加えた新しい version を作る。各 version に対する区間和クエリを扱う。内部では `PersistentSegtree` に加法モノイドを与えて使う。

容量（`MAX_SIZE`, `MAX_VERSION`）を両方省略すると可変長版になり、`PersistentPointAddRangeSum<T> tree(n);` のように長さを実行時に与える。

## コンストラクタ

```cpp
(1) PersistentPointAddRangeSum<T, MAX_SIZE, MAX_VERSION>(
        int size = MAX_SIZE  // 可変長版の既定値は 0
    )
(2) PersistentPointAddRangeSum<T, MAX_SIZE, MAX_VERSION>(
        const vector<T>& values
    )
(3) PersistentPointAddRangeSum<T, MAX_SIZE, MAX_VERSION>(
        const array<T, N>& values
    )
```

1. 長さ `size`、各要素が `T(0)` の version 0 を構築する。
2. `values` を version 0 として構築する。
3. `values` を version 0 として構築する。

時間計算量は $O(\texttt{MAX_SIZE}+\texttt{MAX_VERSION}\log\texttt{MAX_SIZE})$ で、オブジェクトが保持する空間も同じオーダーである。

## `size`

```cpp
int size() const
```

列の長さを返す。時間計算量と追加の空間計算量は $O(1)$ である。

## `versions`

```cpp
int versions() const
```

存在する version 数を返す。時間計算量と追加の空間計算量は $O(1)$ である。

## `latest_version`

```cpp
int latest_version() const
```

最後に作った version 番号を返す。時間計算量と追加の空間計算量は $O(1)$ である。

## `add`

```cpp
int add(int version, int position, const T& delta)
```

`version` の `position` 番目に `delta` を加えた新しい version を作り、その番号を返す。指定した version は変更しない。時間計算量は $O(\log\texttt{MAX_SIZE})$ で、更新により保持する追加の空間は $O(\log\texttt{MAX_SIZE})$ である。

## `sum`

```cpp
T sum(int version, int left, int right) const
```

`version` の半開区間 $[left, right)$ の和を返す。時間計算量は $O(\log\texttt{MAX_SIZE})$、追加の空間計算量は $O(\log\texttt{MAX_SIZE})$ である。

## 注意点

- version 0 が初期状態である。`0 <= version < versions()` の version を指定する。
- `0 <= position < size()`、`0 <= left <= right <= size()` が必要である。
- version 0 から追加できる version 数は `MAX_VERSION` 以下である。
- 不正な容量・version・添字・区間、version・ノードの容量超過では `std::runtime_error` を送出する。
- `T` は 0 からの構築・コピー・加算に対応し、すべての和を表現できる必要がある。
- オブジェクトのコピーとムーブはできない。
