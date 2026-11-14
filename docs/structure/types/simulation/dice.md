---
title: Dice Orientation (サイコロの姿勢)
documentation_of: ../../../../src/structure/types/simulation/dice.hpp
---

サイコロの 6 面のラベルと姿勢を管理する。面の順序は
`top, front, right, back, left, bottom` である。

## 型

```cpp
DiceFace
Dice<T>
```

`DiceFace` は `top`, `front`, `right`, `back`, `left`, `bottom` を持つ。
`Dice<T>` は各面に型 `T` のラベルを保持する。

## 構築と面の参照

```cpp
explicit Dice(array<T, 6> labels)
Dice(top, front, right, back, left, bottom)

const array<T, 6>& labels() const
const T& label(DiceFace face) const
const T& top() const
const T& front() const
const T& right() const
const T& back() const
const T& left() const
const T& bottom() const
```

`label` に列挙外の `DiceFace` を渡すと `std::out_of_range` を送出する。

## 回転

```cpp
Dice& roll_front()
Dice& roll_back()
Dice& roll_right()
Dice& roll_left()
Dice& spin_right()
Dice& spin_left()
```

`roll_*` は指定方向へ転がす。`spin_*` は上下の面を保ったまま、上から見て
指定方向へ 90 度回す。いずれも自身を更新して参照を返す。

## 姿勢の指定と列挙

```cpp
static constexpr size_t orientation_count = 24;
array<Dice, 24> orientations() const
Dice oriented(DiceFace top_face, DiceFace front_face) const
optional<Dice> oriented_by_labels(const T& top_label, const T& front_label) const
```

`orientations` は物理的な 24 姿勢をそれぞれ 1 回ずつ返す。ラベルが重複している場合、同じ
ラベル列が複数回現れることがある。

`oriented` は現在の各面を面番号で区別し、指定面を上・前に置いた姿勢を返す。
同じ面または対向面を指定すると `std::invalid_argument`、列挙外の面番号では
`std::out_of_range` を送出する。

`oriented_by_labels` は条件を満たす最初の姿勢を返す。該当する姿勢がなければ
`std::nullopt` を返す。ラベルが重複する場合も、全 24 姿勢の列挙順で決める。

## 回転同値判定

```cpp
bool rotationally_equivalent(const Dice& other) const
bool operator==(const Dice& left, const Dice& right)
```

`rotationally_equivalent` は一方を回転して他方と同じラベル配置にできるかを返す。
`operator==` は現在の姿勢を変えずに 6 面を比較する。

## API 別の時間計算量・空間計算量

`T` のコピー・ムーブ・比較を $O(1)$ とする。

| API | 時間計算量 | 追加領域 |
| --- | --- | --- |
| コンストラクタ、各面の参照 | $O(1)$ | $O(1)$ |
| 各 `roll_*`, `spin_*` | $O(1)$ | $O(1)$ |
| `oriented` | $O(1)$ | $O(1)$ |
| `oriented_by_labels` | $O(1)$ | $O(1)$ |
| `orientations` | $O(1)$ | 戻り値に $O(1)$ |
| `rotationally_equivalent` | $O(1)$ | $O(1)$ |
| `operator==` | $O(1)$ | $O(1)$ |

姿勢数 24 と面数 6 は定数である。

## 注意点

- `T` はコピー可能でなければならない。
- ラベルによる姿勢指定と同値判定には `T` の等値比較が必要である。
- 各面のラベルが互いに異なる必要はない。
