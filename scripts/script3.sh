#BSUB -nnodes 1
#BSUB -W 600
#BSUB -G mcgreivy
#BSUB -J Geant4
#BSUB -q pbatch


cd /usr/workspace/mcgreivy/Scintillator-Neutron-Detection/EJ309-build
source /g/g15/mcgreivy/NSSCResearch/geant4-install/bin/geant4.sh;
echo "START";
./EJ309 120 input3.txt;
echo "DONE";