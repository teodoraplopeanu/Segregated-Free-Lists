#include "about_dlls.h"

/*
 * Functie care trebuie apelata pentru alocarea si initializarea unei liste.
 * (Setare valori initiale pentru campurile specifice structurii LinkedList).
 */
dll_list_t *dll_create(unsigned int data_size)
{
	dll_list_t *list = malloc(sizeof(dll_list_t));
	DIE(!list, "Memory allocation error\n");

	list->head = NULL;
	list->size = 0;
	list->data_size = data_size;

	return list;
}

/*
 * Functia intoarce un pointer la nodul de pe pozitia n din lista.
 * Pozitiile din lista sunt indexate incepand cu 0 (i.e. primul nod din lista se
 * afla pe pozitia n=0). Daca n >= nr_noduri, atunci se intoarce nodul de pe
 * pozitia rezultata daca am "cicla" (posibil de mai multe ori) pe lista si am
 * trece de la ultimul nod, inapoi la primul si am continua de acolo. Cum putem
 * afla pozitia dorita fara sa simulam intreaga parcurgere?
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t *dll_get_nth_node(dll_list_t *list, unsigned int n)
{
	dll_node_t *current_node = list->head;

	//if (n >= list->size)
	//  return;

	for (unsigned int i = 1; i < n; i++)
		current_node = current_node->next;

	return current_node;
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se creeaza un nou nod care e
 * adaugat pe pozitia n a listei reprezentata de pointerul list. Pozitiile din
 * lista sunt indexate incepand cu 0 (i.e. primul nod din lista se afla pe
 * pozitia n=0). Cand indexam pozitiile nu "ciclam" pe lista circulara ca la
 * get, ci consideram nodurile in ordinea de la head la ultimul (adica acel nod
 * care pointeaza la head ca nod urmator in lista). Daca n >= nr_noduri, atunci
 * adaugam nodul nou la finalul listei.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
void dll_add_nth_node(dll_list_t *list, unsigned int n, long address)
{
	dll_node_t *current_node, *previous_node;

	if (!list || list->data_size == 0)
		return;

	// Trunchiez dimensiunea lui n
	if (n > list->size)
		n = list->size;

	// Allocate memory for new node
	dll_node_t *new_node = malloc(sizeof(*new_node));
	new_node->data = malloc(sizeof(sfl_info_t));

	((sfl_info_t *)new_node->data)->address = address;

	if (list->size == 0) {
		list->head = new_node;
		list->head->next = new_node;
		list->head->prev = new_node;
		list->size++;
		return;
	}

	// Get to position n
	previous_node = NULL;
	current_node = list->head;

	for (unsigned int i = 0; i < n; i++)
		current_node = current_node->next;
	previous_node = current_node->prev;

	/* Make the links */
	new_node->next = current_node;
	new_node->prev = previous_node;
	current_node->prev = new_node;
	previous_node->next = new_node;

	if (n == 0)
		list->head = new_node;

	list->size++;
}

/*
 * Elimina nodul de pe pozitia n din lista al carei pointer este trimis ca
 * parametru. Pozitiile din lista se indexeaza de la 0 (i.e. primul nod din
 * lista se afla pe pozitia n=0). Functia intoarce un pointer spre acest nod
 * proaspat eliminat din lista. Daca n >= nr_noduri - 1, se elimina nodul de la
 * finalul listei. Este responsabilitatea apelantului sa elibereze memoria
 * acestui nod.
 * Atentie: n>=0 (nu trebuie tratat cazul in care n este negativ).
 */
dll_node_t *dll_remove_nth_node(dll_list_t *list, unsigned int n)
{
	dll_node_t *current_node, *previous_node;

	if (!list || list->size == 0)
		return NULL;

	if (n >= list->size)
		n = list->size - 1;

	if (n == 0) {
		current_node = list->head;
		list->head->next->prev = list->head->prev;
		list->head->prev->next = list->head->next;
		list->head = list->head->next;
		list->size--;
		return current_node;
	}

	if (n == list->size - 1) {
		current_node = list->head->prev;
		list->head->prev = current_node->prev;
		current_node->prev->next = list->head;
		list->size--;
		return current_node;
	}

	// Parcurg lista pana la n
	previous_node = NULL;
	current_node = list->head;

	for (unsigned int i = 0; i < n; i++) {
		previous_node = current_node;
		current_node = current_node->next;
	}

	// Elimin links urile
	previous_node->next = current_node->next;
	current_node->next->prev = previous_node;

	list->size--;

	return current_node;
}

/*
 * Functia intoarce numarul de noduri din lista al carei pointer este trimis ca
 * parametru.
 */
unsigned int
dll_get_size(dll_list_t *list)
{
	return list->size;
}

/*
 * Procedura elibereaza memoria folosita de toate nodurile din lista, iar la
 * sfarsit, elibereaza memoria folosita de structura lista.
 */
void dll_free(dll_list_t **pp_list)
{
	dll_node_t *current_node;

	if (!pp_list || !*pp_list)
		return;

	while (dll_get_size(*pp_list) > 0) {
		current_node = dll_remove_nth_node(*pp_list, 0);
		//free(((sfl_info_t *)current_node->data)->data);
		free(current_node->data);
		free(current_node);
		current_node = NULL;
	}

	free(*pp_list);
	*pp_list = NULL;
}
