import numpy as np
import os
import collections

class ABDG:
    def __init__(self, file_path):
        self.EDGE_WEIGHTS = {6: 1.0, 5: 0.9, 4: 0.7, 3: 0.5, 2: 0.3, 1: 0.1}
        self.nodes, self.IDL = self.parse_abdg_file(file_path)
        
    def parse_abdg_file(self, file_path):
        """Parses the authentic ABDG input file format into an Incident-Dependency List."""
        if not os.path.exists(file_path):
            raise FileNotFoundError(f"Required structural graph file '{file_path}' not found.")
            
        with open(file_path, 'r') as f:
            lines = f.read().split()
        
        if not lines:
            return [], []
        
        # Read total number of vertices and names
        n = int(lines[0])
        nodes = lines[1:n+1]
        
        # Determine vertex type counts exactly as done in the C++ framework
        self.r = sum(1 for v in nodes if v.startswith('R'))
        self.a = sum(1 for v in nodes if v.startswith('A'))
        self.p = sum(1 for v in nodes if v.startswith('P'))
        
        # Initialize Incident Dependency List using an array of lists matching node positions
        IDL = [[] for _ in range(n)]
        
        # Read total edge count and map backward dependency paths
        e_start_idx = n + 1
        num_edges = int(lines[e_start_idx])
        
        current = e_start_idx + 1
        for _ in range(num_edges):
            tail = lines[current]
            head = lines[current + 1]
            edge_type = int(lines[current + 2])
            
            # Map head and tail strings to their deterministic indices
            head_idx = self.index(head, nodes)
            IDL[head_idx].append((tail, edge_type))
            current += 3
            
        return nodes, IDL

    def index(self, v, node_list=None):
        """Method definition for index tracking matching the C++ switch-case map layout."""
        if v == "S0":
            return 0
        
        # Handling fallback during init parsing vs execution time
        current_nodes = node_list if node_list is not None else self.nodes
        if v not in current_nodes:
            # Handle out of bounds or trailing edge dependencies gracefully
            return 0
            
        char_type = v[0]
        try:
            id_num = int(v[1:])
        except ValueError:
            return 0

        if char_type == 'R':
            return id_num
        elif char_type == 'A':
            return self.r + id_num
        elif char_type == 'P':
            return self.r + self.a + id_num
        else:
            return self.r + self.a + self.p + id_num

    def dsabm_pruned(self, s, tau):
        """Priority-Aware Pruning Algorithm"""
        temp_ds = set()
        node_relevance = collections.defaultdict(float)
        
        start_node = self.index(s)
        q = collections.deque([(start_node, 1.0)])
        node_relevance[start_node] = 1.0

        while q:
            v, current_w = q.popleft()

            for tail, edge_type in self.IDL[v]:
                u = self.index(tail)
                # Calculate decaying weight
                new_w = current_w * self.EDGE_WEIGHTS.get(edge_type, 0.1)

                # PRUNING LOGIC
                if new_w >= tau and new_w > node_relevance[u]:
                    node_relevance[u] = new_w
                    temp_ds.add(u)
                    q.append((u, new_w))

        temp_ds.discard(start_node)
        return temp_ds

def run_reproducible_sensitivity(file_path, criterion, target_tau=0.15):
    """Evaluates system variance by perturbing the edge weights of the class instance."""
    try:
        graph = ABDG(file_path)
    except FileNotFoundError:
        print(f"{file_path:<25} | File Missing (Place in working directory to parse)")
        return

    # Hold reference to original static mapping
    base_weights = dict(graph.EDGE_WEIGHTS)
    
    np.random.seed(42)
    slice_sizes = []
    
    for _ in range(50):
        # Dynamically inject the randomized, perturbed weights into the active object layout
        graph.EDGE_WEIGHTS = {
            k: min(1.0, max(0.01, v * np.random.uniform(0.8, 1.2))) 
            for k, v in base_weights.items()
        }
        slice_set = graph.dsabm_pruned(criterion, target_tau)
        slice_sizes.append(len(slice_set))
        
    mean_val = np.mean(slice_sizes)
    var_val = np.var(slice_sizes)
    std_val = np.std(slice_sizes)
    
    print(f"{file_path:<25} | {slice_sizes[0]:<10} | {mean_val:<22.2f} | {var_val:<10.2f} | {std_val:<10.2f}")

if __name__ == "__main__":
    print(f"{'Graph Source':<25} | {'Base Size':<10} | {'Mean Size (Perturbed)':<22} | {'Variance':<10} | {'Std Dev':<10}")
    print("-" * 85)
    
    run_reproducible_sensitivity("BTS.txt", "A15", target_tau=0.15)
    run_reproducible_sensitivity("Case Studies/CMS.txt", "P3", target_tau=0.05)
    run_reproducible_sensitivity("Case Studies/FMS.txt", "A18", target_tau=0.10)
    run_reproducible_sensitivity("Case Studies/IHG.txt", "A12", target_tau=0.05)
    run_reproducible_sensitivity("Case Studies/IHS.txt", "A18", target_tau=0.15)
    run_reproducible_sensitivity("Case Studies/PTY.txt", "P2", target_tau=0.05)
    run_reproducible_sensitivity("Case Studies/TMS.txt", "A20", target_tau=0.05)