from seaborn import lineplot
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from collections import defaultdict
import pandas as pd

plt.rcParams.update({'font.size': 22})

NLOGS = 6
logs = [open("experiments-new-%d.logs" % id).readlines() for id in range(NLOGS)]

lines_per_log = 82
maxk = 20

def plot(datalogs):
    data = defaultdict(list)
    name = datalogs[0][0].split('/')[-1].strip()
    print 'Plotting ' + name + '...'
    
    for datalog in datalogs:
        data['lazy'].append(0)
        data['half'].append(0)
        data['ours'].append(0)
        data['k'].append(0)
        for k in range(maxk):
            data['lazy'].append(int(datalog[2+4*k].split()[-1]))
            data['half'].append(int(datalog[3+4*k].split()[-1]))
            data['ours'].append(int(datalog[4+4*k].split()[-1]))
            data['k'].append(k+1)
    
    df = pd.DataFrame(data=data)
    lineplot(data=df, x='k', y='lazy', label="lazy greedy", marker="o")
    lineplot(data=df, x='k', y='half', label="1/2-thresholding", marker="o")
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
    
i = 0
while i < len(logs[0]):
    datalogs = [logs[j][i:i+lines_per_log-1] for j in range(NLOGS)]
    plot(datalogs)
    i += lines_per_log