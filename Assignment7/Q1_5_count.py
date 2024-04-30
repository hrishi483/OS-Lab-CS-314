import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

def count_lines(file_path):
    # Initialize counters
    seg_violation_count = 0
    valid_count = 0

    # Open the file and read line by line
    with open(file_path, 'r') as file:
        for line in file:
            if "SEGMENTATION VIOLATION" in line:
                seg_violation_count += 1
            elif "VALID" in line:
                valid_count += 1

    return seg_violation_count, valid_count

def main(s):
    seg_violation_counts = []
    valid_counts = []
    fractions = []

    seed = s

    for i in range(0, 1000):
        file_path = f"Dataset/limit{i}_seed{seed}.txt"
        seg_violation_count, valid_count = count_lines(file_path)
        fractions.append(valid_count/(seg_violation_count+valid_count))
        seg_violation_counts.append(seg_violation_count)
        valid_counts.append(valid_count)

    return seg_violation_counts, valid_counts, fractions

if __name__ == "__main__":
    seg_violation_counts1, valid_counts1,fractions1 = main(1)
    seg_violation_counts2, valid_counts2,fractions2 = main(2)
    seg_violation_counts3, valid_counts3,fractions3 = main(3)


    print("Fractions ",len(fractions1))
    print("Fractions ",len(fractions2))
    print("Fractions ",len(fractions3))


    x = np.array([i for i in range(0,1000)])
    # y = fractions1

    plt.title('Fraction of Randomly generated Virtual Address vs Limit Values')
    plt.plot(x,fractions1,label='seed 1', color='orange')
    plt.plot(x,fractions2, label='seed 2',color='hotpink')
    plt.plot(x,fractions3, label='seed 3',color='purple')  
    plt.grid(True)
    plt.xlabel('Bound Values')
    plt.ylabel('Valid Fraction')
    plt.legend()
    plt.savefig('Q1_5.png')

