import time
import matplotlib.pyplot as plt


# Lists to store the execution times
# day_parse_10000 = []
# test2_10000_treediff_parse = [0.0203256, 0.020939, 0.0213194, 0.0230991, 0.0212019]


# test2_10000_day_parse = []
# test2_10000_day_alg = []

# day_parse_time = [0.0192521, 0.0353947, 0.0418901, 0.0660122, 0.0861058, 0.0900307, 0.122115, 0.124687, 0.147837, 0.15465]
# day_algorithm_time = [0.0053983, 0.0100662, 0.0161749, 0.0223244, 0.0279225, 0.0354709, 0.0421111, 0.0480477, 0.0537256, 0.0612711]

# treediff_parse_time = [0.0219721, 0.03916, 0.051773, 0.0745898, 0.0962205, 0.104265, 0.126698, 0.162573, 0.16667, 0.212179]
# treediff_algorithm_time = [0.0871719, 0.220918, 0.573953, 1.10068, 1.34823, 1.84379, 2.35461,  3.01787, 3.34216, 4.20205]

execution_times_2 = [0.369, 0.645, 1.1, 1.2, 1.8, 2.2, 2.1, 2.6, 3.7, 3.7]
execution_times_1 = [0.741, 1.4, 1.6, 2.7, 3.1, 3.2, 5.3, 5.3, 6.1, 6.2]

# Generate x-axis values for the plot
x_values = [10000, 20000, 30000, 40000, 50000, 60000, 70000, 80000, 90000, 100000]


# Plot the execution times
plt.plot(x_values, execution_times_1, label='Day')
plt.plot(x_values, execution_times_2, label='Treediff')

plt.scatter(x_values, execution_times_1, marker='x')
plt.scatter(x_values, execution_times_2, marker='o')

# Add labels and title to the plot
plt.xlabel('Num leaves')
plt.ylabel('Execution memory peak (MiB)')
plt.title('Comparison of algorithm memory peak')

# Add a legend
plt.legend()

# Display the plot
plt.show()

plt.savefig('algorithm_memory.png')