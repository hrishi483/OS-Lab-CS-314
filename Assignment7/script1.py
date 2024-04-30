import subprocess
from tqdm import tqdm

def main(seed=1, i=0):
    # Command to execute
    n=100
    command = ["python", "relocation.py", "-c", "-l", f"{i}", "-s",f"{seed}","-n",f"{n}"]


    # Redirect output to limit0.txt
    with open(f"Data/limit{i}_seed{seed}.txt", "w") as output_file:
        # Run the command and redirect stdout to the output file
        subprocess.run(command, stdout=output_file)
    print(f"File generated: limit{i}_seed{seed}.txt")

if __name__ == "__main__":
    # Define the total number of iterations
    total_iterations = 2 * 1000  # 2 * (number of seeds) * (number of iterations)

    # Use tqdm to create a progress bar
    progress_bar = tqdm(total=total_iterations, desc="Generating Files")

    for s in range(1, 3):
    # s=3
        for i in range(0, 1000):
            main(s, i)
            # Update the progress bar after each iteration
            progress_bar.update(1)

    # Close the progress bar after completion
    progress_bar.close()
