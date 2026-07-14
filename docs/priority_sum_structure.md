---
title: Priority Sum Structure (優先度付き和構造) [ABC281_E]
documentation_of: ../src/structure/other/priority_sum_structure.hpp
---

重複を許す multiset を保持し、現在の `k` 個の最小要素の和と最大要素の和を管理する。
部分木の要素数と和を持つ AVL tree を使い、任意の大きさへの `k` 変更も
`k` の差に依存せず $O(\log N)$ で処理する。和は更新時にキャッシュされるため、
取得は $O(1)$ である。

「最小」「最大」は `Compare` が定める順序を指す。デフォルトの
`std::less<T>` では通常の最小値・最大値になる。

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

- `T`: multiset に格納する値の型。
- `Sum`: 和の型。`Sum{}` が 0 で、コピーと `operator+=` が可能でなければならない。
- `Compare`: `T` に狭義弱順序を与える比較関数。
- `Projection`: `T` を加算する `Sum` に変換する関数オブジェクト。

デフォルトの `PrioritySumDefaultProjection<T, Sum>` は
`static_cast<Sum>(value)` を返す。例えば値は `int` のまま、和だけを `long long` にして
オーバーフローを避けられる。また、構造体を priority で並べ、別の cost を合計することもできる。

`Compare` 上で同値な値が異なる射影値を持つ場合、その同値要素群の途中に `k` の境界が
来たときにどの要素が選ばれるかは未規定である。結果を一意にしたい場合は、同値な値の
射影値を同じにするか、`Compare` に tie-break を含める。

## デフォルト構築

```cpp
explicit PrioritySumStructure(
    const Compare& compare = Compare(),
    const Projection& project = Projection()
)
```

空、かつ `k == 0` の構造を作る。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## vector からの構築

```cpp
explicit PrioritySumStructure(
    const std::vector<T>& values,
    std::size_t k,
    const Compare& compare = Compare(),
    const Projection& project = Projection()
)
```

`values` の全要素と指定した `k` で構築する。重複する値も個別に保持する。
`k > values.size()` なら `runtime_error` を送出する。

- 時間計算量: $O(N\log(N+1))$
- 追加空間計算量: $O(N)$

## `size`

```cpp
std::size_t size() const
```

重複を含む現在の要素数を返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `empty`

```cpp
bool empty() const
```

要素が 0 個かを返す。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `k`

```cpp
std::size_t k() const
```

現在選択している要素数を返す。常に `0 <= k() && k() <= size()` が成り立つ。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `contains`

```cpp
bool contains(const T& value) const
```

`Compare` 上で `value` と同値な要素が 1 個以上あるかを返す。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## `insert`

```cpp
void insert(const T& value)
```

`value` を 1 個追加する。すでに同値な値があっても新しい要素として追加する。
追加後の最小側・最大側の和も更新する。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: AVL node 1 個と再帰 stack $O(\log(N+1))$

## `erase`

```cpp
void erase(const T& value)
```

`Compare` 上で `value` と同値な要素を 1 個削除する。該当する要素がない場合は
`runtime_error` を送出する。

削除後にも `k <= size()` を保つ。したがって削除前に `k == size()` なら状態を変更せず
`runtime_error` を送出する。全要素を選択中に削除したい場合は、先に `set_k(k() - 1)`
を呼ぶ。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: 再帰 stack $O(\log(N+1))$

## `set_k`

```cpp
void set_k(std::size_t k)
```

選択数を `k` に変更し、新しい最小側・最大側の和を計算する。
変更前後の `k` の差に依存せず $O(\log N)$ である。`k > size()` なら状態を変更せず
`runtime_error` を送出する。

- 時間計算量: $O(\log(N+1))$
- 追加空間計算量: $O(1)$

## `smallest_sum`

```cpp
const Sum& smallest_sum() const
```

`Compare` 順で先頭 `k()` 個の射影値の和を返す。`k() == 0` なら `Sum{}`、
`k() == size()` なら全要素の和である。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## `largest_sum`

```cpp
const Sum& largest_sum() const
```

`Compare` 順で末尾 `k()` 個の射影値の和を返す。`k() == 0` なら `Sum{}`、
`k() == size()` なら全要素の和である。

- 時間計算量: $O(1)$
- 追加空間計算量: $O(1)$

## 空間計算量と object の性質

保持要素数を $N$ とすると object 全体は $O(N)$、AVL tree の高さは
$O(\log(N+1))$ である。重複要素もそれぞれ 1 node を使う。

動的 node を一意所有するため copy constructor と copy assignment は削除されている。
move constructor と move assignment は使用できる。

## 注意点

`0 <= n <= MAX_SIZE`、`0 <= k <= size()`。index・容量・`k` の違反では
`runtime_error`。`erase(index)` は現在存在するindexを要求する。比較関数はstrict weak
orderingを満たし、同値要素はindexで区別する。
保存領域は $O(\mathtt{MAX\_SIZE})$。constructor以外の各更新・queryの追加領域は
再帰stack $O(\log \mathtt{MAX\_SIZE})$、返却値以外は $O(1)$。
