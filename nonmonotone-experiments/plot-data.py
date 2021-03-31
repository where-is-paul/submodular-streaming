from seaborn import lineplot
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from collections import defaultdict
import pandas as pd

plt.rcParams.update({'font.size': 22})

datasets = ['yahoo', 'bing', 'gowalla']
logs = [open("logs/experiments-%s.log" % ds).readlines() for ds in datasets]

def plot(name, datalog):
    data = defaultdict(list)
    print 'Plotting ' + name + '...'

    maxk = 10
    offset = 0
    while offset < len(datalog):
        data['lazy'].append(0)
        data['rand'].append(0)
        data['ours'].append(0)
        data['k'].append(0)
        for k in range(maxk):
            line = datalog[offset+k].split()
            data['lazy'].append(float(line[1]))
            data['rand'].append(float(line[2]))
            data['ours'].append(float(line[3]))
            data['k'].append(k+1)
        offset += maxk
    
    df = pd.DataFrame(data=data)
    #lineplot(data=df, x='k', y='lazy', label="standard greedy", marker="o")
    lineplot(data=df, x='k', y='rand', label="random greedy", marker="o")
    ax = lineplot(data=df, x='k', y='ours', label="our algorithm", marker="o")
    ax.xaxis.set_major_locator(MaxNLocator(integer=True))
    
    plt.xlabel('k')
    plt.ylabel('objective')
    plt.legend(frameon=False, loc='lower right')
    if '.dat' in name:
        plt.title('{}'.format(name[:-4]))
    else:
        plt.title('{}'.format(name))
    plt.savefig("./plots/" + name + ".eps", bbox_inches='tight')
    plt.savefig("./plots/" + name + ".png", bbox_inches='tight')
    plt.clf()

for i, name in enumerate(datasets):    
    plot(name, logs[i])