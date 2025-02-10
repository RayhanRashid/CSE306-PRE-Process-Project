#include <stdio.h>
#include <stdlib.h>

int main (int argc, char* argv[]) {

    if (argc > 6) {
        printf("Invalid number of arguments: expected maximum 6 but got %d\n", argc);
        return 1;
    }

    //flags are as follows:
    //-f displays the number of records in the first field
    //-r displays the number of lines (minus one if -h is present)
    //-h causes the first line to be interpreted as a header.
    //More details can be found in the project doc.

    //-min "field name" finds the minimum value within the provided field name. If a number is provided,
    // it will check the field with that number index.
    //-max "field name" does the same with the maximum value.
    //-mean "field name" does the same with the mean.
    //-records "field" "value" displays the records from field that contain value.

    //The last element in argv[] is the file name.


    
    char* filename = argv[argc - 1];

    FILE *filepointer;
    filepointer = fopen(filename, "r");

    if (filepointer == NULL) {
        printf("Error opening file.\n");
        return 1;
    }
    //CODE GOES HERE


    printf("%s\n", filename);

    
    

    if (fclose (filepointer) == 0) {
        printf("Closed file.\n");
    } else {
        printf("Error closing file.\n");
        return 1;
    }
    
    printf("Run successful.\n");
    return 0;
}
