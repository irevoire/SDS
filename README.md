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

Pour générer la couverture de code on peut lancer `make test`.
Ensuite en on peut ouvrir `coverage/index.html` pour accèder au rapport graphique de la couverture de code.


Ajout des flags suivants:
* CFLAGS := --coverage -O0
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
### Tests d'intégration

Les tests d'intégration génère un checksum du résultat de la commande `spcheck -i image -d` puis le compare a un checksum d'une version du programme qui est censé fonctionner.

Avec `make checksum` on peut regénérer les checksums de toutes les images stif.
Avec `make integration` on peut générer des checksums avec la nouvelle version du programme et les comparer aux dernier checksums qui ont été calculé.

 
## Comportement suspect

* Dans la fonction `read_stif_block`, si un bug apparait dans le second check d'erreur nous avons déjà incrémenté la variable `bytes_read`
