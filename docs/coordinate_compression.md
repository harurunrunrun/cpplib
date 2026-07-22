---
title: Coordinate Compression (座標圧縮)
documentation_of: ../src/algorithm/other/utility/coordinate_compression.hpp
---

値を昇順に重複除去し、元の大小関係を保つ $[0,K)$ の添字へ写す。
圧縮後の座標は容量 `MAX_SIZE` の `std::array` に保持する。

## テンプレート引数

```cpp
template<class T, int MAX_SIZE>
struct CoordinateCompression;
```

`T` は `std::sort`、`std::lower_bound`、`operator==` を使用できる型とする。

## 構築・`build`

```cpp
CoordinateCompression()
explicit CoordinateCompression(const std::vector<T>& values)
void build(const std::vector<T>& values)
```

デフォルト構築では空である。`build` または引数付き構築は `values` を昇順に
重複除去する。`values.size() > MAX_SIZE` なら `runtime_error`。再度 `build` すると
以前の座標を置き換える。

- 時間計算量: $O(N\log N)$
- 追加空間計算量: $O(N)$

## `size` / `empty`

```cpp
std::size_t size() const
bool empty() const
```

異なる座標数と、それが 0 かを返す。

- 時間計算量: $O(1)$

## `values`

```cpp
std::span<const T> values() const
```

昇順の座標列を返す。返した `span` は次の `build` まで有効である。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `lower_bound_index` / `upper_bound_index`

```cpp
std::size_t lower_bound_index(const T& value) const
std::size_t upper_bound_index(const T& value) const
```

圧縮座標列に対する `lower_bound` / `upper_bound` の添字を返す。戻り値は
`size()` と等しいことがある。

- 時間計算量: $O(\log(K+1))$
- 追加空間計算量: $O(1)$

## `contains`

```cpp
bool contains(const T& value) const
```

`value` が圧縮座標列にあるかを返す。

- 時間計算量: $O(\log(K+1))$

## `index`

```cpp
std::size_t index(const T& value) const
```

`value` の圧縮後添字を返す。登録されていなければ `runtime_error`。

- 時間計算量: $O(\log(K+1))$

## `value`

```cpp
const T& value(std::size_t index) const
```

圧縮後添字から元の座標を返す。`index >= size()` なら `runtime_error`。

- 時間計算量: $O(1)$

## `compress`

```cpp
std::vector<std::size_t> compress(const std::vector<T>& source) const
```

`source` の各値を圧縮後添字へ変換する。一つでも未登録の値があれば
`runtime_error`。

- 時間計算量: $O(M\log(K+1))$
- 追加空間計算量: 戻り値 $O(M)$

## 注意点

$0\le K\le MAX\_SIZE$。object の保存領域は $O(MAX\_SIZE)$。`build` は入力の
要素数に対して容量を検査するため、重複除去後に収まる場合でも
`values.size() > MAX_SIZE` は許可しない。

