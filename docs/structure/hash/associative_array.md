---
title: Associative Array (連想配列) [associative_array]
documentation_of: ../../../src/structure/hash/associative_array.hpp
---

Robin Hood法によるopen addressing hash table。bucket数は`MAX_SIZE`の2倍以上となる2冪で、領域は構築時に確保する。削除ではbackward shiftを行うためtombstoneを残さない。

## テンプレート引数

```cpp
AssociativeArray<Key, T, MAX_SIZE, Hash, KeyEqual>
```

- `Key`: key型
- `T`: value型
- `MAX_SIZE`: 保持できる要素数
- `Hash`: `std::size_t operator()(const Key&)`を持つhash関数
- `KeyEqual`: keyの等値判定

hash値にはSplitMix64によるmixを行う。

## 構築

```cpp
AssociativeArray(std::uint64_t seed = ..., Hash hash = {}, KeyEqual equal = {})
```

空のtableを構築する。`seed`を変えるとbucket配置が変わる。

## 時間計算量

- $O(\mathtt{capacity})$

## 検索

```cpp
T* find(const Key& key)
const T* find(const Key& key) const
bool contains(const Key& key) const
T& at(const Key& key)
const T& at(const Key& key) const
```

`find`はkeyが存在すればvalueへのpointer、存在しなければ`nullptr`を返す。`at`は存在しないkeyに対して`std::out_of_range`を送出する。

## 時間計算量

- expected $O(1)$
- worst $O(\mathtt{MAX\_SIZE})$

## 追加・更新

```cpp
bool insert_or_assign(Key key, T value)
T& operator[](const Key& key)
T& operator[](Key&& key)
```

`insert_or_assign`は新しくkeyを追加した場合に`true`、既存valueを更新した場合に`false`を返す。`operator[]`は存在しないkeyを`T()`で追加する。要素数が`MAX_SIZE`の状態で新しいkeyを追加すると例外を送出する。

## 時間計算量

- expected $O(1)$
- worst $O(\mathtt{MAX\_SIZE})$

## 削除

```cpp
bool erase(const Key& key)
void clear()
```

`erase`は削除できた場合に`true`を返す。`clear`は全要素を削除する。

## 時間計算量

- `erase`: expected $O(1)$、worst $O(\mathtt{MAX\_SIZE})$
- `clear`: $O(\mathtt{capacity})$

## 要素数

```cpp
int size() const
bool empty() const
static constexpr int max_size()
static constexpr std::size_t capacity()
```

## 時間計算量

- すべて $O(1)$

## 空間計算量

- $O(\mathtt{MAX\_SIZE})$

copyは無効で、moveに対応する。

## 注意点

nconstructor、clear、find、contains、at、insert_or_assign、operator[]、erase、size、empty、max_size、capacityはいずれも追加空間計算量 O(1)。新規要素のkey/valueは本体保存領域として別に数える。

`MAX_SIZE > 0` が必要。`Hash` と `KeyEqual` は、等しいkeyに同じhash値を与える必要がある。
存在しないkeyへの `at` は `out_of_range`、満杯のtableへの新規挿入は `runtime_error`。
`find` が返すpointerと `at` / `operator[]` が返す参照は、erase、clear、moveまたは破棄後に使えない。
構築・`clear` 以外の各APIの追加領域は $O(1)$。挿入したkey・valueを含む保存領域は
$O(\mathtt{MAX\_SIZE})$。

## 容量を省略した可変長版

容量テンプレートを省略すると既定値 `-1` でvectorを使い、必要な保存領域を明示的に伸長する。
固定容量を指定する既存版も同じヘッダに残る。

```cpp
using Variable = AssociativeArray<Key, T>;
using Fixed = AssociativeArray<Key, T, N>;
```

要素数 $N$ に対し領域 $O(N)$。負荷率が半分を超える前に表を倍増して再hashするため、良好なhashで検索・挿入・削除は期待償却 $O(1)$、一回の再hashは $O(N)$。
可変長版の `capacity()` は現在の表のスロット数を返す。再hashは既存の要素参照を無効化する。
整数添字の表現上限を超える要素数は例外。構造上の文字種類・bit幅・座標域は、件数容量とは別に指定できる。
