# LIMiT (Line Intensity Mapping Tool)
This tool simulates galaxy-line intensity maps from a given halo/galaxy catalog.

## Prerequisites
"g++" compiler is required to compile the code. Any other compiler can be used, requiring the makefile to be altered accordingly.

## Format of halo catalogs

The halo catalogs should be in the form of a directory that holds separate files for the following quantities of each halo:\
**Common for all lines:** Mass.bin (halo mass in $10^{10}h^{-1}M_\odot$ units), Pos_x.bin, Pos_y.bin, Pos_z.bin (components of halo position in grid units)\
**21-cm line:** H_mass.bin (total hydrogen mass (all phases) in $10^{10}h^{-1}M_\odot$ units)\
**CII and CO lines:** SFR.bin (star-formation rate in $M_\odot/yr$), Metallicity.bin (gas metallicity in $Z_\odot$ units)\
Additionally, there should be a file named "num_halos.txt", which should contain a single integer number which is the number of halos in the files.

## Intensity mapping models
<!-- Inside the "src" directory, the source file "sfr_buffered.cc" contains the default model of star-formation rate for a given halo mass (Eq. (26) of [Silva et al. 2013](https://doi.org/10.1088/0004-637X/763/2/132)). A different model of star-formation rate can be included and used in the source file, "process_buffered.cc". The source file, "lum_buffered.cc", converts SFR to line-luminosities using the pre-defined functions. New models can be added and used in "process_buffered.cc". -->
The CO luminosities are calculated using prescriptions given in [Li et al., 2016](https://doi.org/10.3847/0004-637X/817/2/169), [Keating et al., 2020](https://doi.org/https://iopscience.iop.org/article/10.3847/1538-4357/abb08e), or [Yang et al., 2022](https://doi.org/10.3847/1538-4357/ac5d57).\
The CII luminosities follow [Lagache et al., 2018](https://doi.org/10.1051/0004-6361/201732019) or [Romano et al., 2022](https://doi.org/10.1051/0004-6361/202142265).\
The Hydrogen masses are gridded into an HI density map, which is scaled to a brightness temperature map using either [Wolz et al., 2017](https://doi.org/10.1093/mnras/stx1388) or [Villaescusa-Navarro et al., 2018](https://doi.org/10.3847/1538-4357/aadba0).

## How to use
```
$ make
```
compiles the code and generates an executable "LIM_simulator". This executable needs two text files as inputs to function. The first one (e.g. "param.txt") contains the necessary parameters for the simulation. An example file named "param.txt" is given here, with the meaning of the parameters explained inside. The second file (e.g. "paths_CO.txt") should contain the paths for the halo catalogs, redshifts and names of the output files aranged in seperate columns. For example, this might look like:
```
subhalocat-z3.010 3.010 CO_map_z3.010.bin
subhalocat-z2.000 2.000 CO_map_z2.000.bin
subhalocat-z1.500 1.500 CO_map_z1.500.bin
```
Here, the first column represents the directory that holds the halo catalogs. The second column represents the redshift and the third one represents the output binary files that contain the intensity mapping cube. To run the executable, use command "./LIM_simulator param_CO.txt paths_CO.txt". The generated map can be viewed using "view_map.ipynb".

## Acknowledgements
If you are using this tool, please acknowledge the following article:  [Murmu, Majumdar and Datta, 2021, MNRAS 507, 2500–2509](https://doi.org/10.1093/mnras/stab2347)
