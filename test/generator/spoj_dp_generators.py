#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

from __future__ import annotations

import argparse
import functools
import itertools
import math
import random
from pathlib import Path


def write(out: Path, name: str, data: str, answer: str) -> None:
    (out / f"{name}.in").write_text(data, encoding="ascii")
    (out / f"{name}.out").write_text(answer, encoding="ascii")


def acquire(out: Path) -> None:
    def solve(rectangles: list[tuple[int, int]]) -> int:
        n = len(rectangles)
        group_cost = [0] * (1 << n)
        for mask in range(1, 1 << n):
            widths = [rectangles[i][0] for i in range(n) if mask >> i & 1]
            heights = [rectangles[i][1] for i in range(n) if mask >> i & 1]
            group_cost[mask] = max(widths) * max(heights)
        dp = [10**30] * (1 << n)
        dp[0] = 0
        for mask in range(1, 1 << n):
            first = mask & -mask
            subset = mask
            while subset:
                if subset & first:
                    dp[mask] = min(dp[mask], dp[mask ^ subset] + group_cost[subset])
                subset = (subset - 1) & mask
        return dp[-1]

    cases = [
        ("official", [(100, 1), (15, 15), (20, 5), (1, 100)]),
        ("dominated", [(2, 2), (2, 5), (4, 3), (7, 1)]),
    ]
    source = random.Random(151)
    cases += [
        (f"random_{index:02d}", [(source.randint(1, 30), source.randint(1, 30)) for _ in range(source.randint(1, 7))])
        for index in range(20)
    ]
    for name, rectangles in cases:
        write(out, name, str(len(rectangles)) + "\n" + "".join(f"{w} {h}\n" for w, h in rectangles), f"{solve(rectangles)}\n")
    maximum = [(1_000_000, 1_000_000)] * 50_000
    write(out, "maximum", "50000\n" + "1000000 1000000\n" * 50_000, "1000000000000\n")


def apio10a(out: Path) -> None:
    def solve(values: list[int], a: int, b: int, c: int) -> int:
        prefix = [0]
        for value in values:
            prefix.append(prefix[-1] + value)
        dp = [0] + [-10**40] * len(values)
        for end in range(1, len(values) + 1):
            dp[end] = max(dp[start] + a * (prefix[end] - prefix[start]) ** 2 + b * (prefix[end] - prefix[start]) + c for start in range(end))
        return dp[-1]

    source = random.Random(152)
    tests: list[tuple[list[int], int, int, int]] = [([2, 2, 3, 4], -1, 10, -20)]
    for _ in range(40):
        tests.append(([source.randint(1, 9) for _ in range(source.randint(1, 12))], -source.randint(1, 5), source.randint(-20, 20), source.randint(-20, 20)))
    data = [str(len(tests))]
    answers = []
    for values, a, b, c in tests:
        data += [str(len(values)), f"{a} {b} {c}", " ".join(map(str, values))]
        answers.append(str(solve(values, a, b, c)))
    write(out, "random", "\n".join(data) + "\n", "\n".join(answers) + "\n")
    n = 1_000_000
    write(out, "maximum", f"1\n{n}\n-1 0 0\n" + "1 " * n + "\n", f"{-n}\n")


def nkleaves(out: Path) -> None:
    def solve(weights: list[int], groups: int) -> int:
        n = len(weights)
        cost = [[0] * n for _ in range(n)]
        for left in range(n):
            for right in range(left + 1, n):
                cost[left][right] = cost[left][right - 1] + weights[right] * (right - left)
        previous = [0] + [10**30] * n
        for group in range(1, groups + 1):
            current = [10**30] * (n + 1)
            for end in range(group, n + 1):
                current[end] = min(previous[split] + cost[split][end - 1] for split in range(group - 1, end))
            previous = current
        return previous[n]

    source = random.Random(153)
    cases = [([1, 2, 3, 4, 5], 2)]
    for _ in range(25):
        n = source.randint(2, 13)
        weights = [source.randint(1, 20) for _ in range(n)]
        cases.append((weights, source.randint(1, n)))
    for index, (weights, groups) in enumerate(cases):
        name = "official" if index == 0 else f"random_{index:02d}"
        write(out, name, f"{len(weights)} {groups}\n" + "\n".join(map(str, weights)) + "\n", f"{solve(weights, groups)}\n")
    n, groups = 100_000, 10
    write(out, "maximum", f"{n} {groups}\n" + "1\n" * n, "499950000\n")


def brkstrng(out: Path) -> None:
    def solve(length: int, cuts: list[int]) -> int:
        p = [0] + cuts + [length]
        n = len(p)
        dp = [[0] * n for _ in range(n)]
        for span in range(2, n):
            for left in range(n - span):
                right = left + span
                dp[left][right] = p[right] - p[left] + min(
                    dp[left][cut] + dp[cut][right]
                    for cut in range(left + 1, right)
                )
        return dp[0][-1]

    def balanced_unit_break_cost(segment_count: int) -> int:
        height = (segment_count - 1).bit_length()
        return segment_count * height - ((1 << height) - segment_count)

    assert solve(10_000, list(range(1, 33))) == (
        10_000 + balanced_unit_break_cost(32)
    )

    source = random.Random(154)
    cases = [(10, []), (20, [3, 8, 10]), (20, [2, 3, 8, 10])]
    for _ in range(20):
        length = source.randint(3, 80)
        count = source.randint(1, min(9, length - 1))
        cases.append((length, sorted(source.sample(range(1, length), count))))
    data = "".join(f"{length} {len(cuts)}\n{' '.join(map(str, cuts))}\n" for length, cuts in cases)
    write(out, "random", data, "".join(f"{solve(length, cuts)}\n" for length, cuts in cases))
    cuts = list(range(1, 1001))
    maximum_answer = 10_000_000 + balanced_unit_break_cost(1000)
    write(out, "maximum", "10000000 1000\n" + " ".join(map(str, cuts)) + "\n", f"{maximum_answer}\n")


def mixtures(out: Path) -> None:
    def solve(colors: list[int]) -> int:
        n = len(colors)
        prefix = [0]
        for color in colors: prefix.append((prefix[-1] + color) % 100)
        dp = [[0] * n for _ in range(n)]
        for span in range(2, n + 1):
            for left in range(n - span + 1):
                right = left + span - 1
                dp[left][right] = min(dp[left][split] + dp[split + 1][right] + ((prefix[split + 1] - prefix[left]) % 100) * ((prefix[right + 1] - prefix[split + 1]) % 100) for split in range(left, right))
        return dp[0][-1]

    source = random.Random(155)
    cases = [[18, 19], [40, 60, 20]] + [[source.randrange(100) for _ in range(source.randint(1, 9))] for _ in range(30)] + [[0] * 100]
    write(out, "all", "".join(f"{len(case)}\n{' '.join(map(str, case))}\n" for case in cases), "".join(f"{solve(case)}\n" for case in cases))


def trt(out: Path) -> None:
    def solve(values: list[int]) -> int:
        @functools.lru_cache(None)
        def dp(left: int, right: int) -> int:
            if left > right: return 0
            day = len(values) - (right - left)
            return max(values[left] * day + dp(left + 1, right), values[right] * day + dp(left, right - 1))
        return dp(0, len(values) - 1)
    source = random.Random(156)
    cases = [[1, 3, 1, 5, 2]] + [[source.randint(1, 30) for _ in range(source.randint(1, 11))] for _ in range(20)]
    for index, values in enumerate(cases):
        write(out, "official" if index == 0 else f"random_{index:02d}", str(len(values)) + "\n" + "\n".join(map(str, values)) + "\n", f"{solve(values)}\n")
    n = 2000
    write(out, "maximum", f"{n}\n" + "1000\n" * n, f"{1000*n*(n+1)//2}\n")


def rent(out: Path) -> None:
    def solve(offers: list[tuple[int, int, int]]) -> int:
        best = 0
        for mask in range(1 << len(offers)):
            selected = sorted((offers[i] for i in range(len(offers)) if mask >> i & 1), key=lambda x: x[0])
            if all(selected[i][0] + selected[i][1] <= selected[i + 1][0] for i in range(len(selected) - 1)):
                best = max(best, sum(x[2] for x in selected))
        return best
    source = random.Random(157)
    cases = []
    for _ in range(30):
        cases.append([(source.randint(0, 20), source.randint(1, 8), source.randint(0, 50)) for _ in range(source.randint(1, 9))])
    data = [str(len(cases))]
    answers = []
    for offers in cases:
        data.append(str(len(offers))); data.extend(f"{a} {b} {c}" for a, b, c in offers); answers.append(str(solve(offers)))
    write(out, "random", "\n".join(data) + "\n", "\n".join(answers) + "\n")
    n = 10_000
    write(out, "maximum", f"1\n{n}\n" + "".join(f"{i} 1 1000000\n" for i in range(n)), "10000000000\n")


def aba12c(out: Path) -> None:
    def solve(limit: int, prices: list[int]) -> int:
        target = len(prices); dp = {(0, 0): 0}; answer = 10**9
        for count in range(1, limit + 1):
            for total in range(1, target + 1):
                values = [dp[(count - 1, total - w)] + prices[w - 1] for w in range(1, total + 1) if prices[w - 1] >= 0 and (count - 1, total - w) in dp]
                if values: dp[(count, total)] = min(values)
            answer = min(answer, dp.get((count, target), 10**9))
        return -1 if answer == 10**9 else answer
    source = random.Random(158)
    cases = [(3, [-1, -1, 4, 5, -1]), (5, [1, 2, 3, 4, 5])]
    for _ in range(40):
        k = source.randint(1, 12); cases.append((source.randint(1, 12), [(-1 if source.random() < .3 else source.randint(1, 30)) for _ in range(k)]))
    data = [str(len(cases))]; answers = []
    for limit, prices in cases:
        data += [f"{limit} {len(prices)}", " ".join(map(str, prices))]; answers.append(str(solve(limit, prices)))
    write(out, "all", "\n".join(data) + "\n", "\n".join(answers) + "\n")


def bytesm2(out: Path) -> None:
    def solve(grid: list[list[int]]) -> int:
        dp = grid[0][:]
        for row in grid[1:]: dp = [row[c] + max(dp[max(0, c - 1):min(len(dp), c + 2)]) for c in range(len(dp))]
        return max(dp)
    source = random.Random(159)
    cases = [[[3,1,7,4,2],[2,1,3,1,1],[1,2,2,1,8],[2,2,1,5,3],[2,1,4,4,4],[5,2,7,5,1]]]
    for _ in range(25):
        height = source.randint(1, 8)
        width = source.randint(1, 8)
        cases.append([[source.randint(0, 100) for _ in range(width)] for _ in range(height)])
    cases.append([[100] * 100 for _ in range(100)])
    data = [str(len(cases))]; answers = []
    for grid in cases:
        data.append(f"{len(grid)} {len(grid[0])}"); data.extend(" ".join(map(str, row)) for row in grid); answers.append(str(solve(grid)))
    write(out, "all", "\n".join(data) + "\n", "\n".join(answers) + "\n")


def samer08f(out: Path) -> None:
    values = [1, 2, 8, 100] + list(range(3, 30))
    write(out, "all", "\n".join(map(str, values + [0])) + "\n", "\n".join(str(n * (n + 1) * (2 * n + 1) // 6) for n in values) + "\n")


def anarc05h(out: Path) -> None:
    def solve(text: str) -> int:
        @functools.lru_cache(None)
        def dp(position: int, minimum: int) -> int:
            if position == len(text): return 1
            total = result = 0
            for end in range(position, len(text)):
                total += int(text[end])
                if total >= minimum: result += dp(end + 1, total)
            return result
        return dp(0, 0)
    source = random.Random(161)
    cases = ["1119", "1234567890", "0", "000000", "9" * 25] + ["".join(str(source.randrange(10)) for _ in range(source.randint(1, 18))) for _ in range(30)]
    write(out, "all", "\n".join(cases + ["bye"]) + "\n", "".join(f"{i}. {solve(text)}\n" for i, text in enumerate(cases, 1)))


def tilings(height: int, width: int) -> int:
    dp = {0: 1}
    for _ in range(width):
        nxt: dict[int, int] = {}
        for mask, ways in dp.items():
            def fill(row: int, occupied: int, outgoing: int) -> None:
                while row < height and occupied >> row & 1: row += 1
                if row == height:
                    nxt[outgoing] = nxt.get(outgoing, 0) + ways; return
                if row + 1 < height and not (occupied >> (row + 1) & 1): fill(row + 2, occupied | 3 << row, outgoing)
                fill(row + 1, occupied | 1 << row, outgoing | 1 << row)
            fill(0, mask, 0)
        dp = nxt
    return dp.get(0, 0)


def m3tile(out: Path) -> None:
    values = [0, 1, 2, 3, 8, 12, 30]
    write(out, "all", "\n".join(map(str, values + [-1])) + "\n", "\n".join(str(tilings(3, n)) for n in values) + "\n")


def m4tile(out: Path) -> None:
    values = [0, 1, 2, 3, 7, 10]
    write(out, "all", str(len(values)) + "\n" + "\n".join(map(str, values)) + "\n", "".join(f"{i} {tilings(4, n)}\n" for i, n in enumerate(values, 1)))


def party(out: Path) -> None:
    def solve(budget: int, parties: list[tuple[int, int]]) -> tuple[int, int]:
        best = (-1, 0)
        for mask in range(1 << len(parties)):
            fee = sum(parties[i][0] for i in range(len(parties)) if mask >> i & 1)
            fun = sum(parties[i][1] for i in range(len(parties)) if mask >> i & 1)
            if fee <= budget and (fun > best[0] or fun == best[0] and fee < best[1]): best = (fun, fee)
        return best[1], best[0]
    source = random.Random(164)
    cases = []
    for _ in range(25):
        cases.append((source.randint(0, 50), [(source.randint(1, 20), source.randint(0, 10)) for _ in range(source.randint(0, 10))]))
    data = []; answers = []
    for budget, parties in cases:
        data.append(f"{budget} {len(parties)}"); data.extend(f"{fee} {fun}" for fee, fun in parties); answers.append("%d %d" % solve(budget, parties))
    data.append("500 100"); data.extend(["5 10"] * 100); answers.append("500 1000"); data.append("0 0")
    write(out, "all", "\n".join(data) + "\n", "\n".join(answers) + "\n")


def mpilot(out: Path) -> None:
    def solve(pilots: list[tuple[int, int]]) -> int:
        inf = 10**30; dp = {(0, 0): 0}
        for index, (captain, assistant) in enumerate(pilots):
            nxt = {}
            for (_, balance), value in dp.items():
                nxt[(index + 1, balance + 1)] = min(nxt.get((index + 1, balance + 1), inf), value + assistant)
                if balance: nxt[(index + 1, balance - 1)] = min(nxt.get((index + 1, balance - 1), inf), value + captain)
            dp = nxt
        return dp[(len(pilots), 0)]
    source = random.Random(165)
    cases = [[(5000,3000),(6000,2000),(8000,1000),(9000,6000)]]
    cases += [[(source.randint(1, 100), source.randint(1, 100)) for _ in range(2 * source.randint(1, 7))] for _ in range(20)]
    for index, pilots in enumerate(cases):
        write(out, "official" if index == 0 else f"random_{index:02d}", str(len(pilots)) + "\n" + "".join(f"{a} {b}\n" for a, b in pilots), f"{solve(pilots)}\n")
    n = 10_000
    write(out, "maximum", f"{n}\n" + "10000 1\n" * n, "50005000\n")


def scubadiv(out: Path) -> None:
    def solve(ro: int, rn: int, cylinders: list[tuple[int, int, int]]) -> int:
        dp = {(0, 0): 0}
        for oxygen, nitrogen, weight in cylinders:
            nxt = dict(dp)
            for (o, n), value in dp.items():
                key = (min(ro, o + oxygen), min(rn, n + nitrogen))
                nxt[key] = min(nxt.get(key, 10**9), value + weight)
            dp = nxt
        return dp.get((ro, rn), -1)
    source = random.Random(166)
    cases = [(5,60,[(3,36,120),(10,25,129),(5,50,250),(1,45,130),(4,20,119)])]
    for _ in range(25):
        ro,rn=source.randint(1,8),source.randint(1,10); cylinders=[(source.randint(1,8),source.randint(1,10),source.randint(1,30)) for _ in range(source.randint(1,10))]; cylinders.append((ro,rn,source.randint(1,30))); cases.append((ro,rn,cylinders))
    cases.append((21,79,[(21,79,800)]+[(1,1,800)]*999))
    data=[str(len(cases))]; answers=[]
    for ro,rn,cylinders in cases:
        data += [f"{ro} {rn}",str(len(cylinders))]; data.extend(f"{o} {n} {w}" for o,n,w in cylinders); answers.append(str(solve(ro,rn,cylinders)))
    write(out,"all","\n".join(data)+"\n","\n".join(answers)+"\n")


def ioipalin(out: Path) -> None:
    def solve(text: str) -> int:
        dp = [0] * len(text)
        for left in range(len(text) - 2, -1, -1):
            diagonal = 0
            for right in range(left + 1, len(text)):
                old = dp[right]
                dp[right] = diagonal if text[left] == text[right] else 1 + min(dp[right], dp[right - 1])
                diagonal = old
        return dp[-1] if dp else 0
    source=random.Random(167); cases=["a","racecar","abc","google"]+["".join(source.choice("abcd") for _ in range(source.randint(1,30))) for _ in range(20)]+["a"*5000]
    for index,text in enumerate(cases): write(out,f"case_{index:02d}",f"{len(text)}\n{text}\n",f"{solve(text)}\n")


def bat3(out: Path) -> None:
    def solve(values: list[int], robin: int) -> int:
        dp=[1]*len(values)
        for i in range(len(values)):
            for j in range(i):
                if values[i] < values[j] or j == robin: dp[i]=max(dp[i],dp[j]+1)
        return max(dp)
    source=random.Random(168); cases=[]
    for _ in range(50):
        n=source.randint(1,20); cases.append(([source.randint(1,50) for _ in range(n)],source.randrange(n)))
    cases.append((list(range(2000,0,-1)),1000))
    data=[str(len(cases))]; answers=[]
    for values,robin in cases: data += [f"{len(values)} {robin}"," ".join(map(str,values))]; answers.append(str(solve(values,robin)))
    write(out,"all","\n".join(data)+"\n","\n".join(answers)+"\n")


def hist2(out: Path) -> None:
    def solve(values: list[int]) -> tuple[int,int]:
        n=len(values); dp={(1<<i,i):(values[i],1) for i in range(n)}
        for mask in range(1,1<<n):
            for last in range(n):
                if (mask,last) not in dp: continue
                value,count=dp[(mask,last)]
                for nxt in range(n):
                    if mask>>nxt&1: continue
                    key=(mask|1<<nxt,nxt); candidate=value+abs(values[last]-values[nxt]); old=dp.get(key,(-1,0))
                    dp[key]=(candidate,count) if candidate>old[0] else (old[0],old[1]+count) if candidate==old[0] else old
        best=-1; count=0
        for last in range(n):
            value,ways=dp[((1<<n)-1,last)]; value+=values[last]+2*n
            if value>best: best,count=value,ways
            elif value==best: count+=ways
        return best,count
    source=random.Random(169); cases=[[7],[1,2,3,4],[2,6,5]]
    cases += [source.sample(range(1,60),source.randint(2,10)) for _ in range(18)]; cases.append(list(range(1,16)))
    data=[]; answers=[]
    for values in cases: data += [str(len(values))," ".join(map(str,values))]; answers.append("%d %d"%solve(values))
    data.append("0"); write(out,"all","\n".join(data)+"\n","\n".join(answers)+"\n")


def edist(out: Path) -> None:
    def solve(a: str,b: str)->int:
        dp=list(range(len(b)+1))
        for i,x in enumerate(a,1):
            nxt=[i]+[0]*len(b)
            for j,y in enumerate(b,1): nxt[j]=min(dp[j]+1,nxt[j-1]+1,dp[j-1]+(x!=y))
            dp=nxt
        return dp[-1]
    source=random.Random(170); cases=[("FOOD","MONEY"),("A","A"),("A","")]
    cases += [("".join(source.choice("ABCD") for _ in range(source.randint(1,30))),"".join(source.choice("ABCD") for _ in range(source.randint(1,30)))) for _ in range(30)]
    # Online token format cannot represent empty strings; retain only nonempty cases.
    cases=[case for case in cases if all(case)] + [("A"*2000,"B"*2000)]
    data=[str(len(cases))]; data.extend(x for case in cases for x in case)
    write(out,"all","\n".join(data)+"\n","\n".join(str(solve(*case)) for case in cases)+"\n")


def dsubseq(out: Path) -> None:
    def solve(text: str)->int:
        total=1; last={}
        for ch in text: before=total; total=(2*total-last.get(ch,0))%1_000_000_007; last[ch]=before
        return total
    source=random.Random(171); cases=["AAA","ABCDEFG","CODECRAFT"]+["".join(source.choice("ABCDE") for _ in range(source.randint(1,100))) for _ in range(40)]+["A"*100_000]
    write(out,"all",str(len(cases))+"\n"+"\n".join(cases)+"\n","\n".join(str(solve(x)) for x in cases)+"\n")


def lisa(out: Path) -> None:
    def solve(expression: str)->tuple[int,int]:
        numbers=list(map(int,expression[::2])); operations=expression[1::2]; n=len(numbers)
        low=[[0]*n for _ in range(n)]; high=[[0]*n for _ in range(n)]
        for i,x in enumerate(numbers): low[i][i]=high[i][i]=x
        for span in range(2,n+1):
            for left in range(n-span+1):
                right=left+span-1; lows=[]; highs=[]
                for split in range(left,right):
                    op=operations[split]; f=(lambda a,b:a+b) if op=='+' else (lambda a,b:a*b)
                    lows.append(f(low[left][split],low[split+1][right])); highs.append(f(high[left][split],high[split+1][right]))
                low[left][right]=min(lows); high[left][right]=max(highs)
        return high[0][-1],low[0][-1]
    source=random.Random(172); cases=["1+2*3+4*5","2*0*3+7+1*0*3"]
    for _ in range(40):
        n=source.randint(1,9); cases.append("".join(str(source.randrange(10))+(source.choice("+*") if i+1<n else "") for i in range(n)))
    write(out,"all",str(len(cases))+"\n"+"\n".join(cases)+"\n","\n".join(f"{a} {b}" for a,b in map(solve,cases))+"\n")


def fisher(out: Path) -> None:
    def solve(times:list[list[int]],tolls:list[list[int]],limit:int)->tuple[int,int]:
        n=len(times); inf=(10**9,10**9); dp=[[inf]*n for _ in range(limit+1)]
        for remaining in range(limit+1):
            dp[remaining][n-1]=(0,0)
            for city in range(n-2,-1,-1):
                choices=[]
                for nxt in range(n):
                    if nxt!=city and times[city][nxt]<=remaining and dp[remaining-times[city][nxt]][nxt]!=inf:
                        toll,used=dp[remaining-times[city][nxt]][nxt]; choices.append((toll+tolls[city][nxt],used+times[city][nxt]))
                if choices: dp[remaining][city]=min(choices)
        return dp[limit][0]
    source=random.Random(173)
    cases=[([[0,3],[3,0]],[[0,5],[5,0]],3)]
    for _ in range(20):
        n=source.randint(3,6); limit=source.randint(3,15); times=[[0 if i==j else source.randint(1,8) for j in range(n)] for i in range(n)]; tolls=[[0 if i==j else source.randint(0,20) for j in range(n)] for i in range(n)]; times[0][-1]=min(times[0][-1],limit); cases.append((times,tolls,limit))
    data=[]; answers=[]
    for times,tolls,limit in cases:
        data.append(f"{len(times)} {limit}"); data.extend(" ".join(map(str,row)) for row in times); data.extend(" ".join(map(str,row)) for row in tolls); answers.append("%d %d"%solve(times,tolls,limit))
    data.append("0 0"); write(out,"all","\n".join(data)+"\n","\n".join(answers)+"\n")


def assign(out: Path) -> None:
    def solve(matrix:list[list[int]])->int:
        dp=[0]*(1<<len(matrix)); dp[0]=1
        for mask in range(len(dp)):
            student=mask.bit_count()
            if student==len(matrix): continue
            for topic,liked in enumerate(matrix[student]):
                if liked and not mask>>topic&1: dp[mask|1<<topic]+=dp[mask]
        return dp[-1]
    source=random.Random(174); cases=[]
    for _ in range(30):
        n=source.randint(1,9); cases.append([[source.randrange(2) for _ in range(n)] for _ in range(n)])
    cases += [[[int(i==j) for j in range(20)] for i in range(20)],[[1]*20 for _ in range(20)]]
    data=[str(len(cases))]; answers=[]
    for matrix in cases: data.append(str(len(matrix))); data.extend(" ".join(map(str,row)) for row in matrix); answers.append(str(solve(matrix)))
    write(out,"all","\n".join(data)+"\n","\n".join(answers)+"\n")


def lsort(out: Path) -> None:
    def solve(permutation:list[int])->int:
        n=len(permutation); positions=[0]*(n+1)
        for i,x in enumerate(permutation): positions[x]=i
        prefix=[[0]*(n+1) for _ in range(n+1)]
        for value in range(1,n+1):
            row=prefix[value]
            previous=prefix[value-1]
            position=positions[value]
            for index in range(n + 1):
                row[index]=previous[index]+(position < index)
        inf=10**30; dp=[[inf]*(n+2) for _ in range(n+2)]
        for x in range(1,n+1): dp[x][x]=positions[x]+1
        for length in range(2,n+1):
            for left in range(1,n-length+2):
                right=left+length-1
                left_position=positions[left]
                right_position=positions[right]
                larger_before=prefix[right][left_position]-prefix[left][left_position]
                smaller_before=prefix[right-1][right_position]-prefix[left-1][right_position]
                rank_left=left_position+1-larger_before
                rank_right=right_position+1-smaller_before
                dp[left][right]=min(dp[left+1][right]+rank_left*length,dp[left][right-1]+rank_right*length)
        return dp[1][n]
    source=random.Random(175); cases=[]
    for _ in range(40):
        p=list(range(1,source.randint(1,10)+1)); source.shuffle(p); cases.append(p)
    cases += [list(range(1,1001)),list(range(1000,0,-1))]
    data=[str(len(cases))]; answers=[]
    for p in cases: data += [str(len(p))," ".join(map(str,p))]; answers.append(str(solve(p)))
    write(out,"all","\n".join(data)+"\n","\n".join(answers)+"\n")


GENERATORS = {
    "acquire": acquire, "apio10a": apio10a, "nkleaves": nkleaves,
    "brkstrng": brkstrng, "mixtures": mixtures, "trt": trt, "rent": rent,
    "aba12c": aba12c, "bytesm2": bytesm2, "samer08f": samer08f,
    "anarc05h": anarc05h, "m3tile": m3tile, "m4tile": m4tile,
    "party": party, "mpilot": mpilot, "scubadiv": scubadiv,
    "ioipalin": ioipalin, "bat3": bat3, "hist2": hist2, "edist": edist,
    "dsubseq": dsubseq, "lisa": lisa, "fisher": fisher, "assign": assign,
    "lsort": lsort,
}


def main(name: str) -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--out-dir", required=True, type=Path)
    args = parser.parse_args()
    args.out_dir.mkdir(parents=True, exist_ok=True)
    GENERATORS[name](args.out_dir)
