#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

void throw(const char *msg, ...);
void assert(bool condition, const char *msg, ...);
void compile_src(const char *input, char **ret);

int main(int argc, char **argv) {
    FILE *fp;
    char *file_content, *output_content;
    uint32_t file_length;
    const char *input_file, *output_file;
    
    assert(argc > 2, "Error: Usage: %s <input file> <output file>\n", *argv);
    input_file = argv[1];
    output_file = argv[2];

    fp = fopen(input_file, "r");
    assert(fp, "Error: input file %s could not be opened for reading\n", input_file);
    file_content = malloc(file_length = (fseek(fp, 0L, SEEK_END), ftell(fp)));
    fseek(fp, 0L, SEEK_SET);
    fread(file_content, 1, file_length, fp);
    fclose(fp);

    compile_src(file_content, &output_content);

    fp = fopen(output_file, "w");
    assert(fp, "Error: output file %s could not be opened/created for writing\n", output_file);
    fwrite(output_content, 1, strlen(output_content), fp);
    fclose(fp);
    
    free(file_content);
    free(output_content);

    return 0;
}

void throw(const char *msg, ...) {
    va_list list;
    va_start(list, msg);
    vprintf(msg, list);
    exit(1);
}

void assert(bool condition, const char *msg, ...) {
    if(!condition) {
        va_list list;
        va_start(list, msg);
        vprintf(msg, list);
        exit(1);
    }
}

void compile_src(const char *input, char **ret) {
    const char *preheader =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "\t<style>\n"
        "\t\th1{display: inline-block;}\n"
        "\t\th2{display: inline-block;}\n"
        "\t\th3{display: inline-block;}\n"
        "\t\th4{display: inline-block;}\n"
        "\t\th5{display: inline-block;}\n"
        "\t\th6{display: inline-block;}\n",
    *header =
        "\t</style>\n"
        "\t<body id=\"work\"></body>\n"
        "\t<button onclick=\"{this.style.display='none'; main();}\">Begin</button>\n"
        "\t<script>\n"
        "\t\tconst work = document.getElementById(\"work\");\n"
        "\t\tvar speed = 30;\n"
        "\t\tconst sleep = ms => new Promise(r => setInterval(r, ms));\n"
        "\t\tconst dprint = async msg => { for(let i=0; i<msg.length; i++) {await sleep(speed); work.innerHTML += msg[i] } };\n"
        "\t\tconst edprint = async (msg, t) => {for(let i=0; i<msg.length; i++) {await sleep(speed); work.innerHTML += \"<\"+t+\">\" + msg[i] + \"</\"+t+\">\" } };\n"
        "\t\tconst printnl = () => work.innerHTML += \"<br/>\";\n"
        "\t\tconst main = async () => {\n",
    *footer =
        "\t\t}\n"
        "\t</script>\n"
        "</html>\n";
    char *buf, *tmp, *eptr;
    uint32_t ip, bp, line;
    uint64_t sln;
    *ret = malloc(strlen(preheader)+1);
    strcpy(*ret, preheader);
    for(ip=0; input[ip] == ' ' || input[ip] == '\n' || input[ip] == '\t' || input[ip] == '\r'; ip++);\
    if(input[ip] == '%') {
        buf = calloc(3,1);
        strcpy(buf, "\t\t");
        ip++;
        while(input[ip] != '%') {
            assert(input[ip], "Error: style does not end\n");
            if(input[ip-1] == '\n') {
                buf = realloc(buf, strlen(buf)+3);
                strcat(buf, "\t\t");
            }
            buf = realloc(buf, strlen(buf)+2);
            buf[strlen(buf)+1] = 0;
            buf[strlen(buf)] = input[ip];
            ip++;
        }
        ip++;
        *ret = realloc(*ret, strlen(*ret)+strlen(buf)+1);
        strcat(*ret, buf);
    }
    *ret = realloc(*ret, strlen(*ret)+strlen(header)+1);
    strcat(*ret, header);
    for(line=0; input[ip]; ip++, line++) {
        buf = calloc(1,1);
        for(; input[ip] != '\n' && input[ip] != '\0'; ip++) {
            buf = realloc(buf, strlen(buf)+2);
            buf[strlen(buf)+1] = 0;
            buf[strlen(buf)]   = input[ip];
        }
        switch(*buf) {
        case '!':
            for(bp = 0; buf[bp] == '!'; bp++);
            *ret = realloc(*ret, strlen(*ret)+strlen("\t\t\tawait edprint(String.raw`")+1);
            strcat(*ret, "\t\t\tawait edprint(String.raw`");
            *ret = realloc(*ret, strlen(buf)+strlen(*ret)-bp+1);
            strcat(*ret, buf+bp);
            *ret = realloc(*ret, strlen(*ret)+strlen("`, \"h\");\n")+11);
            strcat(*ret, "`, \"h%d\");\n");
            tmp = malloc(strlen(*ret)+1);
            strcpy(tmp, *ret);
            sprintf(*ret, tmp, bp);
            free(tmp);
            break;
        case '~':
            *ret = realloc(*ret, strlen(*ret)+strlen("\t\t\tprintnl();\n")+1);
            strcat(*ret, "\t\t\tprintnl();\n");
            if(buf[1]) {
                *ret = realloc(*ret, strlen(*ret)+strlen("\t\t\tawait dprint(String.raw`")+strlen(buf)+strlen("`);\n"));
                strcat(*ret, "\t\t\tawait dprint(String.raw`");
                strcat(*ret, buf+1);
                strcat(*ret, "`);\n");
            }
            break;
        case '#': break;
        case '\\':
            if(buf[1]) {
                *ret = realloc(*ret, strlen(*ret)+strlen("\t\t\tawait dprint(String.raw`")+strlen(buf)+strlen("`);\n"));
                strcat(*ret, "\t\t\tawait dprint(String.raw`");
                strcat(*ret, buf+1);
                strcat(*ret, "`);\n");
            }
            break;
        case ']':
            if(!memcmp(buf+1, "speed ", 6)) {
                sln = strtol(buf+7, &eptr, 0);
                assert(!(*eptr) && buf[7], "Error: speed value NaN on line %d\n", line);
                *ret = realloc(*ret, strlen(*ret)+strlen("\t\t\tspeed = ;\n")+11);
                strcat(*ret, "\t\t\tspeed = %d;\n");
                tmp = malloc(strlen(*ret)+1);
                strcpy(tmp, *ret);
                sprintf(*ret, tmp, sln);
                free(tmp);
            } else if(!memcmp(buf+1, "pause ", 6)) {
                sln = strtol(buf+7, &eptr, 0);
                assert(!(*eptr) && buf[7], "Error: pause value NaN on line %d\n", line);
                *ret = realloc(*ret, strlen(*ret)+strlen("\t\t\tawait sleep();\n")+11);
                strcat(*ret, "\t\t\tawait sleep(%d);\n");
                tmp = malloc(strlen(*ret)+1);
                strcpy(tmp, *ret);
                sprintf(*ret, tmp, sln);
                free(tmp);
            }
            break;
        case '*':
            bp = buf[1] == '*' ? 'b': 'i';
            *ret = realloc(*ret, strlen(*ret)+strlen("\t\t\tawait edprint(String.raw`")+1);
            strcat(*ret, "\t\t\tawait edprint(String.raw`");
            *ret = realloc(*ret, strlen(*ret)+strlen(buf + (bp - 'b' ? 1 : 2))+1);
            strcat(*ret, buf+(bp-'b'? 1 : 2));
            *ret = realloc(*ret, strlen(*ret)+strlen("`, \"%c\")\n")+1);
            strcat(*ret, "`, \"%c\")\n");
            tmp = malloc(strlen(*ret)+1);
            strcpy(tmp, *ret);
            sprintf(*ret, tmp, bp);
            break;
        default:
            *ret = realloc(*ret, strlen(*ret)+strlen("\t\t\tawait dprint(String.raw`")+strlen(buf)+strlen("`);\n")+1);
            strcat(*ret, "\t\t\tawait dprint(String.raw`");
            strcat(*ret, buf);
            strcat(*ret, "`);\n");
            break;
        }
        free(buf);
    }
    *ret = realloc(*ret, strlen(*ret)+strlen(footer)+1);
    strcat(*ret, footer);
    printf("%s\n", *ret);
}