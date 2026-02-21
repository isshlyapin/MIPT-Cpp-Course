import subprocess
import re
import sys

def run_benchmark():
    sizes_start = 15
    sizes_end = 25
    local_sizes = [128, 256, 512, 1024]
    
    print(f"{'LSize':<8} {'Size':<12} {'CPU (ms)':<10} {'GPU (ms)':<10}")
    print("-" * 42)

    for lsize in local_sizes:
        for p in range(sizes_start, sizes_end + 1):
            size = 2**p
            try:
                # Run the command
                result = subprocess.run(
                    ['./build/bitonic', '--size', str(size), '--lsize', str(lsize)],
                    capture_output=True,
                    text=True,
                    check=True
                )
                
                output = result.stdout
                
                # Parse output
                # Expected format:
                # GPU wall time measured: 95 ms
                # GPU pure time measured: 92 ms
                # CPU time measured: 8 ms
                
                gpu_pure_match = re.search(r'GPU pure time measured:\s+(\d+)', output)
                cpu_match = re.search(r'CPU time measured:\s+(\d+)', output)
                
                gpu_time = gpu_pure_match.group(1) if gpu_pure_match else "N/A"
                cpu_time = cpu_match.group(1) if cpu_match else "N/A"
                
                print(f"{lsize:<8} {size:<12} {cpu_time:<10} {gpu_time:<10}")
                sys.stdout.flush()
                
            except subprocess.CalledProcessError as e:
                print(f"{lsize:<8} {size:<12} {'Err':<10} {'Err':<10}")
                # print(e.stderr) # Uncomment for debugging
            except Exception as e:
                 print(f"{lsize:<8} {size:<12} {'Exc':<10} {'Exc':<10}")
        print()

if __name__ == "__main__":
    run_benchmark()
