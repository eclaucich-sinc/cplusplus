import h5py
import numpy as np
import matplotlib.pyplot as plt

def generate_plot(ncases):
    metrics = ['areaA', 'areaB', 'diffAB', 'diffBA', 'intsAB', 'unionAB']
    calc_means = []
    ref_means = []
    
    calc_file = f'output{ncases}.h5'
    ref_file = f'outputref{ncases}.h5'
    
    with h5py.File(calc_file, 'r') as f_calc, h5py.File(ref_file, 'r') as f_ref:
        for m in metrics:
            calc_means.append(np.mean(f_calc[m][:]))
            ref_means.append(np.mean(f_ref[m][:]))
            
    x = np.arange(len(metrics))
    width = 0.35
    
    # Usar un estilo agradable si está disponible
    plt.style.use('ggplot')
    
    fig, ax = plt.subplots(figsize=(10, 6))
    rects1 = ax.bar(x - width/2, calc_means, width, label='Calculado (N=100)', color='#3498db')
    rects2 = ax.bar(x + width/2, ref_means, width, label='Referencia (Exacto)', color='#e74c3c')
    
    ax.set_ylabel('Valor Medio')
    ax.set_title(f'Comparación de Resultados ({ncases} casos)')
    ax.set_xticks(x)
    ax.set_xticklabels(metrics)
    ax.legend()
    
    # Añadir valores sobre las barras
    def autolabel(rects):
        for rect in rects:
            height = rect.get_height()
            ax.annotate(f'{height:.2f}',
                        xy=(rect.get_x() + rect.get_width() / 2, height),
                        xytext=(0, 3),  # 3 points vertical offset
                        textcoords="offset points",
                        ha='center', va='bottom', fontsize=9)
                        
    autolabel(rects1)
    autolabel(rects2)
    
    fig.tight_layout()
    output_filename = f'comparacion_resultados_{ncases}.png'
    plt.savefig(output_filename, dpi=300)
    print(f"Gráfica guardada en {output_filename}")
    plt.close()

def main():
    generate_plot(100)
    generate_plot(1000)

if __name__ == '__main__':
    main()
