import pandas as pd
import matplotlib.pyplot as plt

data = """
6068 0
6011 1
6716 2
5888 3
5806 4
6673 5
5705 6
6431 7
6449 8
5434 9
6358 10
6327 11
5816 12
6781 13
5858 14
5771 15
6702 16
5895 17
6352 18
6513 19
5441 20
6539 21
6127 22
5827 23
6747 24
5822 25
5824 26
6706 27
5690 28
6449 29
6404 30
5541 31
6428 32
6300 33
5837 34
6762 35
6082 36
5714 37
6658 38
5922 39
6089 40
6689 41
5605 42
6062 43
6626 44
5708 45
18433 46
33655 47
3850 48
3157 49
3770 50
3326 51
3754 52

"""


lines = [line.strip().split() for line in data.strip().split('\n') if line.strip()]
parsed = [(int(parts[1]), int(parts[0])) for parts in lines]

df = pd.DataFrame(parsed, columns=['Sample_Index', 'Voltage'])

# Plotting
plt.figure(figsize=(12, 5))
plt.plot(df['Sample_Index'], df['Voltage'], color='#1f77b4', linewidth=1.5, marker='o', markersize=3, label='Voltage Value')
plt.title('Voltage Sine Wave Signal', fontsize=14, fontweight='bold', pad=12)
plt.xlabel('Sample Index', fontsize=12)
plt.ylabel('Voltage', fontsize=12)
plt.grid(True, linestyle='--', alpha=0.6)
plt.legend(loc='upper right')
plt.tight_layout()
plt.savefig('voltage_sine_wave.png', dpi=300)
plt.show()
