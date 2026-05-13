---
title: Integer Set
documentation_of: ../src/structure/tree/integer_set.hpp
---

非負整数限定で定数倍を削ったordered? set。

AVX512が使える環境じゃないとエラー。

TODO:

- kthをwrapperにする

# コンストラクタ

```cpp
Int_Set<class L, L MAX_SIZE>()
```

- `L`: 型、`unsigned int` or `unsigned long long`
- `MAX_SIZE`: 最大値 (閉区間なので実際の最大値+1して)

# insert

```cpp
void insert(const L& x)
```
`x` を挿入する。
存在する場合は何もしない。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$


# erase

```cpp
void erase(const L& x)
```

`x` を削除する。
存在しない場合は何もしない。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$


# contain

```cpp
bool contain(const L& x)
```

`x` が存在するか判定する。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$

# range_sum

```cpp
L range_sum(const L& l, const L& r)
```

区間 `[l,r)` に存在する要素数を返す。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$ (SIMD)

# least

```cpp
std::optional<L> least(const L& x)
```

`x <= y` を満たす最小の `y` を返す。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$

# more

```cpp
std::optional<L> more(const L& x)
```

`x < y` を満たす最小の `y` を返す。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$


# most

```cpp
std::optional<L> most(const L& x)
```

`x >= y` を満たす最小の `y` を返す。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$

# more

```cpp
std::optional<L> less(const L& x)
```

`x > y` を満たす最小の `y` を返す。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$


# empty
```cpp
bool empty()
```

空か否かを返す。

## 時間計算量
- $O(1)$

# max
```cpp
std::optional<L> max()
```

最大値を返す。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$


# min
```cpp
std::optional<L> min()
```

最小値を返す。

## 時間計算量
- $O(\log {{MAXSIZE} / 16})$


# clear
```cpp
void clear()
```

要素をすべて削除する。

## 時間計算量
- $O(|要素数|\log {{MAXSIZE} / 16})$ (だと思われる)


# list

```cpp
std::vector<L> list()
```

要素を昇順に列挙する。

## 時間計算量
- $O(|要素数|\log {{MAXSIZE} / 16})$ (だと思われる)


# kth_ge

```cpp
std::optional<L> kth_ge(const L& x, const L& y)
```

`x <= y` を満たす `y` のうち、昇順で `k` 番目の値を返す。

## 時間計算量
- $O(16\log {{MAXSIZE} / 16})$ (だと思われる)

# kth_gt

```cpp
std::optional<L> kth_gt(const L& x, const L& y)
```

`x < y` を満たす `y` のうち、昇順で `k` 番目の値を返す。

## 時間計算量
- $O(16\log {{MAXSIZE} / 16})$ (だと思われる)

# kth_le

```cpp
std::optional<L> kth_le(const L& x, const L& y)
```

`x >= y` を満たす `y` のうち、降順で `k` 番目の値を返す。

## 時間計算量
- $O(16\log {{MAXSIZE} / 16})$ (だと思われる)

# kth_gt

```cpp
std::optional<L> kth_lt(const L& x, const L& y)
```

`x > y` を満たす `y` のうち、降順で `k` 番目の値を返す。

## 時間計算量
- $O(16\log {{MAXSIZE} / 16})$ (だと思われる)


