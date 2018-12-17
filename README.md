### Campistron Thomas
### Kaleba Sophie

# SDS TP stif

## Tests

Lancer `make all_test` pour exécuter les tests unitaires et le calcul de couverture de tests associé, ainsi que les tests d'intégration.

### Tests unitaires

Utilisation de la librairie check (ajout de flags à la compilation)
Le fichier de tests se trouve dans le répertoire SDS/tests.

### Fuzzing

Utilisation de la librairie AFL, avec exécution du fuzzing pendant 1h30. L'image donnée en input était modifiée et envoyée au visualisateur stif. 
Nous n'avons pas obtenu d'échec.

### Couverture de code

100% de couverture \\o/

Utilisation des librairies gcov et lcov.

Pour générer la couverture de code on peut lancer `make test`.
Ensuite on peut ouvrir `coverage/index.html` pour accéder au rapport graphique de la couverture de code.

### Tests d'intégration

Les tests d'intégration génèrent un checksum du résultat de la commande `spcheck -i image -d` puis le comparent à un checksum du résultat d'une version du programme qui est censé fonctionner.

Avec `make checksum` on peut regénérer les checksums de toutes les images stif.
Avec `make integration` on peut générer des checksums avec la nouvelle version du programme et les comparer aux dernier checksums qui ont été calculés.

## Propositions d'amélioration du fichier 

* Certains des types des différentes structures sont signés alors qu'ils ne devraient pas pouvoir être négatifs. Nous proposons donc de remplacer les `int32_t` de `stif_header_s.width`, `stif_header_s.height`, `stif_block_s.block_size` par des `uint32_t`.

* On pourrait remplacer les types existants pour les champs `stif_header_s.color_type` et `stif_block_s.block_type` par des champs de bits. Soit on force la taille du champ à 1 (`int block_type : 1`), soit on supprime les flags `STIF_BLOCK_TYPE_HEADER` et `STIF_BLOCK_TYPE_DATA`, ainsi que `stif_block_s.block_type`, pour les remplacer par 2 champs dans la structure `int type_header : 1` et `int type_data : 1`. 

* On pourrait faire une union anonyme entre les tableaux `grayscale_pixels` et `rgb_pixels` pour minimiser le risque d'erreur. Par exemple: 

```c
typedef struct stif_s
{
	stif_header_t    header;
	union {
		pixel_grayscale_t *grayscale_pixels;
		pixel_rgb_t       *rgb_pixels;
	};
	stif_block_t      *block_head;
} stif_t;
```
* Pour limiter le nombre de malloc et de free, on pourrait intégrer les data directement dans le `stif_block_s` plutôt que d'allouer de la mémoire pour les data séparément de la structure. Par exemple: 

```c
typedef struct stif_block_s
{
#define STIF_BLOCK_TYPE_HEADER 0
#define STIF_BLOCK_TYPE_DATA   1
	int8_t  block_type;
	int32_t block_size;
	struct stif_block_s *next;

	uint8_t data[];
} stif_block_t;

// malloc(sizeof(stif_block_t)+block_size)
```

Cette "amélioration" en plus de simplifier la gestions des allocations devrait permettre un accès plus rapide aux données des blocs.
Plus on fait d'allocation plus on augmente les chances que le programme ne soit plus capable de mettre les données suffisamment proche dans la mémoire pour charger le `stif_bloc_t` et sa data en cache dans le CPU. Utiliser une structure comme définie au dessus permet de forcer a ce qu'un bloc et sa data soient toujours sauvegardé de manière contigue dans la mémoire et devrait rendre les accès plus rapide.


## Comportement suspect

* Dans la fonction `read_stif_block`, si un bug apparait dans le second check d'erreur nous avons déjà incrémenté la variable `bytes_read`
