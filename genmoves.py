dims = ["U"+c for c in "XYTL"]


knightDeltas = []
for di in dims:
    for dj in dims:
        if di!=dj:
            for mi in [["-"],["","-"]][di!="UT" or dj=="UL"]:
                for mj in ["-","+-"][dj!="UT" or di=="UL"]:
                    knightDeltas.append(f"{mi}{di}{mj}2*{dj}")

print(f"int knightDeltas[{len(knightDeltas)}] = {{{', '.join(knightDeltas)}}}")

rbud = [[] for i in range(4)]

for dir in range(1,81):
    ds = []
    for i in range(4):
        dx,dir = (dir+1)%3-1,dir//3
        ds.append(dx)
    if ds[2]>0 and ds[3]==0:
        continue
    n = sum(abs(x) for x in ds)-1
    rbud[n].append("".join( f"{'-+'[d//2+1]}{di}" for d,di in zip(ds,dims) if d))


for s,l in zip(["rook","bishop","unicorn","dragon"],rbud):
    print(f"int {s}Deltas[{len(l)}] = {{{', '.join(l)}}}")