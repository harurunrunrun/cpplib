#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden

import argparse
import bisect
import random
from pathlib import Path

parser = argparse.ArgumentParser()
parser.add_argument('--out-dir', required=True)
out = Path(parser.parse_args().out_dir)
out.mkdir(parents=True, exist_ok=True)
rng = random.Random(2026071311)
n, q = 20000, 100000
initial = [rng.randrange(-10**15, 10**15) for _ in range(n)]
history = [[(0, value)] for value in initial]
current = initial.copy()
version = 0
commands = []
answers = []
for query in range(q):
    kind = rng.randrange(20)
    if kind < 12:
        index = (query * 11939 + rng.randrange(n)) % n
        value = rng.randrange(-10**18, 10**18)
        version += 1
        history[index].append((version, value))
        current[index] = value
        commands.append(f'SET {index} {value}')
        answers.append(str(version))
    elif kind < 19:
        index = rng.randrange(n)
        target = rng.randrange(version + 1)
        changes = history[index]
        pos = bisect.bisect_right(changes, (target, 10**30)) - 1
        commands.append(f'GET {target} {index}')
        answers.append(str(changes[pos][1]))
    else:
        index = rng.randrange(n)
        commands.append(f'CURRENT {index}')
        answers.append(str(current[index]))
(out / 'case_00.in').write_text(
    f'{n} {q}\n' + ' '.join(map(str, initial)) + '\n'
    + '\n'.join(commands) + '\n', encoding='utf-8')
(out / 'case_00.out').write_text(
    '\n'.join(answers) + '\n', encoding='utf-8')
