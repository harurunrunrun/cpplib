---
title: Minimum Mixture Smoke (混合で生じる煙の最小量) [MIXTURES]
documentation_of: ../src/algorithm/other/minimum_mixture_smoke.hpp
---

隣接する色を順に混ぜて1個にするときの煙の最小総量を返す。

# minimum_mixture_smoke

```cpp
long long minimum_mixture_smoke(const vector<int>& colors);
```

色は0以上99以下で、色 `a`, `b` の混合は煙 `a*b` と色 `(a+b)%100` を生じる。

## API別の時間計算量・空間計算量

| API | 時間計算量 | 空間計算量 |
| --- | --- | --- |
| `minimum_mixture_smoke` | $O(N^3)$ | $O(N^2)$ |

## 注意点

- 空列と1要素列では0を返す。
- 色が範囲外なら `runtime_error` を送出する。
