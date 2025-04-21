import pandas as pd
import matplotlib.pyplot as plt

with open ('test/data.txt', 'r') as file:
    title = file.readline().strip()

df = pd.read_csv ('test/data.txt', header=None, names=['n', 'm'], skiprows=1)

# ===== check data ===== #
print ("Title:", title)
print ("First 5 rows:")
print (df.head())
# ====================== #

n = df['n']
m = df['m']

max_elements = m.max()
variance = m.var()

plt.figure (figsize =(10, 6))
plt.bar (n, m, color='skyblue', edgecolor='purple')

plt.xlabel ('Buckets number')
plt.ylabel ('Number of elements')
plt.title (title)

plt.grid (True, alpha=0.3)

plt.ylim (0, 80)

plt.text (0.02, 0.96, f'Max collisions: {max_elements}\nVariance: {variance:.2f}',
          transform=plt.gca().transAxes,
          fontsize=12,
          verticalalignment='top',
          bbox=dict (facecolor='white', alpha=0.8))

filename = f'img/histo{title}.png'
#plt.savefig (filename, dpi=300, bbox_inches='tight')

plt.show()
