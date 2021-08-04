while IFS=, read -r ptcl energy unit num; do
  EJ309-build/EJ309 $ptcl $energy $unit $num
  python3 G4DataToLY/G4DataToLY.py $ptcl $energy $unit
done < input.txt