import subprocess
import re
import sys

def get_unit_factor(val_ns):
    if val_ns == 0: return 1, "ns"
    if val_ns < 1000 * 10:
        return 1, "ns"
    elif val_ns < 1000 * 1000 * 10:
        return 1000, "us"
    elif val_ns < 1000 * 1000 * 1000 * 10:
        return 1_000_000, "ms"
    else:
        return 1_000_000_000, "s"

def format_time_pair(cpu_ns_str, gpu_ns_str):
    if cpu_ns_str == "N/A" or gpu_ns_str == "N/A":
        return f"{cpu_ns_str:<15} {gpu_ns_str:<15}"

    try:
        c_ns = float(cpu_ns_str)
        g_ns = float(gpu_ns_str)
    except ValueError:
        return f"{cpu_ns_str:<15} {gpu_ns_str:<15}"
    
    # Determine unit for CPU
    fact_c, unit_c = get_unit_factor(c_ns)
    # Determine unit for GPU
    fact_g, unit_g = get_unit_factor(g_ns)
    
    # We want the "smallest" unit (factor). 
    final_factor = min(fact_c, fact_g)
    
    if final_factor == 1:
        final_unit = "ns"
    elif final_factor == 1000:
        final_unit = "us"
    elif final_factor == 1_000_000:
        final_unit = "ms"
    else:
        final_unit = "s"
        
    c_val = c_ns / final_factor
    g_val = g_ns / final_factor
    
    if final_factor == 1:
        return f"{c_val:.0f} {final_unit}", f"{g_val:.0f} {final_unit}"
    else:
        return f"{c_val:.2f} {final_unit}", f"{g_val:.2f} {final_unit}"

def run_benchmark():
    sizes_start = 15
    sizes_end = 25
    local_sizes = [128, 256, 512, 1024]
    
    print(f"{'LSize':<8} {'Size':<12} {'CPU':<15} {'GPU':<15}")
    print("-" * 52)
    
    for lsize in local_sizes:
        for p in range(sizes_start, sizes_end + 1):
            size = 2**p
            try:
                result = subprocess.run(
                    ['./build/bitonic', '--size', str(size), '--lsize', str(lsize)],
                    capture_output=True,
                    text=True,
                    check=True
                )
                
                output = result.stdout
                gpu_pure_match = re.search(r'GPU pure time measured:\s+(\d+)', output)
                cpu_match = re.search(r'CPU time measured:\s+(\d+)', output)
                
                gpu_ns = gpu_pure_match.group(1) if gpu_pure_match else "N/A"
                cpu_ns = cpu_match.group(1) if cpu_match else "N/A"
                
                c_str, g_str = format_time_pair(cpu_ns, gpu_ns)
                
                print(f"{lsize:<8} {size:<12} {c_str:<15} {g_str:<15}")
                sys.stdout.flush()
                
            except subprocess.CalledProcessError:
                print(f"{lsize:<8} {size:<12} {'Err':<15} {'Err':<15}")
            except Exception:
                 print(f"{lsize:<8} {size:<12} {'Exc':<15} {'Exc':<15}")
        print()

if __name__ == "__main__":
    run_benchmark()
