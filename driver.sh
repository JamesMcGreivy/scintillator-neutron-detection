while IFS=, read -r ptcl energy unit num; do
  # do something... Don't forget to skip the header line!
  echo $ptcl $energy $unit $num
done < input.txt