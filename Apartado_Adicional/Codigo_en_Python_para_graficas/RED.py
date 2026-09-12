import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.lines import Line2D

def parse_spin_file(filename, grid_size):
    """
    Lee el fichero donde cada 3 líneas son (Sx, Sy, Sz) de cada nodo
    y los nodos están ordenados con x variando más rápido, luego y, luego z:
      index_linear = x + y*L + z*L^2
    Devuelve edges_x, edges_y, edges_z con shape (L,L,L).
    """
    with open(filename, 'r') as f:
        lines = [ln.strip() for ln in f if ln.strip() != '']
    spins = [int(x) for x in lines]

    expected = 3 * grid_size**3
    if len(spins) != expected:
        raise ValueError(f"Se esperaban {expected} valores (3*L^3), hay {len(spins)}")

    edges_x = np.zeros((grid_size, grid_size, grid_size), dtype=int)
    edges_y = np.zeros((grid_size, grid_size, grid_size), dtype=int)
    edges_z = np.zeros((grid_size, grid_size, grid_size), dtype=int)

    idx = 0
    # Recorremos los nodos en orden lineal y calculamos (x,y,z) con x más rápido
    for n in range(grid_size**3):
        x = n % grid_size
        y = (n // grid_size) % grid_size
        z = n // (grid_size * grid_size)

        edges_x[x, y, z] = spins[idx]; idx += 1
        edges_y[x, y, z] = spins[idx]; idx += 1
        edges_z[x, y, z] = spins[idx]; idx += 1

    print(f"Parseado: {idx} espines (3*{grid_size}^3). Orden: x rápido, luego y, luego z.")
    return edges_x, edges_y, edges_z


def plot_3d_lattice_periodic(edges_x, edges_y, edges_z, show_numbers=True):
    """
    Dibuja la red 3D:
      - Cada arista se dibuja una sola vez (desde su nodo origen hacia +x, +y, +z).
      - Si el origen está en la cara máxima (x==L-1, etc.), la arista periódica se dibuja
        con línea discontínua (solo en dirección positiva) para no duplicar.
    """
    fig = plt.figure(figsize=(14, 10))
    ax = fig.add_subplot(111, projection='3d')

    L = edges_x.shape[0]
    col_plus = 'blue'
    col_minus = 'red'

    # Aristas en X: desde (x,y,z) hacia (x+1,y,z)
    for x in range(L):
        for y in range(L):
            for z in range(L):
                spin = edges_x[x, y, z]
                color = col_plus if spin > 0 else col_minus
                x0, y0, z0 = x, y, z
                x1 = (x + 1) % L
                if x != L - 1:
                    ax.plot([x0, x1], [y0, y0], [z0, z0], color=color, linewidth=2, alpha=0.9)
                else:
                    # Conexión periódica representada solo en la dirección positiva (discontínua)
                    # dibujamos hasta la coordenada L para que se vea el "salto"
                    ax.plot([x0, L], [y0, y0], [z0, z0], color=color, linewidth=2, linestyle='--', alpha=0.9)

    # Aristas en Y: desde (x,y,z) hacia (x,y+1,z)
    for x in range(L):
        for y in range(L):
            for z in range(L):
                spin = edges_y[x, y, z]
                color = col_plus if spin > 0 else col_minus
                x0, y0, z0 = x, y, z
                y1 = (y + 1) % L
                if y != L - 1:
                    ax.plot([x0, x0], [y0, y1], [z0, z0], color=color, linewidth=2, alpha=0.9)
                else:
                    ax.plot([x0, x0], [y0, L], [z0, z0], color=color, linewidth=2, linestyle='--', alpha=0.9)

    # Aristas en Z: desde (x,y,z) hacia (x,y,z+1)
    for x in range(L):
        for y in range(L):
            for z in range(L):
                spin = edges_z[x, y, z]
                color = col_plus if spin > 0 else col_minus
                x0, y0, z0 = x, y, z
                z1 = (z + 1) % L
                if z != L - 1:
                    ax.plot([x0, x0], [y0, y0], [z0, z1], color=color, linewidth=2, alpha=0.9)
                else:
                    ax.plot([x0, x0], [y0, y0], [z0, L], color=color, linewidth=2, linestyle='--', alpha=0.9)

    # Numeración de nodos con fondo amarillo (si se desea)
    if show_numbers:
        for x in range(L):
            for y in range(L):
                for z in range(L):
                    node_num = x + y * L + z * (L**2) + 1
                    ax.text(x, y, z, str(node_num),
                            fontsize=8, ha='center', va='center',
                            bbox=dict(facecolor='yellow', alpha=0.85, edgecolor='none', boxstyle='round,pad=0.2'))

    # Etiquetas, leyenda y límites
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')
    ax.set_title('Red 3D con condiciones periódicas (solo en dirección positiva)', fontsize=12)

    legend_elems = [
        Line2D([0], [0], color=col_plus, lw=2, label='Spin +1'),
        Line2D([0], [0], color=col_minus, lw=2, label='Spin -1'),
        Line2D([0], [0], color='black', lw=2, linestyle='--', label='Conexión periódica (+)'),
    ]
    ax.legend(handles=legend_elems, loc='upper left', bbox_to_anchor=(1.02, 1), borderaxespad=0.)

    # Ajuste para ver los saltos periódicos
    ax.set_xlim(-0.5, L)
    ax.set_ylim(-0.5, L)
    ax.set_zlim(-0.5, L)
    ax.set_box_aspect((1,1,1))
    ax.view_init(elev=25, azim=35)

    plt.tight_layout()
    plt.show()


def visualize_spin_network_periodic(filename, grid_size):
    edges_x, edges_y, edges_z = parse_spin_file(filename, grid_size)
    plot_3d_lattice_periodic(edges_x, edges_y, edges_z)
    return edges_x, edges_y, edges_z


# === EJEMPLO DE USO ===
# Usa r"ruta\al\archivo.txt" si estás en Windows
filename = r"Resultados_simulacion\CONFIGURACION_INICIAL\I_27.txt"
L = 4

edges_x, edges_y, edges_z = visualize_spin_network_periodic(filename, L)
