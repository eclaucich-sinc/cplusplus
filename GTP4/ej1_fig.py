import numpy as np
import matplotlib.pyplot as plt
import os

# Crear la carpeta de imágenes si no existe
os.makedirs('imgs', exist_ok=True)

# Grilla 2D equivalente a la integración numérica del C++
x = np.linspace(-2.5, 2.5, 800)
y = np.linspace(-2.5, 2.5, 800)
X, Y = np.meshgrid(x, y)

# Figuras base (Cuadrado de 4x4 y Círculo de R=1)
sq = (np.abs(X) <= 2.0) & (np.abs(Y) <= 2.0)
circ = (X**2 + Y**2 <= 1.0)

# Operaciones booleanas (igual lógica que en el C++)
inter = sq & circ
uni = sq | circ
diff = sq & (~circ)

# Configuración de los subplots
fig, axs = plt.subplots(1, 3, figsize=(15, 5))
color_fill = '#10B981' # Color de rellenado para el resultado

titles = [
    'Intersección\n(Círculo $\cap$ Cuadrado)',
    'Unión\n(Círculo $\cup$ Cuadrado)',
    'Diferencia\n(Cuadrado - Círculo)'
]

for ax, mask, title in zip(axs, [inter, uni, diff], titles):
    # Pintar área resultante
    ax.contourf(X, Y, mask.astype(float), levels=[0.5, 1.5], colors=[color_fill], alpha=0.5)
    
    # Dibujar contornos originales punteados como referencia
    ax.contour(X, Y, sq.astype(float), levels=[0.5], colors='black', linewidths=1.5, linestyles='--')
    ax.contour(X, Y, circ.astype(float), levels=[0.5], colors='black', linewidths=1.5, linestyles='--')
    
    ax.set_title(title, fontsize=14, fontweight='bold')
    ax.set_aspect('equal')
    ax.axis('off')

plt.tight_layout()
plt.savefig('imgs/operaciones_booleanas.png', dpi=200, bbox_inches='tight')
print("Figura de operaciones booleanas generada correctamente en imgs/operaciones_booleanas.png")