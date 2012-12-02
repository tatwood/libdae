libdae
======

Auto generated C library for processing COLLADA files based on the XSD schema

author:  Thomas Atwood (tatwood.net)
date:    2011
license: see UNLICENSE.txt

Description
===========

One of the goals of this project is to be as easy as possible to setup and
build. Technically, only one C file is required to build and link with this
library. If ./src/dae.c is linked into an application, and dae.h is in the
include path, no additional setup is required. Realistically, any project
using this library will also need an XML parser. No dependencies exist on any
particular XML parser, but utility functions are provided that are designed to
interact with a SAX parser. Specifically, even though they do not depend on
expat, they are written so they can be plugged directly into expat without any
glue.

Unfortunately, due to differences in the type names and the methodology used
to define types between the 1.4.1 xsd and 1.5 xsd, the apis generated from
the two different versions of the spec are not compatible. The spec themselves
are defined to be backwards compatible, so code generated from the 1.5 xsd may
be used to parse xml exported using the 1.4.1 spec.

Code generated from the 1.5 spec is included by default.

Building
========

    ./include/dae.h  - auto generated header for public API
    ./include/daeu.h - public API definition of optional utility functions
    ./src/dae.c      - auto generated implementation of public API
    ./src/daeu.c     - implementation of optional utility functions

    add ./include to the include search path
    add ./src/dae.c and optionally ./src/daeu.c as build dependencies

Usage
=====

Assuming that an expat XML_Parser with the variable name "expat" was created
and given a data buffer containing COLLADA xml, the following code will parse
it into a dae_COLLADA struct:

    dae_COLLADA* collada = dae_create();
    daeu_xml_parser daeparser;
    daeu_xml_create(collada, &daeparser);
    XML_SetElementHandler(
        expat,
        daeu_xml_startelement,
        daeu_xml_endelement);
    XML_SetCharacterDataHandler(expat, daeu_xml_chardata);
    XML_SetUserData(expat, daeparser);
    if(XML_ParseBuffer(expat, size, 1) != XML_STATUS_OK)
    {
        err = -1;
    }
    daeu_xml_destroy(parser);

When finished with the document, destroy it as follows:

    dae_destroy(collada);

Code Generation
===============

The source code generation project is in the directory ./gen. The binary will
regenerate source code given a path to the XSD schema and a template input
file. For example, if the schema file "collada_schema_1_5.xsd" was placed
inside the gen folder, dae.c and dae.h could be generated as follows:

    cd gen
    bin/daegen collada_schema_1_5.xsd template/dae.c ../src/dae.c
    bin/daegen collada_schema_1_5.xsd template/dae.h ../include/dae.h
