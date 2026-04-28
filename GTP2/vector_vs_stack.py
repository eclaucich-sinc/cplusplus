import matplotlib.pyplot as plt
import numpy as np


vector_times = np.array([[0, 0, 63, 359, 1775, 3597],
                        [0, 0, 71, 399, 1783, 3675],
                        [0, 1, 88, 367, 1759, 3557],
                        [0, 1, 84, 460, 1900, 3622],
                        [0, 0, 74, 377, 1797, 3863]])


stack_times = np.array([[0,0,0,1,8,13],
                        [0,0,0,1,5,16],
                        [0,0,0,1,8,17],
                        [0,0,0,1,6,16],
                        [0,0,0,1,9,15]])


mean_vector_times = np.mean(vector_times, axis=0)
mean_stack_times = np.mean(stack_times, axis=0)

std_vector_times = np.std(vector_times, axis=0)
std_stack_times = np.std(stack_times, axis=0)

Ns = [10, 100, 1000, 10000, 50000, 100000]

plt.figure()
plt.plot(Ns, mean_vector_times, label='Vector', marker='o')
plt.fill_between(Ns, np.array(mean_vector_times) - np.array(std_vector_times),
                 np.array(mean_vector_times) + np.array(std_vector_times), alpha=0.2)
plt.plot(Ns, mean_stack_times, label='Stack', marker='o')
plt.fill_between(Ns, np.array(mean_stack_times) - np.array(std_stack_times),
                 np.array(mean_stack_times) + np.array(std_stack_times), alpha=0.2)

plt.xlabel('N')
plt.ylabel('Tiempo de ejecución (ms)')
plt.title('Comparación de tiempos de ejecución entre Vector y Stack')
plt.legend()
plt.grid(True, which="both", ls="--")
plt.savefig('ej_vector_vs_stack.png')