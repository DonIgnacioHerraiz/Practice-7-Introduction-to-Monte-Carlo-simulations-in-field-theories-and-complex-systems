import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Patch
from matplotlib.lines import Line2D

def visualizar_plano_2d(filename, nodo, flag='z', mostrar_numeros=True):
    """
    Visualiza un plano 2D de la red cúbica con aristas coloreadas y condiciones periódicas.
    Asume que en el fichero cada 3 líneas son (Sx, Sy, Sz) para cada nodo y que los nodos
    están ordenados con x cambiando más rápido, luego y, luego z:
        index = x + y*L + z*L*L
    Origen (0,0) en esquina inferior izquierda.
    """
    # --- Leer datos ---
    data = np.loadtxt(filename)
    N = len(data)
    L = int(round((N / 3) ** (1/3)))
    if 3 * L**3 != N:
        raise ValueError("El número de líneas no corresponde a una red cúbica 3D (3*L^3).")
    
    # Reshape y reordenar ejes para tener spins[x,y,z,dir]
    raw = data.reshape((L, L, L, 3))   # raw has axes (z,y,x,dir) because of how linear index maps
    spins = raw.transpose(2, 1, 0, 3)  # now spins[x,y,z,dir]

    # --- convertir nodo -> (x0,y0,z0) según orden x rápido ---
    n0 = nodo - 1
    x0 = n0 % L
    y0 = (n0 // L) % L
    z0 = n0 // (L * L)

    fig, ax = plt.subplots(figsize=(7, 7))

    # --- Dibujado por plano ---
    if flag == 'z':
        const = z0
        for x in range(L):
            for y in range(L):
                # Arista en x (entre (x,y) y (x+1,y) en este plano z=const)
                s_x = int(spins[x, y, const, 0])
                color_x = 'b' if s_x == 1 else 'r'
                x2 = (x + 1) % L
                if x2 == 0:  # cruza borde: dibujar segmento hasta L (dashed) y stub en x=0
                    ax.plot([x, L], [y, y], color=color_x, lw=2, ls='--')
                    ax.plot([0, 0.4], [y, y], color=color_x, lw=1.5, ls='--', alpha=0.6)
                else:
                    ax.plot([x, x2], [y, y], color=color_x, lw=2)

                # Arista en y (entre (x,y) y (x,y+1))
                s_y = int(spins[x, y, const, 1])
                color_y = 'b' if s_y == 1 else 'r'
                y2 = (y + 1) % L
                if y2 == 0:  # cruza borde en y
                    ax.plot([x, x], [y, L], color=color_y, lw=2, ls='--')
                    ax.plot([x, x], [0, 0.4], color=color_y, lw=1.5, ls='--', alpha=0.6)
                else:
                    ax.plot([x, x], [y, y2], color=color_y, lw=2)

                # Numeración correcta con x rápido
                if mostrar_numeros:
                    num = 1 + x + y * L + const * (L * L)
                    ax.text(x + 0.05, y + 0.05, str(num),
                            fontsize=9, color='black', ha='left', va='bottom',
                            bbox=dict(facecolor='yellow', alpha=0.75, edgecolor='none',
                                      boxstyle='round,pad=0.15'))

        ax.set_xlabel("Eje X")
        ax.set_ylabel("Eje Y")
        ax.set_title(f"Plano z={const} (direcciones x,y)", fontsize=12, pad=10)

    elif flag == 'x':
        const = x0
        for y in range(L):
            for z in range(L):
                # Arista en y (entre (y,z) y (y+1,z) en plano x=const)
                s_y = int(spins[const, y, z, 1])
                color_y = 'b' if s_y == 1 else 'r'
                y2 = (y + 1) % L
                if y2 == 0:
                    ax.plot([y, L], [z, z], color=color_y, lw=2, ls='--')
                    ax.plot([0, 0.4], [z, z], color=color_y, lw=1.5, ls='--', alpha=0.6)
                else:
                    ax.plot([y, y2], [z, z], color=color_y, lw=2)

                # Arista en z (entre (y,z) y (y,z+1))
                s_z = int(spins[const, y, z, 2])
                color_z = 'b' if s_z == 1 else 'r'
                z2 = (z + 1) % L
                if z2 == 0:
                    ax.plot([y, y], [z, L], color=color_z, lw=2, ls='--')
                    ax.plot([y, y], [0, 0.4], color=color_z, lw=1.5, ls='--', alpha=0.6)
                else:
                    ax.plot([y, y], [z, z2], color=color_z, lw=2)

                if mostrar_numeros:
                    num = 1 + const + y * L + z * (L * L)
                    ax.text(y + 0.05, z + 0.05, str(num),
                            fontsize=9, color='black', ha='left', va='bottom',
                            bbox=dict(facecolor='yellow', alpha=0.75, edgecolor='none',
                                      boxstyle='round,pad=0.15'))

        ax.set_xlabel("Eje Y")
        ax.set_ylabel("Eje Z")
        ax.set_title(f"Plano x={const} (direcciones y,z)", fontsize=12, pad=10)

    elif flag == 'y':
        const = y0
        for x in range(L):
            for z in range(L):
                # Arista en x (entre (x,z) y (x+1,z) en plano y=const)
                s_x = int(spins[x, const, z, 0])
                color_x = 'b' if s_x == 1 else 'r'
                x2 = (x + 1) % L
                if x2 == 0:
                    ax.plot([x, L], [z, z], color=color_x, lw=2, ls='--')
                    ax.plot([0, 0.4], [z, z], color=color_x, lw=1.5, ls='--', alpha=0.6)
                else:
                    ax.plot([x, x2], [z, z], color=color_x, lw=2)

                # Arista en z (entre (x,z) y (x,z+1))
                s_z = int(spins[x, const, z, 2])
                color_z = 'b' if s_z == 1 else 'r'
                z2 = (z + 1) % L
                if z2 == 0:
                    ax.plot([x, x], [z, L], color=color_z, lw=2, ls='--')
                    ax.plot([x, x], [0, 0.4], color=color_z, lw=1.5, ls='--', alpha=0.6)
                else:
                    ax.plot([x, x], [z, z2], color=color_z, lw=2)

                if mostrar_numeros:
                    num = 1 + x + const * L + z * (L * L)
                    ax.text(x + 0.05, z + 0.05, str(num),
                            fontsize=9, color='black', ha='left', va='bottom',
                            bbox=dict(facecolor='yellow', alpha=0.75, edgecolor='none',
                                      boxstyle='round,pad=0.15'))

        ax.set_xlabel("Eje X")
        ax.set_ylabel("Eje Z")
        ax.set_title(f"Plano y={const} (direcciones x,z)", fontsize=12, pad=10)

    else:
        raise ValueError("flag debe ser 'x', 'y' o 'z'.")

    # --- Ajustes visuales ---
    ax.set_xlim(0, L)
    ax.set_ylim(0, L)
    ax.set_aspect('equal')
    ax.set_xticks(range(L + 1))
    ax.set_yticks(range(L + 1))
    ax.grid(True, linestyle=':', alpha=0.3)

    # Origen abajo-izquierda: NO invertimos Y
    # ax.invert_yaxis()  # eliminado intencionadamente

    # --- Leyenda fuera del gráfico (abajo) ---
    legend_elements = [
        Line2D([0], [0], color='b', lw=2, label='Espín +1'),
        Line2D([0], [0], color='r', lw=2, label='Espín -1'),
        Line2D([0], [0], color='k', lw=1.5, ls='--', label='Arista periódica (salto)'),
        Patch(facecolor='yellow', alpha=0.75, edgecolor='none', label='Nodo (número)')
    ]
    ax.legend(handles=legend_elements,
              loc='upper center',
              bbox_to_anchor=(0.5, -0.08),
              ncol=4,
              framealpha=0.9)

    plt.tight_layout(rect=(0, 0.05, 1, 1))
    plt.show()


# Ejemplo de uso:
visualizar_plano_2d("Resultados_simulacion/CONFIGURACION_INICIAL/I_27.txt", nodo=1, flag='z')
