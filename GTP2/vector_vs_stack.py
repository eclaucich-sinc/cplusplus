import matplotlib.pyplot as plt
import numpy as np


# vector_times = np.array([[1, 1, 63, 359, 1775, 3597],
#                         [1, 1, 71, 399, 1783, 3675],
#                         [1, 1, 88, 367, 1759, 3557],
#                         [1, 1, 84, 460, 1900, 3622],
#                         [1, 1, 74, 377, 1797, 3863]])

vector_times = np.array([[1, 1, 12, 885, 22578, 91334],
                         [1, 1, 14, 885, 22495, 90561],
                         [1, 1, 12, 897, 22403, 90748],
                         [1, 1, 12, 809, 22497, 90834],
                         [1, 1, 13, 878, 22456, 90812]])


stack_times = np.array([[1,1,1,1,3,3],
                        [1,1,1,1,3,2],
                        [1,1,1,1,3,5],
                        [1,1,1,1,3,3],
                        [1,1,1,1,3,3]])


mean_vector_times = np.mean(vector_times, axis=0)
mean_stack_times = np.mean(stack_times, axis=0)

std_vector_times = np.std(vector_times, axis=0)
std_stack_times = np.std(stack_times, axis=0)

Ns = [10, 100, 1000, 10000, 50000, 100000]


log_ts = np.log10(mean_vector_times)
log_ns = np.log10(Ns)

m = np.polyfit(log_ns, log_ts, 1)[0]
print(m)


fig = plt.figure(figsize=(12, 6))
fig.suptitle('Comparación de tiempos de ejecución entre Vector y Stack', fontsize=13)
plt.subplot(1,2,1)
plt.plot(Ns, mean_vector_times, label='Vector', marker='o')
plt.fill_between(Ns, np.array(mean_vector_times) - np.array(std_vector_times),
                 np.array(mean_vector_times) + np.array(std_vector_times), alpha=0.2)
plt.plot(Ns, mean_stack_times, label='Stack', marker='o')
plt.fill_between(Ns, np.array(mean_stack_times) - np.array(std_stack_times),
                 np.array(mean_stack_times) + np.array(std_stack_times), alpha=0.2)
plt.xlabel('N')
plt.ylabel('Tiempo de ejecución (ms)')
plt.legend()
plt.grid(True, which="both", ls="--")


plt.subplot(1,2,2)
plt.plot(Ns, mean_vector_times, label='Vector', marker='o')
plt.fill_between(Ns, np.array(mean_vector_times) - np.array(std_vector_times),
                 np.array(mean_vector_times) + np.array(std_vector_times), alpha=0.2)
plt.plot(Ns, mean_stack_times, label='Stack', marker='o')
plt.fill_between(Ns, np.array(mean_stack_times) - np.array(std_stack_times),
                 np.array(mean_stack_times) + np.array(std_stack_times), alpha=0.2)
plt.plot(Ns, 10**(2*log_ns), label=f'Pendiente m={2}', linestyle='--')
plt.xscale('log')
plt.yscale('log')
plt.xlabel('N (log)')
plt.ylabel('Tiempo de ejecución (ms) (log)')
plt.legend()
plt.grid(True, which="both", ls="--")
plt.savefig('ej_vector_vs_stack.png')