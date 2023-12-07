/*
** rpl - Search and replace strings.
** Copyright (C) 2023  Olivia May
**
** This program is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>

// looks at beginning of stream
bool is_stream_match_word(const char *stream, const char *word) {
    for (int j = 0; j < strlen(word); j++) {
        if (stream[j] != word[j]) {
            return false;
        }
    }

    return true;
}

char *insert_string(char *insertee_string, const int index, const char *inserter_string) {
    char *new_string = 
        (char *)malloc((strlen(insertee_string) + strlen(inserter_string) + 2)
        * sizeof(char));
    int i, j;

    if (!new_string) {
        fprintf(stderr, "Failed to allocate memory to insert %s into stream\n",
            inserter_string);
        exit(2);
    }

    for (i = 0; i < index; i++) {
        new_string[i] = insertee_string[i];
    }
    for (j = 0; j < strlen(inserter_string); j++) {
        new_string[i + j] = inserter_string[j];
    }
    for (i; i < strlen(insertee_string) + 1; i++) {
        new_string[i + j] = insertee_string[i];
    }

    free(insertee_string);
    return new_string;
}

// beginning of stream
void remove_chars(char *stream, const int amount) {
    int i;
    for (i = 0; i < strlen(stream); i++) {
        stream[i] = stream[i + amount];
    }
}

// 0   1        2        3
// rpl OLD-TEXT NEW-TEXT FILE
int main(int argc, char **argv) {

    if (argc < 4) {
        
        printf("usage: rpl [-h]\n           OLD-TEXT NEW-TEXT FILE\n");

        if (argc > 1) {
            if (!strcmp("-h", argv[1]) || !strcmp("--help", argv[1])) {

                printf("Search and replace text in files.\n \
                    \n \
                    positional arguments:\n  OLD-TEXT\n  NEW-TEXT\n \
                    FILE\n \
                    \n \
                    optional arguments:\n \
                    -h, --help\n");
                
                return 0;
            }
        }

        fprintf(stderr, "rpl: error: the following arguments are required: \
            OLD-TEXT, NEW-TEXT, FILE\n");
    
        return 1;
    }
    if (access(argv[3], F_OK)) {

        fprintf(stderr, "rpl: error: file not found '%s'\n", argv[3]);
        return 1;
    }

    char *stream;
    FILE *file;
    int i, ch, old_text_len, new_text_len;
    int stream_old_text_diff;
    struct stat statinfo;
    
    stat(argv[3], &statinfo);
    old_text_len = strlen(argv[1]);
    new_text_len = strlen(argv[2]);
    stream_old_text_diff = statinfo.st_size - old_text_len;
    if (stream_old_text_diff < 0) {
        fprintf(stderr, "rpl: error: OLD-TEXT larger than file\n");
        return 1;
    }
   
    // + 1 for null char
    stream = (char *)malloc((statinfo.st_size + 1) * sizeof(char));
    if (!stream) {
        fprintf(stderr, "rpl: error: Failed to allocate memory to read %s\n",
            argv[3]);
        return 2;
    }

    file = fopen(argv[3], "r");
    
    i = 0;
    while (true) {
        ch = fgetc(file);
        if (ch < 0) break;
        stream[i] = ch;
        i++;
    }
    stream[i] = '\0';

    fclose(file);

    // same as `i <= x;`
    for (i = 0; i < stream_old_text_diff + 1; i++) {
        if (is_stream_match_word(&stream[i], argv[1])) {
            remove_chars(&stream[i], strlen(argv[1]));
            stream = insert_string(stream, i, argv[2]);
            stream_old_text_diff = statinfo.st_size + new_text_len - old_text_len;
        }
    }

    file = fopen(argv[3], "w");
    fputs(stream, file);

    fclose(file);
    free(stream);
    return 0;
}
