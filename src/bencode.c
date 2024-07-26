#include "bencode.h"

char* read_string(FILE* fp) {
    char buf[1024];
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != ':') {
        if (!isdigit(c) || i >= 1024) {
            return NULL;
        }
        buf[i++] = c;
    }
    buf[i] = '\0';
    int len = atoi(buf);
    char *str = malloc(len + 1);
    fread(str, 1, len, fp);
    str[len] = '\0';
    return str;
}

long long read_integer(FILE* fp) {
    char buf[1024];
    int i = 0;
    char c;
    while ((c = fgetc(fp)) != 'e') {
        if (!isdigit(c) || i >= 1024) {
            return 0;
        }
        buf[i++] = c;
    }
    buf[i] = '\0';
    return atoll(buf);
}

bencode_value* parse_list(FILE* fp) {
    bencode_value *obj = malloc(sizeof(bencode_value));
    obj->type = B_ENCODED_LIST;
    int size = 0;
    while (1) {
        char c = fgetc(fp);
        if (c == 'e')
            break;
        ungetc(c, fp);
        obj->list.list_val = realloc(obj->list.list_val, sizeof(bencode_value *) * ++size);
        obj->list.list_val[size-1] = parse_object(fp);
    }
    obj->list.llen = size;
    return obj;
}

bencode_value* parse_dict(FILE* fp) {
    bencode_value *obj = malloc(sizeof(bencode_value));
    obj->type = B_ENCODED_DICTIONARY;
    int size = 0;
    while (1) {
        char c = fgetc(fp);
        if (c == 'e')
            break;
        ungetc(c, fp);
        obj->dict.keys = realloc(obj->dict.keys, sizeof(bencode_value *) * (size + 1));
        obj->dict.values = realloc(obj->dict.values, sizeof(bencode_value *) * (size + 1));
        obj->dict.keys[size] = parse_object(fp);
        obj->dict.values[size] = parse_object(fp);
        size++;
    }
    obj->dict.dlen = size;
    return obj;
}

bencode_value* parse_object(FILE* fp) {
    char c;
    if ((c = fgetc(fp)) == 'i') {
        bencode_value *obj = malloc(sizeof(bencode_value));
        obj->type = B_ENCODED_INTEGER;
        obj->integer = read_integer(fp);
        return obj;
    } else if (isdigit(c)) {
        ungetc(c, fp);
        bencode_value *obj = malloc(sizeof(bencode_value));
        obj->type = B_ENCODED_STRING;
        obj->string.string = read_string(fp);
        return obj;
    } else if (c == 'l') {
        return parse_list(fp);
    } else if (c == 'd') {
        return parse_dict(fp);
    } 
    
    return NULL;
}

void free_bencode_value(bencode_value* obj) {
    if (!obj) return;
    switch (obj->type) {
        case B_ENCODED_STRING:
            free(obj->string.string);
            break;
        case B_ENCODED_LIST:
            for (int i = 0; i < obj->list.llen; ++i) {
                free_bencode_value(obj->list.list_val[i]);
            }
            free(obj->list.list_val);
            break;
        case B_ENCODED_DICTIONARY:
            for (int i = 0; i < obj->dict.dlen; ++i) {
                free_bencode_value(obj->dict.keys[i]);
                free_bencode_value(obj->dict.values[i]);
            }
            free(obj->dict.keys);
            free(obj->dict.values);
            break;
        default:
            break;
    }
    free(obj);
}

bencode_value* bencode_decode_file(const char* filename){
    FILE* fp; 
    char c;

    // Open file
    fp = fopen(filename, "rb");
    
    // File opened?
    if (fp == NULL) 
        return NULL;

    // Parse torrent file
    bencode_value *data = parse_object(fp);

    if(!data)
        return NULL;

    // Close file
    fclose(fp);

    return data;
}

bencode_value* bencode_decode_str(const char* input_str) {
    string in = {
        input_str, 
        0
    };
    return parse_object_str(&in);
}

void bencode_encode(bencode_value *value, char *output, size_t *offset) {
    switch(value->type) {
        case B_ENCODED_INTEGER: {
            sprintf(output + *offset, "i%llde", value->integer);
            *offset += strlen(output + *offset);
            break;
        } case B_ENCODED_STRING: {
            sprintf(output + *offset, "%d:", value->string.strlen);
            *offset += strlen(output + *offset);
            memcpy(output + *offset, value->string.string, value->string.strlen);
            *offset += value->string.strlen;
            break;
        } case B_ENCODED_LIST: {
            strcat(output + *offset, "l");
            *offset += 1;
            for (int i = 0; i < value->list.llen; i++) {
                bencode_encode(value->list.list_val[i], output, offset);
            }
            strcat(output + *offset, "e");
            *offset += 1;
            break;   
        } case B_ENCODED_DICTIONARY: {
            strcat(output + *offset, "d");
            *offset += 1;
            for (int i = 0; i < value->dict.dlen; i++) {
                bencode_encode(value->dict.keys[i], output, offset);
                bencode_encode(value->dict.values[i], output, offset);
            }
            strcat(output + *offset, "e");
            *offset += 1;
            break;
        }
    }
}


char* read_string_str(string* input) {
    char buf[1024];
    int i = 0;
    char c;
    while ((c = input->str[input->pos++]) != ':') {
        if (!isdigit(c) || i >= 1024) {
            return NULL;
        }
        buf[i++] = c;
    }
    buf[i] = '\0';
    int len = atoi(buf);
    char *str = malloc(len + 1);
    strncpy(str, input->str + input->pos, len);
    input->pos += len;
    str[len] = '\0';
    return str;
}

long long read_integer_str(string* input) {
    char buf[1024];
    int i = 0;
    char c;
    while ((c = input->str[input->pos++]) != 'e') {
        if (!isdigit(c) || i >= 1024) {
            return 0;
        }
        buf[i++] = c;
    }
    buf[i] = '\0';
    return atoll(buf);
}

bencode_value* parse_object_str(string* in) {
    char c = in->str[in->pos++];
    if (c == 'i') {
        bencode_value *obj = malloc(sizeof(bencode_value));
        obj->type = B_ENCODED_INTEGER;
        obj->integer = read_integer_str(in);
        return obj;
    } 
    
    if (isdigit(c)) {
        in->pos--;
        bencode_value *obj = malloc(sizeof(bencode_value));
        obj->type = B_ENCODED_STRING;
        obj->string.string = read_string_str(in);
        return obj;
    } 
    
    if (c == 'l')
        return parse_list_str(in);
    
    if (c == 'd')
        return parse_dict_str(in);

    return NULL;
}

bencode_value* parse_list_str(string* in) {
    bencode_value *obj = malloc(sizeof(bencode_value));
    obj->type = B_ENCODED_LIST;
    int size = 0;
    obj->list.list_val = NULL;
    while (1) {
        char c = in->str[in->pos++];
        if (c == 'e')
            break;
        in->pos--;
        obj->list.list_val = realloc(obj->list.list_val, sizeof(bencode_value *) *++size);
        obj->list.list_val[size-1] = parse_object_str(in);
    }
    obj->list.llen = size;
    return obj;
}

bencode_value* parse_dict_str(string* in) {
    bencode_value *obj = malloc(sizeof(bencode_value));
    obj->type = B_ENCODED_DICTIONARY;
    int size = 0;
    obj->dict.keys = NULL;
    obj->dict.values = NULL;
    while (1) {
        char c = in->str[in->pos++];
        if (c == 'e')
            break;
        in->pos--;
        obj->dict.keys = realloc(obj->dict.keys, sizeof(bencode_value *)* (size+1));
        obj->dict.values = realloc(obj->dict.values, sizeof(bencode_value *)* (size+1));
        obj->dict.keys[size] = parse_object_str(in);
        obj->dict.values[size] = parse_object_str(in);
        size++;
    }
    obj->dict.dlen = size;
    return obj;
}

void print_bencode_value(bencode_value *value, int indent) {
    if (!value) return;
    
    for(int i = 0; i < indent; i++)
        printf("\t");

    switch (value->type) {
        case B_ENCODED_INTEGER:
            printf("%lld", value->integer);
            break;
        case B_ENCODED_STRING:
            printf("\"%.*s\"", value->string.strlen, value->string.string);
            break;
        case B_ENCODED_LIST:
            printf("[\n");
            for (int i = 0; i < value->list.llen; i++) {
                print_bencode_value(value->list.list_val[i], indent);
                printf(",");
            }
            printf("]\n");
            break;
        case B_ENCODED_DICTIONARY:
            printf("{\n");
            for (int i = 0; i < value->dict.dlen; i++) {
                print_bencode_value(value->dict.keys[i], indent+1);
                printf(": ");
                print_bencode_value(value->dict.values[i], indent+1);
                printf(",\n");
            }
            printf("}\n");
            break;
        default:
            printf("Unknown bencode type\n");
    }
}