def splitpred(pred, xs):
    groups = []
    last_i = 0
    for i, x in enumerate(xs):
        if not pred(x):
            continue
        prev_group = xs[last_i:i]
        if prev_group:
            groups.append(prev_group)
        groups.append(x)
        last_i = i+1
        
    last_group = xs[last_i:]
    if last_group:
        groups.append(last_group)
    return groups