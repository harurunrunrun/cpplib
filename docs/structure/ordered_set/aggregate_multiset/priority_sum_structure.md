---
title: Priority Sum Structure (優先度付き和構造)
documentation_of: ../../../../src/structure/ordered_set/aggregate_multiset/priority_sum_structure.hpp
---

重複を許す多重集合を保持し、現在の $k$ 個の最小要素の和と最大要素の和を管理する。
部分木の要素数と和を持つ AVL 木を使い、任意の大きさへの $k$ の変更も、$k$ の差に依存せず $O(\log N)$ で処理する。和は更新時にキャッシュするため、取得は $O(1)$ である。

「最小」「最大」は `Compare` が定める順序を指す。デフォルトの `std::less<T>` では通常の最小値・最大値になる。

## テンプレート引数

```cpp
template<
    class T,
    class Sum = T,
    class Compare = std::less<T>,
    class Projection = PrioritySumDefaultProjection<T, Sum>
>
struct PrioritySumStructure;
```

- `T`: 多重集合に格納する値の型。
- `Sum`: 和の型。`Sum{}` が $0$ で、コピーと `operator+=` ができなければならない。
- `Compare`: `T` に狭義の弱順序を与える比較関数。
- `Projection`: `T` を加算する `Sum` に変換する関数オブジェクト。

デフォルトの `PrioritySumDefaultProjection<T, Sum>` は `static_cast<Sum>(value)` を返す。たとえば値は `int` のまま、和だけを `long long` にしてオーバーフローを避けられる。また、構造体を優先度で並べ、別のコストを合計することもできる。

`Compare` について同値な値が異なる射影値を持つ場合、その同値な要素群の途中に $k$ の境界が来たときにどの要素が選ばれるかは未規定である。結果を一意にしたい場合は、同値な値の射影値を同じにするか、`Compare` にタイブレークを含める。

## デフォルトコンストラクタ

```cpp
explicit PrioritySumStructure(
    const Compare& compare = Compare(),
    const Projection& project = Projection()
)
```

空で、かつ $k = 0$ の構造を作る。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## vector からのコンストラクタ

```cpp
explicit PrioritySumStructure(
    const std::vector<T>& values,
    std::size_t k,
    const Compare& compare = Compare(),
    const Projection& project = Projection()
)
```

`values` の全要素と指定した `k` で構築する。重複する値も個別に保持する。
`k > values.size()` なら `std::runtime_error` を送出する。

- 時間計算量: $O(N\log(N+1))$
- 追加空間計算量: $O(N)$

## size

```cpp
std::size_t size() const
```

重複を含む現在の要素数を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## empty

```cpp
bool empty() const
```

要素が $0$ 個かどうかを返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## k

```cpp
std::size_t k() const
```

現在選択している要素数を返す。常に $0 \le k() \le \mathrm{size}()$ が成り立つ。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## contains

```cpp
bool contains(const T& value) const
```

`Compare` について `value` と同値な要素が 1 個以上あるかを返す。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## insert

```cpp
void insert(const T& value)
```

`value` を 1 個追加する。既に同値な値があっても、新しい要素として追加する。追加後の最小側・最大側の和も更新する。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: AVL 木のノード 1 個と、再帰のスタック $O(\log(N+1))$

## erase

```cpp
void erase(const T& value)
```

`Compare` について `value` と同値な要素を 1 個削除する。該当する要素が無い場合は `std::runtime_error` を送出する。

削除後も $k \le \mathrm{size}()$ を保つ。したがって削除前に `k == size()` なら、状態を変更せずに `std::runtime_error` を送出する。全要素を選択中に削除したい場合は、先に `set_k(k() - 1)` を呼ぶ。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: 再帰のスタック $O(\log(N+1))$

## set_k

```cpp
void set_k(std::size_t k)
```

選択数を `k` に変更し、新しい最小側・最大側の和を計算する。変更前後の $k$ の差に依存せず $O(\log N)$ である。`k > size()` なら状態を変更せずに `std::runtime_error` を送出する。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## smallest_sum

```cpp
const Sum& smallest_sum() const
```

`Compare` の順で先頭 `k()` 個の射影値の和を返す。`k() == 0` なら `Sum{}`、`k() == size()` なら全要素の和である。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## largest_sum

```cpp
const Sum& largest_sum() const
```

`Compare` の順で末尾 `k()` 個の射影値の和を返す。`k() == 0` なら `Sum{}`、`k() == size()` なら全要素の和である。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 空間計算量

保持している要素数を $N$ とすると、オブジェクト全体は $O(N)$、AVL 木の高さは $O(\log(N+1))$ である。重複する要素もそれぞれ 1 ノードを使う。

動的なノードを一意に所有するため、コピーコンストラクタとコピー代入は削除されている。ムーブコンストラクタとムーブ代入は使える。

## 注意点

- 常に $0 \le k \le \mathrm{size}()$ を保つ。`k` が要素数を超える構築・`set_k`、`k == size()` の状態での `erase`、同値な要素が存在しない `erase` は、状態を変更せずに `std::runtime_error` を送出する。
- `Compare` は狭義の弱順序を満たす必要がある。同値な要素どうしは区別しない。
- `insert`・`erase` の追加領域は再帰のスタック $O(\log(N+1))$、それ以外の関数の追加領域は（戻り値を除き）$O(1)$ である。
