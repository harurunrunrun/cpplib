---
title: Cat Feast Milk Feasibility (猫の宴の牛乳充足判定)
documentation_of: ../src/algorithm/graph/can_feed_all_cats.hpp
---

## CatFeastDistance

```cpp
struct CatFeastDistance {
    int first;
    int second;
    long long distance;
};
```

2匹の猫と、その間を移動すると失われる牛乳量を表す。

## can_feed_all_cats

```cpp
bool can_feed_all_cats(
    long long milk,
    int cat_count,
    const vector<CatFeastDistance>& distances
);
```

猫0の位置にある牛乳から出発し、各猫に1単位以上を残せるかを返す。

## 引数

- `milk`: 利用できる牛乳量。
- `cat_count`: 猫の数。
- `distances`: 猫同士を移動する際の牛乳消費量を表す無向辺。

## 戻り値

全猫へ給餌できるなら `true`、できないなら `false`。

## API別の時間計算量・空間計算量

猫の数を $N$、辺数を $M$ とする。

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `can_feed_all_cats` | $O(M\log(M+1))$ | $O(N+M)$ |

## 注意点

- `cat_count` は正、`milk` と各距離は非負、端点は $[0,N)$ でなければならない。
- 入力グラフが非連結、または前提に違反する場合は `runtime_error` を送出する。
- 必要量の加算は符号付き128 bitで行う。
