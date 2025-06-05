import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

def visualize_cache_results(result):

    plt.figure(figsize=(12, 6))
    markers = ['o', 's', '^', 'D', 'v']
    
    if(result == 1):
        df = pd.read_csv('./data/miss_rate_vs_cache_size.csv')
        for i, (key, group) in enumerate(df.groupby('associativity')):
            plt.plot(group['cache_size_power'], group['miss_rate'], label=f'{key}-way Associativity', linewidth=2.5, marker=markers[i % len(markers)])

        plt.xlabel('L1 Cache Size (Powers of 2)', fontsize=12, fontweight='bold')
        plt.ylabel('L1 Miss Rate', fontsize=12, fontweight='bold')
        plt.title('L1 Miss Rate vs L1 Cache Size Power', fontsize=14, fontweight='bold')
        plt.grid(True)
        plt.legend()
        plt.tight_layout()
        
        
        all_cache_powers = sorted(df['cache_size_power'].unique())
        plt.xticks(all_cache_powers, [f'$2^{{{p}}}$' for p in all_cache_powers])

        plt.savefig('./graphs/miss_rate_vs_cache_size.png', dpi=300)
        print("Image saved in ./graphs/miss_rate_vs_cache_size.png")


    else:
        df = pd.read_csv('./data/miss_rate_vs_block_size.csv')

        for i, (key, group) in enumerate(df.groupby('cache_size')):
            plt.plot(group['block_size_power'], group['miss_rate'], label=f'Cache Size: {key} bytes', linewidth=2.5, marker=markers[i % len(markers)])

        plt.xlabel('L1 Block Size (Powers of 2)', fontsize=12, fontweight='bold')
        plt.ylabel('L1 Miss Rate', fontsize=12, fontweight='bold')
        plt.title('L1 Miss Rate vs L1 Block Size Power', fontsize=14, fontweight='bold')
        plt.grid(True)
        plt.legend()
        plt.tight_layout()
        
        
        all_cache_powers = sorted(df['block_size_power'].unique())
        plt.xticks(all_cache_powers, [f'$2^{{{p}}}$' for p in all_cache_powers])

        plt.savefig('./graphs/miss_rate_vs_block_size.png', dpi=300)
        print("Image saved in ./graphs/miss_rate_vs_block_size.png")
    

if __name__ == "__main__":

    print("Visualize which results?")
    print("1. Miss rate vs Cache Size (Varying associativites) - miss_rate_vs_cache_size.csv")
    print("2. Miss rate vs Block Size (Varying cache sizes) - miss_rate_vs_block_size.csv")
    result = int(input())

    visualize_cache_results(result)
