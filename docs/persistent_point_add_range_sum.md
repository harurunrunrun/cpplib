---
title: Persistent Point Add Range Sum (完全永続一点加算区間和) [PSEGTREE]
documentation_of: ../src/structure/segtree/persistent_point_add_range_sum.hpp
---

任意の過去versionを残したまま、一点へ値を加えた新しいversionを作る。
各versionに対する区間和queryを扱う。

## コンストラクタ

```cpp
(1) PersistentPointAddRangeSum<T, MAX_SIZE, MAX_VERSION>(
        int size = MAX_SIZE
    )
(2) PersistentPointAddRangeSum<T, MAX_SIZE, MAX_VERSION>(
        const vector<T>& values
    )
(3) PersistentPointAddRangeSum<T, MAX_SIZE, MAX_VERSION>(
        const array<T, N>& values
    )
```

1. 長さ `size`、各要素が `T(0)` のversion 0を構築する。
2. `values` をversion 0として構築する。
3. `values` をversion 0として構築する。

時間計算量は
$O(\mathtt{MAX\_SIZE}+\mathtt{MAX\_VERSION}\log\mathtt{MAX\_SIZE})$、
オブジェクトが保持する空間も同じorderである。

## `size`

```cpp
int size() const
```

列長を返す。時間計算量と追加空間計算量は $O(1)$。

## `versions`

```cpp
int versions() const
```

存在するversion数を返す。時間計算量と追加空間計算量は $O(1)$。

## `latest_version`

```cpp
int latest_version() const
```

最後に作ったversion番号を返す。時間計算量と追加空間計算量は $O(1)$。

## `add`

```cpp
int add(int version, int position, const T& delta)
```

`version` の `position` 番目へ `delta` を加えた新しいversionを作り、その番号を
返す。指定したversionは変更しない。時間計算量は
$O(\log\mathtt{MAX\_SIZE})$、更新で保持する追加空間は
$O(\log\mathtt{MAX\_SIZE})$。

## `sum`

```cpp
T sum(int version, int left, int right) const
```

`version` の半開区間 `[left, right)` の和を返す。時間計算量は
$O(\log\mathtt{MAX\_SIZE})$、追加空間計算量は
$O(\log\mathtt{MAX\_SIZE})$。

## 注意点

- version 0が初期状態である。`0 <= version < versions()` のversionを指定する。
- `0 <= position < size()`、`0 <= left <= right <= size()` が必要である。
- version 0から追加できるversion数は `MAX_VERSION` 以下である。
- 不正な容量、version、添字、区間またはversion/node容量超過では
  `runtime_error` を送出する。
- `T` は0からの構築、copy、加算に対応し、すべての和を表現できる必要がある。
- オブジェクトのcopyとmoveはできない。
