#ifndef CLUSTER_H_
#define CLUSTER_H_

struct obj_t;
struct cluster_t;

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap);
// Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
void clear_cluster(struct cluster_t *c);
// Frees all clusters in the array and their objects
void free_clusters(struct cluster_t **arr, long int length);
// Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);
/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);
/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);
/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx);
// Pocita Euklidovskou vzdalenost mezi dvema objekty.
float obj_distance(struct obj_t *o1, struct obj_t *o2);
// Pocita vzdalenost dvou shluku.
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);
/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);
// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b);
// Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
void sort_cluster(struct cluster_t *c);
// Tisk shluku 'c' na stdout.
void print_cluster(struct cluster_t *c);
/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr);
/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr);

#endif // CLUSTER_H_

