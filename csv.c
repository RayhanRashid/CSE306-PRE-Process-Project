#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <stdbool.h>

#define FLAG_F 0x01
#define FLAG_R 0x02
#define FLAG_H 0x04
#define FLAG_MAX 0x08
#define FLAG_MIN 0x10
#define FLAG_MEAN 0x20
#define FLAG_RECORDS 0x40

int isNumeric(char* c) {
    if (*c == '\0') {
        return 0;
    }
    while (*c) {
        if (!isdigit(*c)) {
            return 0;
        }
        c++;
    }
    return 1;
}

int isDouble(const char *str) {
    if (str == NULL || *str == '\0') {
        return 0; 
    }

    char *endptr;
    strtod(str, &endptr);

    while (*endptr) {
        if (!isspace((unsigned char)*endptr)) {
            return 0; 
        }
        endptr++;
    }

    return 1; 
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
    unsigned short int flags = 0;
    //Flag reader. 
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-min") == 0) {
            flags |= FLAG_MIN;
            min = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-max") == 0) {
            flags |= FLAG_MAX;
            max = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-mean") == 0) {
            flags |= FLAG_MEAN;
            mean = argv[i + 1];
            i++;
        } else if (strcmp(argv[i], "-records") == 0) {
            flags |= FLAG_RECORDS;
            recordfield = argv[i + 1];
            recordvalue = argv[i + 2];
            i += 2;
        } else if (argv[i][0] == '-') {
            if (strchr(argv[i], 'f'))
                flags |= FLAG_F;
            if (strchr(argv[i], 'r'))
                flags |= FLAG_R;
            if (strchr(argv[i], 'h'))
                flags |= FLAG_H;
        }
        //Ignore non flag arguments until filename (last argument);
    }
    //Reminder to remove all debug statements before submitting!!
    printf("Flags read. flag_f: %d, flag_r: %d, flag_h: %d, min: %s, max: %s, mean: %s, recordfield: %s, recordvalue: %s\n", (flags & FLAG_F), (flags & FLAG_R) >> 1, (flags & FLAG_H) >> 2, min, max, mean, recordfield, recordvalue);
    
    if (flags & FLAG_F) {
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
    if (flags & FLAG_R) {
        int records = 0;
        //if h is present, first line doesn't count
        if (flags & FLAG_H) {
            records = -1;
        }
        
        char reader[512];

        while(fgets(reader, sizeof(reader), filepointer)) {
            records++;
        }
        
        printf("%d\n", records);
        rewind(filepointer);
    }

    if (flags & FLAG_H) {
        //header versions of min max and mean flags
        if (min != NULL) {
            double minvalue = DBL_MAX;
            char reader[512];
            char fieldvalue[512];
            char* fillerpointer;
            int targetfield = 0;
            int fieldindex = 0;
            int valueindex = 0;
            bool inquotes = false;
            fgets(reader, sizeof(reader), filepointer);
            for (char *c = reader; *c; c++) {
                    if (*c == '"') {
                        inquotes = !inquotes;
                    }
                    else if ((*c == ',' || *c == '\n') && !inquotes) {
                        
                        if (*c == '\n') {
                            fieldvalue[strlen(fieldvalue) - 1] = '\0';
                        }
                        
                        if (strcmp(fieldvalue, min) == 0) {
                            targetfield = fieldindex;
                        }

                        memset(fieldvalue, 0, sizeof(fieldvalue));
                        
                        fieldindex++;
                        valueindex = 0;

                    }
                    if (*c != ',' && *c != '\n') {
                        fieldvalue[valueindex] = *c;
                        valueindex++;
                    }
                        
            }
            

            //printf("Target field: %d\n", targetfield);
            
                    
            
        
            while (fgets(reader, sizeof(reader), filepointer)) {
                int field = 0;
                fieldvalue[0] = '\0';
                valueindex = 0;
                inquotes = false;
                bool capturing = false;
                for (char *c = reader; *c; c++) {
                    //printf("C is %d\n", *c);
                    //printf("Field is %d\n", field);
                    if (*c == '"') {
                        inquotes = !inquotes;
                    }
                    if (*c == ',' && !inquotes) {
                        field++;
                        if (capturing) break; 
                        
                        continue;
                    }
                    //printf("Field: %d\n", field);
                    if (field == targetfield && ((*c == '.') || ((*c >= '0') && (*c <= '9')))) {
                        capturing = true;
                        
                        fieldvalue[valueindex] = *c;
                        valueindex++;
                        
                        
                    }
                }
                //printf("Field value: %s\n", fieldvalue);
               
                if (isDouble(fieldvalue)) {
                    double value = strtod(fieldvalue, &fillerpointer);
                    
                    if (fillerpointer != fieldvalue) {
                        if (value < minvalue) {
                            minvalue = value;
                        }
                    }       
                }
            }
            if (minvalue == DBL_MAX) {
                return EXIT_FAILURE;
            }
            printf("%f\n", minvalue);
            rewind(filepointer);
        }
        if (max != NULL) {
            double maxvalue = DBL_MIN;
            char reader[512];
            char fieldvalue[512];
            char* fillerpointer;
            int targetfield = 0;
            int fieldindex = 0;
            int valueindex = 0;
            bool inquotes = false;
            fgets(reader, sizeof(reader), filepointer);
            for (char *c = reader; *c; c++) {
                    if (*c == '"') {
                        inquotes = !inquotes;
                    }
                    else if ((*c == ',' || *c == '\n') && !inquotes) {
                        
                        if (*c == '\n') {
                            fieldvalue[strlen(fieldvalue) - 1] = '\0';
                        }
                        
                        if (strcmp(fieldvalue, max) == 0) {
                            targetfield = fieldindex;
                        }

                        memset(fieldvalue, 0, sizeof(fieldvalue));
                        
                        fieldindex++;
                        valueindex = 0;

                    }
                    if (*c != ',' && *c != '\n') {
                        fieldvalue[valueindex] = *c;
                        valueindex++;
                    }
                        
            }
            

            //printf("Target field: %d\n", targetfield);
            
                    
            
        
            while (fgets(reader, sizeof(reader), filepointer)) {
                int field = 0;
                fieldvalue[0] = '\0';
                valueindex = 0;
                inquotes = false;
                bool capturing = false;
                for (char *c = reader; *c; c++) {
                    //printf("C is %d\n", *c);
                    //printf("Field is %d\n", field);
                    if (*c == '"') {
                        inquotes = !inquotes;
                    }
                    if (*c == ',' && !inquotes) {
                        field++;
                        if (capturing) break; 
                        
                        continue;
                    }
                    //printf("Field: %d\n", field);
                    if (field == targetfield && ((*c == '.') || ((*c >= '0') && (*c <= '9')))) {
                        capturing = true;
                        
                        fieldvalue[valueindex] = *c;
                        valueindex++;
                        
                        
                    }
                }
                //printf("Field value: %s\n", fieldvalue);
               
                if (isDouble(fieldvalue)) {
                    double value = strtod(fieldvalue, &fillerpointer);
                    
                    if (fillerpointer != fieldvalue) {
                        if (value > maxvalue) {
                            maxvalue = value;
                        }
                    }       
                }
            }
            if (maxvalue == DBL_MAX) {
                return EXIT_FAILURE;
            }
            printf("%f\n", maxvalue);
            rewind(filepointer);
        }
        if (mean != NULL) {
            double sum = 0;
            double itemcount = 0;
            char reader[512];
            char fieldvalue[512];
            char* fillerpointer;
            int targetfield = 0;
            int fieldindex = 0;
            int valueindex = 0;
            bool inquotes = false;
            fgets(reader, sizeof(reader), filepointer);
            for (char *c = reader; *c; c++) {
                if (*c == '"') {
                    inquotes = !inquotes;
                }
                else if ((*c == ',' || *c == '\n') && !inquotes) {
                        
                    if (*c == '\n') {
                        fieldvalue[strlen(fieldvalue) - 1] = '\0';
                    }
                    
                    if (strcmp(fieldvalue, mean) == 0) {
                        targetfield = fieldindex;
                    }

                    memset(fieldvalue, 0, sizeof(fieldvalue));
                    
                    fieldindex++;
                    valueindex = 0;
                    
                }
                if (*c != ',' && *c != '\n') {
                    fieldvalue[valueindex] = *c;
                    valueindex++;
                }
                
            }
            

            //printf("Target field: %d\n", targetfield);
            
                    
            
        
            while (fgets(reader, sizeof(reader), filepointer)) {
                int field = 0;
                fieldvalue[0] = '\0';
                memset(fieldvalue, 0, sizeof(fieldvalue));
                valueindex = 0;
                inquotes = false;
                bool capturing = false;
                for (char *c = reader; *c; c++) {
                    //printf("C is %d\n", *c);
                    //printf("Field is %d\n", field);
                    if (*c == '"') {
                        inquotes = !inquotes;
                    }
                    if (*c == ',' && !inquotes) {
                        field++;
                        if (capturing) break; 
                        
                        continue;
                    }
                    //printf("Field: %d\n", field);
                    if (field == targetfield && ((*c == '.') || ((*c >= '0') && (*c <= '9')))) {
                        capturing = true;
                        
                        fieldvalue[valueindex] = *c;
                        valueindex++;
                        
                        
                    }
                }
                //printf("Field value: %s\n", fieldvalue);
               
                if (isDouble(fieldvalue)) {
                    double value = strtod(fieldvalue, &fillerpointer);
                    
                    if (fillerpointer != fieldvalue) {
                        
                            sum += value;
                            itemcount++;
                        
                    } else {
                        //printf("This value was not included.");
                    }       
                }
            }
            if (itemcount == 0) {
                return EXIT_FAILURE;
            }
            //printf("Sum: %f\n", sum);
            //printf("Item count: %f\n", itemcount);
            printf("%f\n", sum / itemcount);
            rewind(filepointer);
        }
    } else {
        if (min != NULL) {
            double minvalue = DBL_MAX;
            char reader[512];
            char* fillerpointer;
            int targetfield = atoi(min);
            while (fgets(reader, sizeof(reader), filepointer)) {
                int field = 0;
                char fieldvalue[512];
                int valueindex = 0;
                bool inquotes = false;
                bool capturing = false;
                for (char *c = reader; *c; c++) {
                    //printf("C is %d\n", *c);
                    //printf("Field is %d\n", field);
                    if (*c == '"') {
                        inquotes = !inquotes;
                    }
                    if (*c == ',' && !inquotes) {
                        field++;
                        if (capturing) break; 
                        
                        continue;
                    }
                    
                    if (field == targetfield && ((*c == '.') || ((*c >= '0') && (*c <= '9')))) {
                        capturing = true;
                        
                        fieldvalue[valueindex] = *c;
                        valueindex++;
                        
                        
                    }
                }
                //printf("Field value: %s\n", fieldvalue);
               
                if (isDouble(fieldvalue)) {
                    double value = strtod(fieldvalue, &fillerpointer);
                    
                    if (fillerpointer != fieldvalue) {
                        if (value < minvalue) {
                            minvalue = value;
                        }
                    }       
                }
            }
            if (minvalue == DBL_MAX) {
                return EXIT_FAILURE;
            }
            printf("%f\n", minvalue);
            rewind(filepointer);
        }

        if (max != NULL) {
            double maxvalue = DBL_MIN;
            char reader[512];
            char* fillerpointer;
            int targetfield = atoi(max);
            
            while (fgets(reader, sizeof(reader), filepointer)) {
                int field = 0;
                char fieldvalue[512];
                int valueindex = 0;
                bool inquotes = false;
                bool capturing = false;
                for (char *c = reader; *c; c++) {
                    //printf("C is %d\n", *c);
                    //printf("Field is %d\n", field);
                    if (*c == '"') {
                        inquotes = !inquotes;
                    }
                    if (*c == ',' && !inquotes) {
                        field++;
                        if (capturing) break; 
                        
                        continue;
                    }
                 
                    if (field == targetfield && ((*c == '.') || ((*c >= '0') && (*c <= '9')))) {
                        capturing = true;
                        
                        fieldvalue[valueindex] = *c;
                        valueindex++;
                        
                        
                    }
                }
                //printf("Field value: %s\n", fieldvalue);
               
                if (isDouble(fieldvalue)) {
                    double value = strtod(fieldvalue, &fillerpointer);
                    
                    if (fillerpointer != fieldvalue) {
                        if (value > maxvalue) {
                            maxvalue = value;
                        }
                    }       
                }
            }
            if (maxvalue == DBL_MIN) {
                return EXIT_FAILURE;
            }
            printf("%f\n", maxvalue);
            rewind(filepointer);
        }
        if (mean != NULL) {
            double sum = 0;
            double itemcount = 0;
            char reader[512];
            char* fillerpointer;
            int targetfield = atoi(mean);

            while (fgets(reader, sizeof(reader), filepointer)) {
                int field = 0;
                char fieldvalue[512];
                int valueindex = 0;
                bool capturing = false;
                bool inquotes = false;
                
                for (char *c = reader; *c; c++) {
                    //printf("C is %d\n", *c);
                    //printf("Field is %d\n", field);
                    if (*c == '"') {
                        inquotes = !inquotes;
                    }
                    if (*c == ',' && !inquotes) {
                        field++;
                        if (capturing) break; 
                        
                        continue;
                    }
                    //printf("Field: %d\n", field);
                    if (field == targetfield) {
                        capturing = true;
                        
                        fieldvalue[valueindex] = *c;
                        valueindex++;
                        
                        
                    }
                }
                fieldvalue[valueindex] = '\0'; 
                //printf("Field value: %s\n", fieldvalue);
                if (isDouble(fieldvalue)) {
                    double value = strtod(fieldvalue, &fillerpointer);
                    if (fillerpointer != fieldvalue) {
                        sum += value;
                        itemcount++;
                    }
                } else {
                    //printf("This value was not included\n");
                }
            }
            
            //printf("Sum: %f\n", sum);
            //printf("Item count: %f\n", itemcount);
            if (itemcount == 0) {
                return EXIT_FAILURE;
            }
           
            printf("%f\n", sum / itemcount);
            rewind(filepointer);
            return EXIT_SUCCESS;
        }          
        
      
        
    }


    

    if (fclose (filepointer) == 0) {
        printf("Closed file.\n");
    } else {
        fprintf(stderr, "Error closing file.\n");
        return EXIT_FAILURE;
    }
    
    printf("Run successful.\n");
    return EXIT_SUCCESS;
}
