# AED2 — Optimal AED Placement Using Spatial Weighting and Genetic Algorithm

![Python 3.10+](https://img.shields.io/badge/Python-3.10%2B-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-green)
[![CANS Lab](https://img.shields.io/badge/CANS_Lab-Homepage-orange)](https://canslab1.github.io/)

A geocomputational genetic algorithm for optimizing the placement of automated external defibrillators (AEDs) across 7-Eleven convenience stores in Taipei, Taiwan, maximizing out-of-hospital cardiac arrest (OHCA) patient survival potential within an 8-minute response window.

## Overview

Heart disease is one of Taiwan's top three causes of death. Out-of-hospital cardiac arrest (OHCA) patients require immediate treatment, and quick access to automated external defibrillators (AEDs) is a major factor contributing to higher survival rates.

This project implements a **genetic algorithm (GA)** with spatial weighting to solve the NP-complete set-cover problem of selecting optimal AED locations. Using 2010 Emergency Medical Service (EMS) reports from Taipei (1,625 OHCA cases), the algorithm evaluates 677 7-Eleven convenience stores as candidate AED deployment sites, considering:

- **Spatial coverage**: Distance-decay weighting — closer AEDs yield higher survival probability
- **Grid-based analysis**: 45m × 45m grid overlay on Taipei for efficient spatial computation
- **Fire station filtering**: OHCA cases within 300m of existing fire stations are pre-filtered (already covered by EMS)
- **Multi-objective fitness**: Balances expected recovery rate, grid coverage, and incident coverage

The GA selects **100 optimal stores** that cover **53% of all OHCA cases** within the 8-minute constraint. One-sixth of the total deployment cost covers 80% of cases — a significant improvement over existing EMS/AED policies.

## Features

- **Genetic algorithm optimization** — Selects 100 optimal AED locations from 677 candidate stores using evolutionary computation.
- **Spatial weighting** — Distance-decay model accounts for decreasing survival probability with increasing AED distance.
- **Fire station filtering** — Pre-filters OHCA cases within 300m of existing fire stations to avoid redundant coverage.
- **Multi-objective fitness** — Balances expected recovery rate, grid coverage, and incident coverage simultaneously.
- **Adaptive mutation (stirring)** — Automatically increases mutation rate during stagnation to escape local optima.
- **Robustness analysis** — Consistency validated across 50 independent GA runs (49 core stores appear in all top results).
- **Jupyter notebooks** — Pre-built visualization notebooks for reproducing all publication figures.

## Installation

### C++ Genetic Algorithm

- C++ compiler with C++11 support (g++, clang++)
- Standard library only — no external dependencies

### Jupyter Notebooks (Visualization)

- Python 3.10+
- pandas
- numpy
- scipy
- geopandas (for Taipei administrative boundary shapefiles)
- matplotlib
- matplotlib-scalebar
- seaborn

## Usage

### Compile and Run the Genetic Algorithm

```bash
g++ -O2 -o aed_ga 基因演算法_不分區模式_20130313.cpp
./aed_ga
```

The program reads six input files from the current directory:

```
99_x.txt, 99_y.txt       → OHCA patient coordinates
711_x.txt, 711_y.txt     → 7-Eleven store coordinates
Fire_x.txt, Fire_y.txt   → Fire station coordinates
```

Execution proceeds through three phases:
1. **Data preprocessing** — Filter OHCA cases near fire stations, map all locations to 45m grid cells
2. **GA evolution** — 400 chromosomes × 10,000 generations with selection, crossover, and mutation
3. **Output** — Write results to `file_1.txt` through `file_10.txt`

### Run Jupyter Notebooks

```bash
pip install pandas geopandas matplotlib matplotlib-scalebar seaborn
jupyter notebook
```

Open any `.ipynb` file to reproduce the publication figures.

## Key Results

| Metric | GA (Ours) | TWM Model | SWM Model |
|--------|-----------|-----------|-----------|
| Expected Recovery Rate | 0.5754 | 0.4672 | 0.4908 |
| Grid Coverage | 0.9754 | 0.9463 | 0.9106 |
| Incident Coverage | 0.5470 | 0.4876 | 0.4813 |

Robustness analysis across 50 independent GA runs shows 49 core stores consistently selected in all top results.

## Algorithm

### Chromosome Encoding

Each chromosome encodes a candidate solution: **100 store indices** (selected from 677 stores) plus **3 binary bits** encoding the grid expansion level (1–8 layers).

### Fitness Function

The fitness function is a product of three objectives:

```
fitness = (phenotype / limit) × (grid_coverage / max_grids) × (OHCA_covered / total_patients)
```

| Component | Description |
|-----------|-------------|
| `phenotype / limit` | Average expected recovery weight across selected stores |
| `grid_coverage / max_grids` | Proportion of grid cells covered by the selected stores |
| `OHCA_covered / total_patients` | Proportion of OHCA incidents within service range |

### Spatial Weighting (Distance Decay)

Each store covers surrounding grid cells with distance-dependent recovery weights:

| Grid Layer | Manhattan Distance | Recovery Weight |
|------------|-------------------|-----------------|
| 0 (store cell) | 0 | 0.9 |
| 1 | 1 | 0.9 |
| 2 | 2 | 0.8 |
| 3 | 3 | 0.7 |
| ... | ... | ... |
| 8 | 8 | 0.2 |

### Genetic Operators

| Operator | Method |
|----------|--------|
| **Selection** | Elitist: top 50% survive to next generation |
| **Crossover** | Single-point crossover (rate: 0.8) with duplicate-gene handling |
| **Mutation** | Random store replacement (base rate: 0.01) |
| **Stirring** | Adaptive mutation — rate increases to 0.1 after 50+ stagnant generations; early termination after 500 stagnant generations |

### Workflow

```
Load OHCA, 7-Eleven, and fire station coordinates
  → Filter OHCA cases within 300m of fire stations
  → Create 45m × 45m grid overlay on Taipei (469 × 614 cells)
  → Map all locations to grid cells
  → Initialize population (400 random chromosomes, each selecting 100 stores)
  → For 10,000 generations:
      → Evaluate fitness (recovery weight × grid coverage × incident coverage)
      → Rank chromosomes by fitness
      → Apply stirring mechanism (adaptive mutation rate on stagnation)
      → Select top 50%, reproduce via crossover + mutation
  → Output best solution and convergence statistics
```

## Configuration

Parameters are defined as `#define` macros in the C++ source (recompilation required):

| Parameter | Default | Description |
|-----------|---------|-------------|
| `grid` | 45 | Grid cell size (meters) |
| `store` | 677 | Number of candidate 7-Eleven stores |
| `fire` | 44 | Number of fire stations |
| `patient` | 1625 | Number of OHCA cases |
| `s_distance` | 300 | Fire station service radius (meters) |
| `limit` | 100 | Number of AEDs to deploy |
| `simulation` | 1 | Number of independent GA runs |
| `generation` | 10000 | Maximum generations per run |
| `population` | 400 | Chromosome population size (must be multiple of 4) |
| `rate_selection` | 0.5 | Selection rate (top fraction preserved) |
| `rate_crossover` | 0.8 | Crossover probability |

## Data Files

### Coordinate System

All coordinates use **TWD97 (Taiwan Datum 1997)** Transverse Mercator projection (meters).

Taipei bounding box:
- X: 295,265.78 – 316,372.88
- Y: 2,761,738.50 – 2,789,379.00

### OHCA Data (`99_x.txt`, `99_y.txt`)

1,625 out-of-hospital cardiac arrest cases reported in Taipei during 2010, sourced from EMS reports. Each file contains one coordinate value per line.

### 7-Eleven Stores (`711_x.txt`, `711_y.txt`)

677 7-Eleven convenience store locations in Taipei. These serve as candidate AED installation sites due to their high density, 24-hour operation, and accessibility.

### Fire Stations (`Fire_x.txt`, `Fire_y.txt`)

44 fire station locations in Taipei. OHCA cases within 300m of a fire station are pre-filtered from the optimization, as they are already within baseline EMS response range.

## Project Structure

```
AED2/
├── 基因演算法_不分區模式_20130313.cpp    # C++ genetic algorithm (689 lines)
├── 99_x.txt, 99_y.txt                 # OHCA patient coordinates (TWD97)
├── 711_x.txt, 711_y.txt               # 7-Eleven store coordinates (TWD97)
├── Fire_x.txt, Fire_y.txt             # Fire station coordinates (TWD97)
├── vip_best.txt                        # Best 100 selected store indices (GA output)
├── fig_02-03-09-15.ipynb               # Spatial visualization notebook
├── fig_08-11-14.ipynb                  # Performance metrics notebook
├── check_similararity_seven.ipynb      # Robustness analysis notebook
├── newfig_distribution.ipynb           # Kernel density estimation notebook
├── prep_stats_newFigure_08.ipynb       # Statistical preparation notebook
├── newFigure_*.png                     # Pre-generated result images
└── LICENSE                             # MIT License
```

## Output Files

### GA Output (10 files per run)

| File | Content |
|------|---------|
| `file_1.txt` | Best phenotype (recovery weight) per generation |
| `file_2.txt` | Average fitness per generation |
| `file_3.txt` | Best fitness per generation |
| `file_4.txt` | Selected store indices (sorted) + grid level bits |
| `file_5.txt` | Selected store coordinates (x, y) |
| `file_6.txt` | Grid coverage count per generation |
| `file_7.txt` | OHCA coverage count per generation |
| `file_8.txt` | Per-store OHCA coverage count (best chromosome) |
| `file_9.txt` | Per-store recovery weight (best chromosome) |
| `file_10.txt` | Covered OHCA patient coordinates |

### Intermediate Files

| File | Content |
|------|---------|
| `save_patient.txt` | Filtered OHCA grid positions |
| `store_gridNumx.txt`, `store_gridNumy.txt` | Store grid mappings |
| `patient_gridNumx.txt`, `patient_gridNumy.txt` | OHCA grid mappings |
| `grid_patientNum.txt` | OHCA count per grid cell |
| `patient_saveDatax.txt`, `patient_saveDatay.txt` | Filtered OHCA coordinates |

## Authors

- **Chung-Yuan Huang** (黃崇源) — Department of Computer Science and Information Engineering, Chang Gung University, Taiwan (gscott@mail.cgu.edu.tw)
- **Tzai-Hung Wen** — Department of Geography, National Taiwan University, Taiwan

## Citation

If you use this software in your research, please cite:

> Huang, C.-Y. and Wen, T.-H. (2014). Optimal Installation Locations for Automated External Defibrillators in Taipei 7-Eleven Stores: Using GIS and a Genetic Algorithm with a New Stirring Operator. *Computational and Mathematical Methods in Medicine*, 2014, 241435. https://doi.org/10.1155/2014/241435

See `CITATION.cff` for machine-readable citation metadata.

## References

1. Tsai, Y.-S., Ko, P. C.-I., Huang, C.-Y., and Wen, T.-H. (2012). Optimizing locations for the installation of automated external defibrillators (AEDs) in urban public streets through the use of spatial and temporal weighting schemes. *Applied Geography*, 35(1–2), 394–404. https://doi.org/10.1016/j.apgeog.2012.09.002

2. Huang, C.-Y. and Wen, T.-H. (2014). Optimal Installation Locations for Automated External Defibrillators in Taipei 7-Eleven Stores: Using GIS and a Genetic Algorithm with a New Stirring Operator. *Computational and Mathematical Methods in Medicine*, 2014, 241435. https://doi.org/10.1155/2014/241435

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
