/* lab6.c template
 * Prof. Russell  << replace with your name!
 * harlanr        << replace
 * ECE 2220, Fall 2016
 * MP6
 *
 * Purpose: ?
 *
 * Assumptions: ?
 *
 * Command line arguments: ?
 *   
 * Known bugs: ?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define NUMINTS 4
#define NUMLTRS 8
struct Record {
    int id;
    float value;
    int a[NUMINTS];
    char name[NUMLTRS];
};

/* This function verifies that the list is sorted.
 * It also checks that the id's are all present
 *
 * The function produces no output if the list is correct.  It
 * causes the program to terminate and print a line beginning with 
 * "Assertion failed:" if an error is detected.
 *
 * The checks are not exhaustive, so an error may still exist in the
 * list even if these checks pass.
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void validate_list(struct Record *ptr, int records, 
        int (*fcomp)(const void *, const void *))
{
    int i;
    int * id_list = (int *)calloc(records, sizeof(int));
    if (id_list == NULL) {
        fprintf(stderr, "lab6: Cannot malloc array of size %d\n", records);
        exit(1);
    }
    for(i = 0; i < records - 1; i++) {
        assert(fcomp(ptr+i, ptr+i+1) <= 0);
        assert(ptr[i].id > 0 && ptr[i].id <= records);
        id_list[ptr[i].id-1] = 1;
    }
    assert(ptr[i].id > 0 && ptr[i].id <= records);
    id_list[ptr[i].id-1] = 1;
    for(i = 0; i < records; i++)
        assert(id_list[i] == 1);
    free(id_list);
}

/* turn the binary data into formated ASCII text and send to sdtout
 *
 * This function is complete.  
 *
 * YOU MUST NOT CHANGE THIS FUNCTION.  WE USE IT DURING GRADING TO VERIFY THAT
 * YOUR LIST IS CONSISTENT.
 */
void print_records(struct Record *S, int nrecs)
{
    int i, j;
    for (i = 0; i < nrecs; i++)
    {
        printf("\nS[%d]->id = %d\n", i, S[i].id);
        printf("S[%d]->value = %g\n", i, S[i].value);
        printf("S[%d]->a = %d", i, S[i].a[0]);
        for (j = 1; j < NUMINTS; j++) 
        { 
            printf(", %d", S[i].a[j]);
        }
        printf("\nS[%d]->name = %s\n", i, S[i].name);
    }
}

/* Your bubble sort algorithm goes here
 *
 * The arguments must be exactly as defined here.
 *
 * Input:
 *    ptr:     points to the start of the array
 *    records: the number of elements in the array
 *    fcomp:   function pointer to comparison function
 *
 * Assumptions: 
 *    the function sorts an array of type struct Record.
 *    
 *    The comparison function must return an integer less than, equal to,
 *    or greater than zero if the first argument should appear in the list
 *    respectively before, no change, or after the second.  
 */
void bubblesort(struct Record *ptr, int records, 
        int (*fcomp)(const void *, const void *))
{
}

/* An example of a function to compare records for use with qsort
 * or bubble sort.
 *
 * The function returns one of three values:
 *    -1 : record a should be before record b
 *     0 : the two records have the same ranking
 *     1 : record a should be after record b
 */
int IDCompare(const void *a, const void *b)
{ 
    struct Record *sa, *sb;

    sa = (struct Record *) a;
    sb = (struct Record *) b;

    if (sa->id < sb->id) return -1;
    else if (sa->id > sb->id) return 1;
    else return 0;
}


/*----------------------------------------------------------*/
int main(int argc, char *argv[])
{ 
    int sort_field = 0;
    int sort_type  = 1;
    clock_t start, end;

    if (argc != 2 && argc != 3) { 
        fprintf(stderr, "Usage: lab6 sort-field [sort-type]\n");
        fprintf(stderr, "Sort-field\n");
        fprintf(stderr, "  1   id\n");
        fprintf(stderr, "  2   value\n");
        fprintf(stderr, "  3   array\n");
        fprintf(stderr, "  4   name\n");
        fprintf(stderr, "Sort-type\n");
        fprintf(stderr, "  0   do not sort just print data\n");
        fprintf(stderr, "  1   qsort (default)\n");
        fprintf(stderr, "  2   bubble-sort\n");

        exit(1);
    }
    sort_field = atoi(argv[1]);
    if (sort_field < 1 || sort_field > 4) {
        fprintf(stderr, "lab6: Invalid sort field %d\n", sort_field);
        exit(1);
    }
    if (argc == 3) {
        sort_type = atoi(argv[2]);
        if (sort_type < 0 || sort_type > 2) {
            fprintf(stderr, "lab6: Invalid sort type %d\n", sort_type);
            exit(1);
        }
    }

    int num_records = -1;
    int num_items = -1;
    struct Record *rec_ptr;

    num_items = fread(&num_records, sizeof(num_records), 1, stdin);
    if (num_items != 1) {
        fprintf(stderr, "lab6: Error fread failed to read number records %d\n", num_items);
        exit(1);
    }
    if (num_records < 2) {
        fprintf(stderr, "lab6: Error invalid number records %d\n", num_records);
        exit(1);
    }
    printf("lab6: num records %d\n", num_records);

    /* Notice this is the same approach we used in MP5 except here
     * we can read all the the records with one call to fread.
     */
    rec_ptr = (struct Record *) malloc(num_records * sizeof(struct Record));
    num_items = fread(rec_ptr, sizeof(struct Record), num_records, stdin);
    if (num_items != num_records) {
        fprintf(stderr, "lab6: failed to read all records %d\n", num_items);
        exit(1);
    }

    // The code in main from the beginning until here is complete
    // Your changes start below

    if (sort_type == 1)   // qsort
    {
        // this is the correct way to time qsort
        start = clock();
        qsort(rec_ptr, num_records, sizeof(struct Record), IDCompare);
        end = clock();

        // you must print using exactly this format
        fprintf(stderr, "lab6: qsort time=%g for %d records\n", 
                1000*((double)(end-start))/CLOCKS_PER_SEC, num_records);

        // after sorting you must validate that the list is sorted
        validate_list(rec_ptr, num_records, IDCompare);
    }
    if (sort_type == 2)   // bubble sort
    {
        start = clock();
        bubblesort(rec_ptr, num_records, IDCompare);
        end = clock();

        fprintf(stderr, "lab6: bubble sort time=%g for %d records\n", 
                1000*((double)(end-start))/CLOCKS_PER_SEC, num_records);

        validate_list(rec_ptr, num_records, IDCompare);
    }

    // printing the records is required.  If you don't want to save
    // the output, on the shell redirect stdout to /dev/null
    print_records(rec_ptr, num_records);

    free(rec_ptr);

    return EXIT_SUCCESS;
}
