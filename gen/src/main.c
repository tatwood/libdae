#include "xsdparse.h"
#include "gen.h"
#include <expat.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TYPEIDS_BGN  "/*GEN_TYPEIDS_BGN*/"
#define TYPEIDS_END  "/*GEN_TYPEIDS_END*/"
#define TYPEDEFS_BGN "/*GEN_TYPEDEFS_BGN*/"
#define TYPEDEFS_END "/*GEN_TYPEDEFS_END*/"
#define STRUCTS_BGN  "/*GEN_STRUCTS_BGN*/"
#define STRUCTS_END  "/*GEN_STRUCTS_END*/"
#define SCHEMA_BGN   "/*GEN_SCHEMA_BGN*/"
#define SCHEMA_END   "/*GEN_SCHEMA_END*/"

int main(
    int argc,
    char** argv)
{
    if(argc == 4)
    {
        FILE* fpx = fopen(argv[1], "rb");
        FILE* fpi = fopen(argv[2], "rb");
        FILE* fpo = fopen(argv[3], "wb");
        if(fpx != NULL && fpi != NULL && fpo != NULL)
        {
            xsd_schema schema;
            char* data;
            size_t size;
            size_t readsize;
            int err;
            xsd_create_schema(&schema);
            {
                xsdparse_state st;
                XML_Parser xml;
                // initialize the xsd state data
                memset(&st, 0, sizeof(st));
                st.schema = &schema;
                // create expat parser
                xml = XML_ParserCreate(NULL);
                // read schema file
                fseek(fpx, 0, SEEK_END);
                size = ftell(fpx);
                fseek(fpx, 0, SEEK_SET);
                data = (char*) XML_GetBuffer(xml, size);
                readsize = fread(data, 1, size, fpx);
                if(readsize == size)
                {
                    // parse schema xml
                    XML_SetElementHandler(
                        xml,
                        xsdparse_startelement,
                        xsdparse_endelement);
                    XML_SetUserData(xml, &st);
                    XML_ParseBuffer(xml, size, 1);
                    // add hacks
                    xsdparse_hacks(&schema);
                    // validate and format schema
                    err = xsd_validate_schema(&schema);
                    assert(err == 0);
                    xsd_flatten_schema(&schema);
                    err = xsd_validate_schema(&schema);
                    assert(err == 0);
                    xsd_remove_unreferenced_types(&schema);
                    err = xsd_validate_schema(&schema);
                    assert(err == 0);
                    free(st.stack);
                }
                else
                {
                    err = -1;
                    assert(0);
                }
                // clean up expat parser
                XML_ParserFree(xml);
            }
            data = NULL;
            if(err == 0)
            {
                size = 0;
                fseek(fpi, 0, SEEK_END);
                size = ftell(fpi);
                fseek(fpi, 0, SEEK_SET);
                if(size > 0)
                {
                    data = (char*) malloc(sizeof(*data) * (size+1));
                    if(fread(data, 1, size, fpi) != size)
                    {
                        free(data);
                        data = NULL;
                        size = 0;
                    }
                }
            }
            if(data != NULL)
            {
                char* itr = data;
                int writing = 1;
                itr[size] = '\0';
                while(*itr != '\0')
                {
                    if(!strncmp(itr, TYPEIDS_BGN, sizeof(TYPEIDS_BGN)-1))
                    {
                        gen_print_typeids(fpo, &schema);
                        writing = 0;
                        itr += sizeof(TYPEIDS_BGN);
                    }
                    else if(!strncmp(itr, TYPEIDS_END, sizeof(TYPEIDS_END)-1))
                    {
                        writing = 1;
                        itr += sizeof(TYPEIDS_END);
                    }
                    else if(!strncmp(itr,TYPEDEFS_BGN,sizeof(TYPEDEFS_BGN)-1))
                    {
                        gen_print_typedefs(fpo, &schema);
                        writing = 0;
                        itr += sizeof(TYPEDEFS_BGN);
                    }
                    else if(!strncmp(itr,TYPEDEFS_END,sizeof(TYPEDEFS_END)-1))
                    {
                        writing = 1;
                        itr += sizeof(TYPEDEFS_END);
                    }
                    else if(!strncmp(itr, STRUCTS_BGN, sizeof(STRUCTS_BGN)-1))
                    {
                        gen_print_structs(fpo, &schema);
                        writing = 0;
                        itr += sizeof(STRUCTS_BGN);
                    }
                    else if(!strncmp(itr, STRUCTS_END, sizeof(STRUCTS_END)-1))
                    {
                        writing = 1;
                        itr += sizeof(STRUCTS_END);
                    }
                    else if(!strncmp(itr, SCHEMA_BGN, sizeof(SCHEMA_BGN)-1))
                    {
                        gen_print_schema(fpo, &schema);
                        writing = 0;
                        itr += sizeof(SCHEMA_BGN);
                    }
                    else if(!strncmp(itr, SCHEMA_END, sizeof(SCHEMA_END)-1))
                    {
                        writing = 1;
                        itr += sizeof(SCHEMA_END);
                    }
                    else
                    {
                        if(writing)
                        {
                            fwrite(itr, 1, 1, fpo);
                        }
                        ++itr;
                    }
                }
                free(data);
            }
            xsd_destroy_schema(&schema);
        }
        if(fpx != NULL)
        {
            fclose(fpx);
        }
        else
        {
            printf("error: could not open file \"%s\"\n", argv[1]);
        }
        if(fpi != NULL)
        {
            fclose(fpi);
        }
        else
        {
            printf("error: could not open file \"%s\"\n", argv[2]);
        }
        if(fpo != NULL)
        {
            fclose(fpo);
        }
        else
        {
            printf("error: could not open file \"%s\"\n", argv[3]);
        }
    }
    else
    {
        printf("usage: daegen <xsd file> <input file> <_output file>\n");
    }
    return EXIT_SUCCESS;
}
