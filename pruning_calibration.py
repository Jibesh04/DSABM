import matplotlib.pyplot as plt
import numpy as np

def parse_abdg_file(file_path):
    """Parses the ABDG input file format."""
    with open(file_path, 'r') as f:
        lines = f.read().split()
    
    if not lines:
        return [], {}, 0
    
    # 1. Read Number of Vertices
    n = int(lines[0])
    # 2. Read Vertex Names
    nodes = lines[1:n+1]
    
    # 3. Read Number of Edges
    e_start_idx = n + 1
    num_edges = int(lines[e_start_idx])
    
    # 4. Read Edges and build Incident-Dependency List (Backward Graph)
    edges = {}
    current = e_start_idx + 1
    for _ in range(num_edges):
        tail = lines[current]
        head = lines[current + 1]
        edge_type = int(lines[current + 2])
        
        if head not in edges:
            edges[head] = []
        edges[head].append((tail, edge_type))
        current += 3
        
    return nodes, edges

def simulate_dsabm(nodes, edges, criterion, tau, weights):
    """Implementation of the priority-aware slicing algorithm."""
    ds = {criterion}
    queue = [(criterion, 1.0)]
    relevance = {criterion: 1.0}
    
    while queue:
        v, current_w = queue.pop(0)
        for u, edge_type in edges.get(v, []):
            new_w = current_w * weights.get(edge_type, 0.1)
            if new_w >= tau and new_w > relevance.get(u, 0):
                relevance[u] = new_w
                ds.add(u)
                queue.append((u, new_w))
    
    if criterion in ds:
        ds.remove(criterion)
    return ds

def calibrate_threshold(file_path, criterion):
    nodes, edges = parse_abdg_file(file_path)
    weight_map = {6: 1.0, 5: 0.9, 4: 0.7, 3: 0.5, 2: 0.3, 1: 0.1}
    
    thresholds = np.linspace(0.0, 1.0, 21) 
    slice_sizes = []
    
    print(f"\nCalibration for Criterion: {criterion}")
    print(f"{'Threshold (Tau)':<15} | {'Slice Size':<10} | {'Compression %':<15}")
    print("-" * 45)
    
    # Get base size at tau=0.0 for compression calculation
    full_ds = simulate_dsabm(nodes, edges, criterion, 0.0, weight_map)
    full_size = len(full_ds) if len(full_ds) > 0 else 1 # Avoid division by zero

    for tau in thresholds:
        current_ds = simulate_dsabm(nodes, edges, criterion, tau, weight_map)
        size = len(current_ds)
        
        # Calculate Compression %
        compression = (1 - (size / full_size)) * 100
        slice_sizes.append(size)
        
        print(f"{tau:<15.2f} | {size:<10} | {compression:<15.1f}%")

    # Knee Point
    x = thresholds
    y = np.array(slice_sizes)
    
    # Check for constant slice size to avoid RuntimeWarning
    if np.ptp(y) == 0:
        tau_opt = 0.0
    else:
        x_norm = (x - x.min()) / np.ptp(x)
        y_norm = (y - y.min()) / np.ptp(y)
        p1 = np.array([x_norm[0], y_norm[0]])
        p2 = np.array([x_norm[-1], y_norm[-1]])
        distances = []
        for i in range(len(x_norm)):
            p3 = np.array([x_norm[i], y_norm[i]])
            dist = np.abs(np.cross(p2-p1, p1-p3)) / np.linalg.norm(p2-p1)
            distances.append(dist)
        knee_idx = np.argmax(distances)
        tau_opt = thresholds[knee_idx]

    print(f"\nOptimal Threshold (Tau_opt) identified at: {tau_opt:.2f}\n")

    # Plotting
    plt.figure(figsize=(8, 5))
    plt.plot(thresholds, slice_sizes, marker='s', linestyle='-', color='b', label='Slice Size')
    plt.axvline(x=tau_opt, color='red', linestyle='--', label=f'Optimal Tau ({tau_opt:.2f})')
    plt.title(f"Knee-Point Calibration (Criterion: {criterion})")
    plt.xlabel("Pruning Threshold (Tau)")
    plt.ylabel("Nodes in Slice")
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.savefig('plot_filename.png', dpi=300, bbox_inches='tight')
    plt.show()

# Main
calibrate_threshold("input_file.txt", "A15")