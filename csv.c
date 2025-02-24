#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char* argv[]) {

    if (argc > 11) {
        printf("Invalid number of arguments: expected maximum 11 but got %d\n", argc);
        return EXIT_FAILURE;
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
        return EXIT_FAILURE;
    }
    
    printf("File opened: %s\n", filename);

    //Recorded flag values.
    char* firstflag;
    char* min;
    char* max;
    char* mean;
    char* recordfield;
    char* recordvalue;
    int headerpresent = 0;
    //Flag reader.
    for (int i = 1; i < argc - 1; i++) {
        if (i == 1) {
            if (strcmp(argv[1], "-f") == 0) {
                firstflag = "f";
            } else if (strcmp(argv[1], "-fr") == 0) {
                firstflag = "fr";
            } else if (strcmp(argv[1], "-fh") == 0) {
                firstflag = "fh";
            } else if (strcmp(argv[1], "-rh") == 0) {
                firstflag = "rh";
            } else if (strcmp(argv[1], "-frh") == 0) {
                firstflag = "frh";
            } else if (strcmp(argv[1], "-r") == 0) {
                firstflag = "r";
            } else if (strcmp(argv[1], "-h") == 0) {
                firstflag = "h";
            } else {
                firstflag = "0";
            }
        }
        if (strcmp(argv[i], "-min") == 0) {
            min = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-max") == 0) {
            max = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-mean") == 0) {
            mean = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-records") == 0) {
            recordfield = argv[i + 1];
            recordvalue = argv[i + 2];
            i += 2;
        }
    }
    //Reminder to remove all debug statements before submitting!!
    printf("Flags read. Firstflag: %s, min: %s, max: %s, mean: %s, recordfield: %s, recordvalue: %s\n", firstflag, min, max, mean, recordfield, recordvalue);
    if (strchr(firstflag, 'h') != NULL) {
        headerpresent = 1;
    }
    
    if (strchr(firstflag, 'f') != NULL) {
        int fields = 1;
        char reader[512];

        fgets(reader, sizeof(reader), filepointer);
        
        for (char *c = reader; *c; c++) {
            if (*c == ',') {
                fields++;
            }
        }
        
        printf("%d\n", fields);
        rewind(filepointer);
    }
    if (strchr(firstflag, 'r') != NULL) {
        int records = 0;
        //if h is present, first line doesn't count
        if (headerpresent) {
            records = -1;
        }
        
        char reader[512];

        while(fgets(reader, sizeof(reader), filepointer)) {
            records++;
        }
        
        printf("%d\n", records);
        rewind(filepointer);
    } 

    if (fclose (filepointer) == 0) {
        printf("Closed file.\n");
    } else {
        printf("Error closing file.\n");
        return EXIT_FAILURE;
    }
    
    printf("Run successful.\n");
    return EXIT_SUCCESS;
}
