//
//  properties.c
//
//  Created by Wolf-Dieter Otte on 3/21/23.
//

#include "debug.h"

#include "properties.h"

/* ******************************************************************** */
/* Return a value for a given key                                       */
/* ******************************************************************** */

char* property_get_property(Properties* properties, char* key) {
    
    char* value = NULL;
    
    // from the beginning of the list ...
    PropertyListElement* current = properties->first;
    
    // ... run through all the properties
    while(current->next)
    {
        if(strcmp(current->property.key, key) == 0)
        {
            value = current->property.value;
            break;
        }

        // go to next node
        current = current->next;
    }
    
    return value;
}


/* ******************************************************************** */
/* Read properties of "key = value" pairs (mind the spaces)             */
/* from a properties file and return a pointer to a list of properties  */
/* ******************************************************************** */
Properties* property_read_properties(char* properties_file_string)
{
    FILE* properties_file;
    char key[64];
    char value[64];
    char line[64];
    
    Properties* properties = property_list_new();
    Property* property_ptr;

    // open file
    properties_file = fopen(properties_file_string, "r");
    probe(properties_file, "Opening property file \"%s\" failed!", properties_file_string)
    //debug("Property file \"%s\" opened successfully!", properties_file_string);

    // run through the lines of the properties file and
    // add each property to the properties list
    while(fgets(line, sizeof(line), properties_file))
    {
        sscanf(line, "%63s = %63s", key, value);
        
        property_ptr = property_new(key, value);
        property_list_add_property(properties, property_ptr);
    }

    return properties;
    
error:
    printf("Oopsie ...");
    exit(EXIT_FAILURE);
}


/* *********************************************************************** */
/* Create a new property, allocate space for it and copy in key/value pair */
/* *********************************************************************** */

Property* property_new(char* key, char* value)
{
    Property* property_ptr = (Property*) malloc(sizeof(Property));
    char* key_ptr   = (char*) malloc(strlen(key)   + 1);
    char* value_ptr = (char*) malloc(strlen(value) + 1);

    strcpy(key_ptr, key);
    strcpy(value_ptr, value);
    
    property_ptr->key = key_ptr;
    property_ptr->value = value_ptr;
    
    return property_ptr;
}

/* *********************************************************************** */
/* Create a new property list                                              */
/* *********************************************************************** */

Properties* property_list_new(void)
{
    // make room for a PropertyList
    Properties* property_list = (Properties*) malloc(sizeof(Properties));

    property_list->first = (PropertyListElement*) malloc(sizeof(PropertyListElement));
    property_list->first->next = NULL;
    
    property_list->last = property_list->first;
    
    return property_list;
}


/* *********************************************************************** */
/* Add a new property to a property list                                   */
/* *********************************************************************** */

void property_list_add_property(Properties* properties, Property* property)
{
    properties->last->property = *property;
    properties->last->next = (PropertyListElement*) malloc(sizeof(PropertyListElement));
    properties->last->next->next = NULL;
    
    properties->last = properties->last->next;
}


/* *********************************************************************** */
/* Example driver to show usage of the property functions                  */
/* To run, remove underscore!                                              */
/* *********************************************************************** */

// #include <stdio.h>
// #include "properties.h"

// int _main(int argc, const char * argv[]) {

//     char* properties_file = "test.properties";
//     Properties* properties;
//     char* key = "MY_PORT";
//     char* value;
    
//     properties = property_read_properties(properties_file);
//     value = property_get_property(properties, key);
    
//     printf("\nValue for %s: %s\n", key, value);

//     return EXIT_SUCCESS;
// }
