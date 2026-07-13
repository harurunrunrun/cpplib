#!/usr/bin/env python3
# competitive-verifier: DISPLAY hidden
from __future__ import annotations
import argparse
import random
from pathlib import Path

def metric(points):
    return [[sum(abs(a-b) for a,b in zip(x,y)) for y in points] for x in points]

def matrix_text(matrix):
    return "".join(" ".join(map(str,row))+"\n" for row in matrix)

def write(directory,index,data,output):
    (directory/f"case_{index:02d}.in").write_text(data,encoding="utf-8")
    (directory/f"case_{index:02d}.out").write_text(output,encoding="utf-8")

def gonzalez(distance,k,first):
    n=len(distance); centers=[first]; selected={first}
    nearest=[distance[p][first] for p in range(n)]
    while len(centers)<k:
        following=min((p for p in range(n) if p not in selected),
                      key=lambda p:(-nearest[p],p))
        centers.append(following); selected.add(following)
        nearest=[min(nearest[p],distance[p][following]) for p in range(n)]
    labels=[min(range(k),key=lambda q:(distance[p][centers[q]],centers[q]))
            for p in range(n)]
    radius=max(distance[p][centers[labels[p]]] for p in range(n))
    return centers,labels,radius

def hierarchy(distance,linkage):
    n=len(distance)
    distances={(a,b):float(distance[a][b]) for a in range(n) for b in range(a+1,n)}
    active=set(range(n)); sizes={p:1 for p in range(n)}; merges=[]
    for step in range(n-1):
        left,right=min(((a,b) for a in active for b in active if a<b),
                       key=lambda pair:(distances[pair],pair))
        value=distances[(left,right)]; merged=n+step
        merges.append((left,right,value,sizes[left]+sizes[right]))
        for other in sorted(active-{left,right}):
            a=distances[tuple(sorted((left,other)))]
            b=distances[tuple(sorted((right,other)))]
            if linkage==1: new=min(a,b)
            elif linkage==2: new=max(a,b)
            else: new=(a*sizes[left]+b*sizes[right])/(sizes[left]+sizes[right])
            distances[tuple(sorted((merged,other)))]=new
        active.remove(left); active.remove(right); active.add(merged)
        sizes[merged]=sizes[left]+sizes[right]
    return merges

def evaluate(distance,opening,facilities):
    facilities=sorted(facilities); labels=[]; cost=sum(opening[f] for f in facilities)
    for client in range(len(distance[0])):
        pos=min(range(len(facilities)),key=lambda q:(distance[facilities[q]][client],facilities[q]))
        labels.append(pos); cost+=distance[facilities[pos]][client]
    return facilities,labels,cost

def facility_search(distance,opening,limit):
    count=len(opening)
    current=min((evaluate(distance,opening,[f]) for f in range(count)),key=lambda x:(x[2],x[0]))
    while True:
        trials=[evaluate(distance,opening,current[0]+[f]) for f in range(count) if f not in current[0]]
        improving=[trial for trial in trials if trial[2]<current[2]]
        if not improving: break
        current=min(improving,key=lambda x:(x[2],x[0]))
    iterations=0
    for _ in range(limit):
        trials=[evaluate(distance,opening,current[0]+[f]) for f in range(count) if f not in current[0]]
        if len(current[0])>1:
            trials += [evaluate(distance,opening,current[0][:q]+current[0][q+1:])
                       for q in range(len(current[0]))]
        improving=[trial for trial in trials if trial[2]<current[2]]
        if not improving: break
        current=min(improving,key=lambda x:(x[2],x[0])); iterations+=1
    return current,iterations

def main():
    parser=argparse.ArgumentParser(); parser.add_argument("--out-dir",required=True,type=Path)
    args=parser.parse_args(); args.out_dir.mkdir(parents=True,exist_ok=True)
    rng=random.Random(2026071310); index=0
    for _ in range(7):
        points=[[rng.randint(-20,20),rng.randint(-20,20)] for _ in range(rng.randint(2,20))]
        distance=metric(points); k=rng.randint(1,len(points)); first=rng.randrange(len(points))
        centers,labels,radius=gonzalez(distance,k,first)
        data=f"0\n{len(points)} {k} {first}\n"+matrix_text(distance)
        output=f"{radius:.6f}\n"+" ".join(map(str,centers))+"\n"+" ".join(map(str,labels))+"\n"
        write(args.out_dir,index,data,output); index+=1
    distance=metric([[0],[2],[5],[9],[10],[20]])
    for linkage in (1,2,3):
        merges=hierarchy(distance,linkage)
        output="".join(f"{a} {b} {value:.6f} {size}\n" for a,b,value,size in merges)
        write(args.out_dir,index,f"{linkage}\n{len(distance)}\n"+matrix_text(distance),output); index+=1
    for _ in range(6):
        facilities=rng.randint(1,9); clients=rng.randint(1,16)
        opening=[rng.randint(0,20) for _ in range(facilities)]
        distance=[[rng.randint(0,30) for _ in range(clients)] for _ in range(facilities)]
        result,iterations=facility_search(distance,opening,20)
        data=f"4\n{facilities} {clients} 20\n"+" ".join(map(str,opening))+"\n"+matrix_text(distance)
        output=f"{result[2]:.6f} {iterations}\n"+" ".join(map(str,result[0]))+"\n"+" ".join(map(str,result[1]))+"\n"
        write(args.out_dir,index,data,output); index+=1

if __name__=="__main__": main()
