# Sortie
set term png;
set output 'graphique.png';
  
# Dessin de la courbe
plot 'fichier.txt' using 1:2 title 'comparaisons' with lines,  'fichier.txt' using 1:3 title 'echanges' with lines;
