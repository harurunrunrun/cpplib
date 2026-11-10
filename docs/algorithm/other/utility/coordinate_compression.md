---
title: Coordinate Compression (座標圧縮)
documentation_of: ../../../../src/algorithm/other/utility/coordinate_compression.hpp
---

値を昇順に並べて重複を除き、元の大小関係を保つ $[0,K)$ の添字へ写す。

## テンプレート引数

```cpp
template<class T, int MAX_SIZE = -1>
struct CoordinateCompression;
```

`T` は `std::sort`、`std::lower_bound`、`operator==` を使用できる型とする。

`MAX_SIZE` を指定すると、圧縮後の座標を容量 `MAX_SIZE` の固定長配列に保持する。省略すると実行時サイズに合わせた `std::vector` を使う可変長版になる。

```cpp
CoordinateCompression<int> dynamic(values);
CoordinateCompression<int, 100000> fixed(values);
```

## 構築・`build`

```cpp
CoordinateCompression()
explicit CoordinateCompression(const std::vector<T>& values)
void build(const std::vector<T>& values)
```

デフォルト構築では空である。`build` または引数付きの構築は、`values` を昇順に並べて重複を除く。固定長版で `values.size() > MAX_SIZE` なら `runtime_error` を送出する。再度 `build` すると以前の座標を置き換える。

- 時間計算量: $O(N\log N)$
- 追加の空間計算量: $O(N)$

## `size` / `empty`

```cpp
std::size_t size() const
bool empty() const
```

相異なる座標の数と、それが $0$ かどうかを返す。

- 時間計算量: $O(1)$

## `values`

```cpp
std::span<const T> values() const
```

昇順の座標列を返す。返した `span` は次の `build` まで有効である。ただし `T = bool` の可変長版では `std::vector<bool>` のコピー（最大 2 要素）を返す。

- 時間計算量: $O(1)$
- 追加の空間計算量: $O(1)$

## `lower_bound_index` / `upper_bound_index`

```cpp
std::size_t lower_bound_index(const T& value) const
std::size_t upper_bound_index(const T& value) const
```

圧縮座標列に対する `lower_bound` / `upper_bound` の添字を返す。戻り値は `size()` と等しいことがある。

- 時間計算量: $O(\log(K+1))$
- 追加の空間計算量: $O(1)$

## `contains`

```cpp
bool contains(const T& value) const
```

`value` が圧縮座標列に含まれるかを返す。

- 時間計算量: $O(\log(K+1))$

## `index`

```cpp
std::size_t index(const T& value) const
```

`value` の圧縮後の添字を返す。登録されていなければ `runtime_error` を送出する。

- 時間計算量: $O(\log(K+1))$

## `value`

```cpp
const T& value(std::size_t index) const
```

圧縮後の添字から元の座標を返す。`index >= size()` なら `runtime_error` を送出する。`T = bool` の可変長版では参照ではなく値を返す。

- 時間計算量: $O(1)$

## `compress`

```cpp
std::vector<std::size_t> compress(const std::vector<T>& source) const
```

`source` の各値を圧縮後の添字へ変換する。1 つでも未登録の値があれば `runtime_error` を送出する。

- 時間計算量: $O(M\log(K+1))$（$M$ は `source.size()`）
- 追加の空間計算量: 戻り値の $O(M)$

## 注意点

- 固定長版では $0\le K\le\texttt{MAX_SIZE}$ であり、オブジェクトの保存領域は $O(\texttt{MAX_SIZE})$ である。`build` は入力の要素数に対して容量を検査するため、重複除去後に収まる場合でも `values.size() > MAX_SIZE` は許可しない。
- 可変長版の保存領域は $O(K)$ である。
