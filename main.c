#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	int *bit_map;		// set to be size of 'groupSize'
	char **strings;		// supposed to be vector
} group;


typedef struct
{
	group *groups;	// dynamicaly allocated array of structs
	int num_groups;	// number of groups
	int array_len;	// logical length of the full s_parse_string_array_t
	int group_size;	// number of strings managed by each group	
} s_parse_string_array_t;


void SSA_new(s_parse_string_array_t *ssa, 
	     int array_length,
	     int group_size);
int SSA_insert(s_parse_string_array_t *ssa,
               int index, 
	       const char *str);
void SSA_map(s_parse_string_array_t *ssa,
	     void (*mapfn)(),
	     void* aux_data);
void SSA_dispose(s_parse_string_array_t *ssa);


static void count_empty_print_not_empty(int index, 
					char *str, 
					void *aux_data);


int main(int argc, char *argv[])
{
	s_parse_string_array_t ssa;
	SSA_new(&ssa, 7000, 35);

	SSA_insert(&ssa, 331, "need");
	SSA_insert(&ssa, 589, "more");
	SSA_insert(&ssa, 330, "Eye");
	SSA_insert(&ssa, 330, "I");
	SSA_insert(&ssa, 679, "cowbell");

	int num_empty_strings = 0;
	SSA_map(&ssa, count_empty_print_not_empty, &num_empty_strings);

	SSA_dispose(&ssa);

	return 0;
}

void SSA_new(s_parse_string_array_t *ssa,
	     int array_length,
	     int group_size)
{
	ssa->num_groups = (array_length  - 1) / group_size + 1;
	ssa->group_size = group_size;
	ssa->array_len = array_length;
	ssa->groups = malloc(sizeof(group) * ssa->num_groups);
	
	// It might be not right at all because of the fact that
	// I call malloc to create the 'groups' to often, because of that
	// there may occur memory fragmentation.
	int i, v;
	for (i = 0; i < ssa->num_groups; i++)
	{
		ssa->groups[i].strings = (char*)malloc(sizeof(char*) * group_size);
		ssa->groups[i].bit_map = malloc(sizeof(int) * group_size);
	}
	
	for (i = 0; i < ssa->num_groups; i++)
	{
		for (v = 0; v < ssa->group_size; v++)
		{
			ssa->groups[i].bit_map[v] = 0;
			ssa->groups[i].strings[v] = "";
		}
	}
}

int SSA_insert(s_parse_string_array_t *ssa,
	       int index,
               const char *str)
{
	int group_index = (index / ssa->group_size);
	int string_index_bitmap = (index % ssa->group_size);
	int index_within_array = 0;

	int i;
	for (i = 0; i < string_index_bitmap; i++)
		if (ssa->groups[group_index].bit_map[i])
			index_within_array++;
	const char* copy = strdup(str);
	int previously_inserted = ssa->groups[group_index].bit_map[string_index_bitmap];
	if (previously_inserted)
	{
		strcpy(ssa->groups[group_index].strings[string_index_bitmap], copy);
	}
	else
	{
		ssa->groups[group_index].strings[string_index_bitmap] = copy;
	}
	return !previously_inserted;
}

void SSA_map(s_parse_string_array_t *ssa,
			 void(*mapfn)(),
			 void *aux_data)
{
	int i, v;

	for (i = 0; i < ssa->num_groups; i++)
	{
		for (v = 0; v < ssa->group_size; v++)
			mapfn(ssa->group_size * i + v, ssa->groups[i].strings[v], aux_data);
	}

	printf("Number of empty strings is %d", *((int*)(aux_data)));
}

void SSA_dispose(s_parse_string_array_t* ssa)
{
	int i;

	for (i = 0; i < ssa->num_groups; i++)
	{
		free(ssa->groups[i].strings);
		free(ssa->groups[i].bit_map);
	}
	free(ssa->groups);
}

static void count_empty_print_not_empty(int index,
					char *str,
					void *aux_data)
{
	if (strcmp(str, "") != 0)
	{
		printf("Oooo! Noneempty string at index %d: \"%s\"\n", index, str);
	}
	else
	{
		(*(int*)(aux_data))++;
	}
}
