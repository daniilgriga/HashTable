import matplotlib.pyplot as plt
import numpy as np

versions = ['base', 'boost_strcmp', 'nasm', 'inline']

search_instr = [4.440e10,  3.141e10, 2.512e10, 2.182e9]
main_instr   = [0.013e10, 0.011e10, 0.011e10, 0.01e10]

search_error = [0.03e9, 0.02e9, 0.02e9, 0.03e9]
main_error   = [0.03e9,  0.02e9, 0.02e9, 0.03e9]

fig, ax = plt.subplots()

bar_width = 0.5

x = np.arange (len(versions))

ax.bar (x, search_instr, bar_width, label='search()', color='blue', yerr=search_error, capsize=5, ecolor='orange')
ax.bar (x, main_instr, bar_width, bottom=search_instr, label='main()', color='red', yerr=main_error, capsize=5, ecolor='orange')

ax.set_xlabel ('Versions')
ax.set_ylabel ('Instructions')
ax.set_title ('Perf results')
ax.set_xticks (x)
ax.set_xticklabels (versions)

ax.ticklabel_format (style='sci', axis='y', scilimits=(0,0))

ax.legend()

plt.tight_layout()
plt.show()
