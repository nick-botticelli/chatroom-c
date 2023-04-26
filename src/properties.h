//
//  properties.h
//  Properties
//
//  Created by Wolf-Dieter Otte on 3/21/23.
//

#pragma once

#include <stdlib.h>


// *********************************************
// * Data type declarations
// *********************************************

// --------------------------------------------
// Property, containing a key/value pair
// --------------------------------------------

typedef struct property_struct
{
    char* key;
    char* value;
} Property;


// -------------------------------------------------
// PropertyListElement for building a property list
// -------------------------------------------------
typedef struct property_list_element_struct
{
    Property property;
    struct property_list_element_struct* next;
} PropertyListElement;


// -------------------------------------------------
// PropertyList containing any number of properties
// -------------------------------------------------

typedef struct property_list_struct
{
    PropertyListElement* first;
    PropertyListElement* last;
} Properties;


// *********************************************
// * Function prototype declarations
// *********************************************

Property*     property_new              (char* key, char* value);
Properties*   property_list_new         (void);
void          property_list_add_property(Properties* properties, Property* property);
Properties*   property_read_properties  (char* properties_file);
char*         property_get_property     (Properties* properties, char* key);
