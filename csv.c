#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define FLAG_F 0x01
#define FLAG_R 0x02
#define FLAG_H 0x04
#define FLAG_MAX 0x08
#define FLAG_MIN 0x10
#define FLAG_MEAN 0x20
#define FLAG_RECORDS 0x40

#define MEM_ALIGN 8 // Size used for heap memory alignment

unsigned short int parseFlags(int argc, char **argv, char **p_min, char **p_max, char **p_mean, char **p_rfield, char **p_rvalue) {
    unsigned short int flags = 0;
    //Flag reader. 
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-min") == 0) {
            flags |= FLAG_MIN;
            *p_min = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-max") == 0) {
            flags |= FLAG_MAX;
            *p_max = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-mean") == 0) {
            flags |= FLAG_MEAN;
            *p_mean = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-records") == 0) {
            flags |= FLAG_RECORDS;
            *p_rfield = argv[i + 1];
            *p_rvalue = argv[i + 2];
            i += 2;
        } else if (argv[i][0] == '-') {
            if (strchr(argv[i], 'f'))
                flags |= FLAG_F;
            if (strchr(argv[i], 'r'))
                flags |= FLAG_R;
            if (strchr(argv[i], 'h'))
                flags |= FLAG_H;
        }
    }
    return flags;
}

int getFields(FILE *f_ptr) {
    int fields;
    char c;
    for (fields = 1, c = fgetc(f_ptr); c != EOF && c != '\n'; c = fgetc(f_ptr)) {
        if (c == ',')
            fields++;
    }
    return fields;
}

// Returns a CSV record as an array of pointers to strings allocated on the heap;
// Takes a file pointer pointer argument in order to advance the file pointer in the parent scope;
// Takes an integer argument to set the length of the array;
// There is an additional index at the end of each record to serve as a next pointer if '-records' is passed
// This allows a linked list to be formed so the records can be printed after the file reading loop
// Returns NULL if End Of File is reached
char **getRecord(FILE** fp_ptr, int fields) {
    FILE *lookahead = *fp_ptr;
    int len, i;
    char c, *txt, **record;

    for (len = 0, c = fgetc(lookahead); c != EOF && c != '\n'; len++, c = fgetc(lookahead));

    len++;
    len += ((MEM_ALIGN - (len % MEM_ALIGN)) % MEM_ALIGN);
    txt = calloc(len, sizeof(char));
    for (i = 1; i < MEM_ALIGN+1; i++) {
        if (txt[len-i] == '\n') {
            txt[len-i] = 0;
            break;
        }
    }

    if (!fgets(txt, len, *fp_ptr)) {
        free(txt);
        return NULL;
    }

    record = calloc(fields + 1, sizeof(char *));
    for (i = 0; i < fields; i++)
        record[i] = strtok(txt, ",");
    
    return record;
}

// Frees the array of pointers generated on the heap by getRecord()
void freeRecord(char **record, int fields) {
    int i;
    for (i = 0; i < fields; i++)
        free(record[i]);
    free(record);
}

int parseIndex(char *value, int fields) {
    if (value == NULL)
        return -1;

    if (value[0] < 48 || value[0] > 57) {
        fprintf(stderr, "Provided index value \"%s\" value is not recognized as positive integer numeric (starts with non-number character)\n", value);
        exit(EXIT_FAILURE);
    }

    int int_val = atoi(value);
    if (int_val >= fields) {
        fprintf(stderr, "Integer \"%d\" exceeds maximum possible index of records of length %d\n", int_val, fields);
        exit(EXIT_FAILURE);
    }

    return int_val;
}

// Converts the header text to a numeric index corresponding to the information
// located in the same "cell" as the header text is found in the header line
int parseIndexFromHeader(char **headers, char *value, int fields) {
    if (value == NULL)
        return -1;

    for (int idx = 0; idx < fields; idx++)
        if (strcmp(headers[idx], value) == 0) {
            return idx;    
    }

    fprintf(stderr, "No header \"%s\" could be found\n", value);
    exit(EXIT_FAILURE);
}

int main (int argc, char* argv[]) {

    if (argc > 11) {
        fprintf(stderr, "Invalid number of arguments: expected maximum 11 but got %d\n", argc);
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
        fprintf(stderr, "Error opening file.\n");
        return EXIT_FAILURE;
    }
    
    printf("File opened: %s\n", filename);

    //Recorded flag values.
    char* min = NULL;
    char* max = NULL;
    char* mean = NULL;
    char* recordfield = NULL;
    char* recordvalue = NULL;
    unsigned short int flags = parseFlags(argc, argv, &min, &max, &mean, &recordfield, &recordvalue);
   
    int fields, records;
    int min_idx, max_idx, mean_idx, record_idx;
    double min_val, max_val, sum;
    char **reader, **headers;
    char **r_head, **r_tail;
    
    fields = getFields(filepointer);
    min_val = INT_MIN;
    max_val = INT_MAX;
    r_head = NULL;
    r_tail = NULL;

    // If flag -h is passed, save the headers record.
    // Because we save the headers record before the reader loop, it will not be counted by flag -r
    if (flags & FLAG_H) {
        headers = getRecord(&filepointer, fields);
        min_idx = parseIndexFromHeader(headers, min, fields);
        max_idx = parseIndexFromHeader(headers, max, fields);
        mean_idx = parseIndexFromHeader(headers, mean, fields);
        record_idx = parseIndexFromHeader(headers, recordfield, fields);
    }
    else {
        min_idx = parseIndex(min, fields);
        max_idx = parseIndex(max, fields);
        mean_idx = parseIndex(mean, fields);
        record_idx = parseIndex(recordfield, fields);
    }

    records = 0;
    sum = 0;
    
    int temp;
    for (reader = getRecord(&filepointer, fields); reader != NULL; reader = getRecord(&filepointer, fields)) {
        // Count total non-header records
        records++;
        if (min){
            // Check if currenet record value in given field is max, if so overwrite
            temp = atof(reader[min_idx]);
            if (temp < min_val)
                min_val = temp;
        }
        if (max) {
            // Check if currenet record value in given field is max, if so overwrite
            temp = atof(reader[max_idx]);
            if (temp < max_val)
                max_val = temp;
        }
        if (mean) {
            // Add to sum that will be divided by # of records at the end of the loop to calculate the mean
            temp = atof(reader[mean_idx]);
            sum += temp;
        }
        if (recordfield) {
            // Save records that match the given value in 'recordfield' variable
            if (strcmp(reader[record_idx], recordvalue) == 0) {
                if (r_head == NULL)
                    r_head = reader;
                r_tail[fields] = (char *) reader;
                r_tail = reader;
            }                
        }
    }
    
    // Close filepointer or program fails
    if (fclose (filepointer) == 0) {
        printf("Closed file.\n");
    } else {
        fprintf(stderr, "Error closing file.\n");
        return EXIT_FAILURE;
    }

    // If flag -f is passed, print the number of fields
    if (flags & FLAG_F) {
        printf("%d\n", fields);
    }
    // If flag -r is passed, print the number of records
    if (flags & FLAG_R) {
        printf("%d\n", records);
    }
    // If flag -min is passed, print the minimum value of the field passed as an argument
    if (min) {
        printf("%lf\n", min_val);
    }
    // If flag -max is passed, print the maximum value of the field passed as an argument
    if (max) {
        printf("%lf\n", max_val);
    }
    // If flag -mean is passed, print the mean value of the field passed as an argument
    if (mean) {
        printf("%lf\n", sum / ((double) records));
    }
    // If flag -records is passed, print all records which have the value of the field passed as an argument
    if (recordfield) {
        for (reader = r_head; reader != NULL; reader = r_head) {
            printf("%s", reader[0]);
            for (int i = 1; i < fields; i++) {
                printf(",%s", reader[i]);
            }
            printf("\n");
        }
    }
    
    printf("Run successful.\n");
    return EXIT_SUCCESS;
}
