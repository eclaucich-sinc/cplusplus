import numpy as np
import matplotlib.pyplot as plt
import pandas as pd

N_matches = 1000

results = pd.read_csv('tictactoe_results.csv')

title_size = 16
axis_size = 14

label_x = "Gana X"
label_o = "Gana O"
label_empates = "Empates"

players = ["dumb", "random", "agressive", "minimax", "rule_based"]

for player in players:
    x_vs_all = results[results['JugadorX'] == player]

    plt.figure(figsize=(10, 6))
    bar_width = 0.2

    index = np.arange(len(x_vs_all['JugadorO'].unique()))

    plt.plot(index, x_vs_all['X'], marker='o', markersize=10, label=label_x)
    plt.plot(index, x_vs_all['O'], marker='o', markersize=10, label=label_o)
    plt.plot(index, x_vs_all['Empates'], marker='o', markersize=10, label=label_empates)

    plt.title(f"Resultados de JugadorX ({player}) vs JugadorO", fontsize=title_size)
    plt.xlabel('JugadorO', fontsize=axis_size)
    plt.ylabel('Resultados en {} partidas'.format(N_matches), fontsize=axis_size)
    plt.xticks(index, players, fontsize=axis_size)
    plt.legend()
    plt.grid()
    plt.tight_layout()
    plt.savefig(f"imgs/{player}_vs_all.png")

