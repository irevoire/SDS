### Kaleba Sophie
### Campistron Thomas

# SDS TP stif

## Tests

### Tests unitaires

Utilisation de la librairie check (ajout de flags à la compilation)
Le fichier de tests se trouve dans le répertoire SDS/tests.

### Fuzzing

### Couverture de code

100% de couverture \\o/

Utilisation des librairies gcov et lcov.
Ajout des flags suivants:
* CDFLAGS := --coverage -O0
* LDFLAGS := -lgcov
Commandes à saisir pour obtenir un rapport graphique:
```bash
make
bin/check_main 
cd src
gcov stif.c
lcov -d . -c -o report.info
genhtml -o ./result report.info 
firefox result/index.html 
```

 
## Comportement suspect

* Dans la fonction `read_stif_block`, si un bug apparait dans le second check d'erreur nous avons déjà incrémenté la variable `bytes_read`
