/**
 * Kostra programu pro 2. projekt IZP 2022/23
 *
 * Jednoducha shlukova analyza: 2D nejblizsi soused.
 * Single linkage
 */
#define _POSIX_SOURCE // needed for kill (needed to terminate in void func)
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <string.h> // strcmp
#include <unistd.h> // getpid
#include <signal.h> // sigkill
#include "cluster.h" // headers
/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *	 a) pri prekladu argumentem prekladaci -DNDEBUG
 *	 b) v souboru (na radek pred #include <assert.h>
 *		#define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *	 struct obj_t - struktura objektu: identifikator a souradnice
 *	 struct cluster_t - shluk objektu:
 *		pocet objektu ve shluku,
 *		kapacita shluku (pocet objektu, pro ktere je rezervovano
 *			misto v poli),
 *		ukazatel na pole shluku.
 */

struct obj_t {
	int id;
	float x;
	float y;
};

struct cluster_t {
	int size;
	int capacity;
	struct obj_t* obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH 'TODO'
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap)
{
	assert(c != NULL);
	assert(cap >= 0);

	struct obj_t *temp = NULL;

	// If the cluster should have some objects, allocate memory for them
	if(cap > 0)
		temp = malloc(sizeof(struct obj_t) * cap);

	// If the allocation was successful, create the cluster
	if(temp != NULL || cap == 0)
	{
		c->obj = temp;
		c->size = 0;
		c->capacity = cap;
	}
	// Otherwise report this incident to stderr and commit suicide
	else
	{
		fprintf(stderr, "Failed to allocate memory for cluster\n");
		kill(getpid(), SIGKILL);
	}
}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
	// If the cluster has objects erase all its objects
	if(c->obj != NULL)
		free(c->obj);
	// Now initialize a new empty cluster in its place
	init_cluster(c, 0);
}

// Frees all clusters in the array and their objects
void free_clusters(struct cluster_t **arr, long int length)
{
	// Free all the memory used by the clusters objects
	for(int i = 0; i < length; i++)
	{
		clear_cluster(*arr + i);
	}

	// Free the memory used by the array of clusters (the clusters themselves)
	free(*arr);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
	// TUTO FUNKCI NEMENTE
	assert(c);
	assert(c->capacity >= 0);
	assert(new_cap >= 0);

	if (c->capacity >= new_cap)
		return c;

	size_t size = sizeof(struct obj_t) * new_cap;

	void *arr = realloc(c->obj, size);
	if (arr == NULL)
		return NULL;

	c->obj = (struct obj_t*)arr;
	c->capacity = new_cap;
	return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
	// cluster je plny, zvetsit o cluster_chunk
	if(c->size == c->capacity)
		c = resize_cluster(c, c->size + CLUSTER_CHUNK);

	// prida objekt do clusteru
	*(c->obj + c->size) = obj;
	c->size++;
}

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1 != NULL);
	assert(c2 != NULL);

	// Go through all the objects in the second cluster
	for(int i = 0; i < c2->size; i++)
	{
		// And add them to the first cluster
		append_cluster(c1, *(c2->obj+i));
	}

	// Now sort all the objects in the first cluster
	sort_cluster(c1);
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
	assert(idx < narr);
	assert(narr > 0);

	// Free the resources used by the specified cluster
	clear_cluster(carr + idx);

	// Shift all the clusters that were behind the specified index by one,
	// as to fill the empty space
	for(int i = idx; i < narr - 1; i++)
	{
		*(carr + i) = *(carr + i + 1);
	}

	return narr - 1;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
	assert(o1 != NULL);
	assert(o2 != NULL);

	return sqrt(pow(o1->x - o2->x, 2) + pow(o1->y - o2->y, 2));
}

/*
 Pocita vzdalenost dvou shluku.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
	assert(c1 != NULL);
	assert(c1->size > 0);
	assert(c2 != NULL);
	assert(c2->size > 0);

	// Smallest known distance between objects
	float smallestDistance = obj_distance(c1->obj, c2->obj);

	// Go through all the objects in the first cluster
	for(int i = 0; i < c1->size; i++)
	{
		// Go through all the objects in the second cluster
		for(int j = 0; j < c2->size; j++)
		{
			// get the distance between the objects
			float distance = obj_distance(c1->obj + i, c2->obj + j);

			// If it is smaller that the smallest known distance,
			// update the smallest known distance
			if(smallestDistance > distance)
				smallestDistance = distance;
		}
	}

	return smallestDistance;
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
	assert(narr > 1);

	// Smallest known distance between clusters
	float smallest_distance = cluster_distance(carr, carr+1);

	// Go through all the clusters in the array
	for(int i = 0; i < narr - 1; i++)
	{
		// Go through all the other clusters that have not been compared
		// to the i cluster yet.
		for(int j = i + 1; j < narr; j++)
		{
			// Get the distance between the two clusters
			float distance = cluster_distance(carr + i, carr + j);

			// If the distance is smaller than the smallest known distance
			if(distance <= smallest_distance)
			{
				// Update the smallest known distance
				smallest_distance = distance;
				// And save the indicees of the clusters
				*c1 = i;
				*c2 = j;
			}
		}
	}
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
	// TUTO FUNKCI NEMENTE
	const struct obj_t *o1 = (const struct obj_t *)a;
	const struct obj_t *o2 = (const struct obj_t *)b;
	if (o1->id < o2->id) return -1;
	if (o1->id > o2->id) return 1;
	return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
	qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
	// TUTO FUNKCI NEMENTE
	for (int i = 0; i < c->size; i++)
	{
		if (i) putchar(' ');
		printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
	}
	putchar('\n');
}

/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
	assert(arr != NULL);

	int i;						// Iterator variable / number of loaded objects.
	FILE *vstup;				// File from which to load cluster information.
	char head_prefix[7];		// count= string at the start of the file, used
								// only to check the validity of the header.
	int head_loaded;			// Result of reading the header.
	int maxNumOfLines;			// Maximum number of lines allowed to be read
								// from the file.
 	struct cluster_t* clusters;	// Array into which to read clusters (basically
								// an alias for *arr.)

	// Open the file for reading.
	vstup = fopen(filename, "r");

	// Check for failure to open file.
	if(vstup == NULL)
	{
		fprintf(stderr, "Failed to open the input file.\n");
		fclose(vstup);
		return -1;
	}

	// Read the header.
	head_loaded = fscanf(vstup, "%6s%d\n", head_prefix, &maxNumOfLines);

	// Error reading, incorrect header format.
	if(head_loaded < 2 || head_loaded == EOF)
	{
		fprintf(stderr, "Failed to read header. Please make sure the header is "
				"formatted as follows: count=<number of rows>\n");
		fclose(vstup);
		return -1;
	}
	// The start of the file is not the expected string "count="
	else if(strcmp(head_prefix, "count=") != 0)
	{
		fprintf(stderr, "%s is not a valid header prefix. Header has to be in "
				"the format of count=<number of rows>\n", head_prefix);
		fclose(vstup);
		return -1;
	}
	// Invalid number for max number of lines
	else if(maxNumOfLines < 1)
	{
		fprintf(stderr, "%i is not a valid number of rows. Number of rows has "
				"to be a natural number.\n", maxNumOfLines);
		fclose(vstup);
		return -1;
	}

	// Allocate enough memory to be able to store all clusters to be read
	clusters = malloc(sizeof(struct cluster_t) * maxNumOfLines);

	// Memory allocation failed
	if(clusters == NULL)
	{
		fprintf(stderr, "Could not allocate memory for array of clusters.\n");
		fclose(vstup);
		return -1;
	}

	// point **arr to clusters
	*arr = clusters;

	// Read the clusters.
	for(i = 0; i < maxNumOfLines; i++)
	{
		int obj_loaded;				// Result of reading an object
		int id;						// Id of a currently read object.
		float x, y;					// x and y of a currently read object.
		char lineEnd;				// Checks to see whether object line ended
									// properly
		struct cluster_t* cluster;	// Cluster being worked on
		struct obj_t object;		// Object being loaded

		obj_loaded = fscanf(vstup, "%i %f %f%c", &id, &x, &y, &lineEnd);

		// ===Check validity of values===
		// EOF, quit
		if(obj_loaded == EOF)
		{
			fprintf(stderr, "Expected %i objects, however file contains only "
					"%i.\n", maxNumOfLines, i+1);
			free_clusters(arr, i);
			fclose(vstup);
			return -1;
		}

		// Tells us on what part of the clusters array we should work
		cluster = clusters + i;

		// Error reading, incorrect header format.
		if(obj_loaded < 3)
		{
			fprintf(stderr, "Failed to read object.\n");
			free_clusters(arr, i);
			fclose(vstup);
			return -1;
		}
		// Line does not end after last float
		if(lineEnd != 10)
		{
			fprintf(stderr, "Line does not end after object.\n"
					"Instead ends in %i", lineEnd);
			free_clusters(arr, i);
			fclose(vstup);
			return -1;
		}

		// Check uniqueness of ID
		for(int j = 0; j < i; j++)
		{
			if((clusters + j)->obj->id == id)
			{
				fprintf(stderr, "All IDs must be unique.\n");
				free_clusters(arr, i);
				fclose(vstup);
				return -1;
			}
		}

		// Check whether x and y are whole numbers
		if(x != ceilf(x) || y != ceilf(y))
		{
				fprintf(stderr, "X and Y components have to be integers.\n");
				free_clusters(arr, i);
				fclose(vstup);
				return -1;
		}

		// Check whether x and y are in the range <0;1000>
		if(x < 0 || x > 1000 || y < 0 || y > 1000)
		{
				fprintf(stderr, "X and Y components have to be in the range "
						"<0;1000>.\n");
				free_clusters(arr, i);
				fclose(vstup);
				return -1;
		}
		// ===End of checking===

		init_cluster(cluster, 1);

		// Loads the read info into the object
		object.id = id;
		object.x = x;
		object.y = y;

		append_cluster(cluster, object);
	}

	fclose(vstup);

	return i;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
	printf("Clusters:\n");
	for (int i = 0; i < narr; i++)
	{
		printf("cluster %d: ", i);
		print_cluster(carr + i);
	}
}

int main(int argc, char *argv[])
{
	struct cluster_t *clusters;				// Array of clusters
	int loadedClusters, desiredClusters;	// Number of loaded and desired
											// clusters

	// ===Read arguments===
	// Only file provided, assume desired number of clusters is 1
	if(argc == 2)
	{
		desiredClusters = 1;
	}
	// User also provided desired number of clusters
	else if(argc == 3)
	{
		char *pEnd;	// Points to the end of a converted long int.

		// Reads and converts the second argument
		desiredClusters = strtol(argv[2], &pEnd, 10);

		// Failed reading the int
		if(pEnd == argv[2] || *pEnd != '\0')
		{
			fprintf(stderr, "Failed to read second argument.\n");
			return -1;
		}
		// Invalid value
		else if(desiredClusters <= 0)
		{
			fprintf(stderr, "Number of desired clusters has to be a natural "
					"number.\n");
			return -1;
		}
	}
	else
	{
		fprintf(stderr, "Incorrect number of arguments\n");
		return -1;
	}
	// ===End of reading arguments===

	loadedClusters = load_clusters(argv[1], &clusters);

	// Some fail occured while loading the file
	if(loadedClusters == -1)
	{
		fprintf(stderr, "Could not load clusters\n");
		return -1;
	}

	// User wants us to split cluster into more clusters than we read
	if(desiredClusters > loadedClusters)
	{
		fprintf(stderr, "Number of desired clusters has to be less than "
				"the number of loaded numbers.\n");
		free_clusters(&clusters, loadedClusters);
		return -1;
	}

	// ===Link the clusters===
	while(loadedClusters != desiredClusters)
	{
		int index1, index2;	// Indexes of the closest clusters in the array

		// First find two closest clusters
		find_neighbours(clusters, loadedClusters, &index1, &index2);

		// Merge the second into the first
		merge_clusters(&clusters[index1], &clusters[index2]);

		// And finaly remove the second
		loadedClusters = remove_cluster(clusters, loadedClusters, index2);
	}
	// ===End of linking the clusters===

	print_clusters(clusters, loadedClusters);

	free_clusters(&clusters, loadedClusters);
}

