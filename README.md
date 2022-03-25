# v1.0.0

Cite as: J. Electrochem. Soc. 169 034524  
DOI: https://doi.org/10.1149/1945-7111/ac5d90

# bubbleCurrentDistributionFoam
It is described how to simulate secndary current distribution in electrochemical reactors with biphasic (gas-liquid) flow with the help of OpenFOAM. It is showed how to pre-process, run and post-process a basic case involving a 2D domain using the developed solver potentialMultiphaseEulerFoam and BC as codedMixed.
The proposed strategy is fully functional in OpenFOAM version 8.

# Disclaimer
This offering is not approved or endorsed by OpeFOAM Foundation, producer and distributor of the OpenFOAM software via www.openfoam.org.

# Usage
In applications (A) you will find the scripts to compile the solver and post-processing utilities in order to solve secondary current distribution in gas-liquid systems.
In tutorial (B) you will find an example of a 2D parallel-plate cell. 

# #  A) Applications
**1.**  Solver  
_A)_ Paste applications/utilities/solvers/potentialMultiphaseEulerFoam inside OpenFOAM user directory (Applications/Utilities/Solvers).  
_B)_ Open a terminal inside potentialMultiphaseEulerFoam.  
_C)_ Run wmake.  
**2.**  Total current density  
_A)_ Paste applications/utilities/postProcessing/potentialWallFlux inside OpenFOAM user directory (Applications/Utilities/postProcessing).  
_B)_ Open a terminal inside potentialWallFlux.  
_C)_ Run wmake.  
**3.**  Total averaged (over time) current density  
_A)_ Paste applications/utilities/postProcessing/potentialWallFluxAverage inside OpenFOAM user directory (Applications/Utilities/postProcessing).  
_B)_ Open a terminal inside potentialWallFluxAverage.  
_C)_ Run wmake.  


# #  B) Tutorial
**1-** Paste tutorial inside OpenFOAM user directory (Run/Tutorials).  
**2-** Enter to tutorial and open a Terminal.  
**3-** Modify transport properties (conductivity, kinetic parameters, equilibrium potentials, etc.) inside constant/transportProperties.  
**4-** Modify control properties (kind of electric control, under-over relaxation parameter, maxPot, tolerances) inside constant/controlProperties.   
**5-** Modify phase properties (population balance, surface tension and interfacial forces) inside constant/phaseProperties.   
**5-** Run ./Allrun.    

