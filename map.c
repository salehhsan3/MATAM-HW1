#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "map.h"


/*a list for the elements in the map- include the data and the key of each element*/
typedef struct node
{
    MapDataElement data;
    MapKeyElement key;
    struct node *next;
} * Node;

/*struct for the map*/
struct Map_t
{
    Node head; //A dummy member that will point to the first member in the list
    Node first;
    Node iterator;
    copyMapDataElements copy_data_elements;
    copyMapKeyElements copy_key_elements;
    freeMapDataElements free_data;
    freeMapKeyElements free_key;
    compareMapKeyElements compare_key_element;
};

/*fill the copy of the map with the map's fields except the head and the first fields*/
static void fillSomeCopyMapFields(Map map,Map duplicate,copyMapKeyElements copy_key_elements,
                copyMapDataElements copy_data_elements,compareMapKeyElements compare_key_element,
                freeMapKeyElements free_key, freeMapDataElements free_data,Node iterator_value)
{
    duplicate->copy_key_elements = map->copy_key_elements;
    duplicate->copy_data_elements = map->copy_data_elements;
    duplicate->compare_key_element = map->compare_key_element;
    duplicate->free_key = map->free_key;
    duplicate->free_data = map->free_data;
    duplicate->iterator = iterator_value;
    return;
}

/*fill the given struct node of an element in the map with the recived key and data
error type:MAP_ERROR in case copy_data_elements or copy_key_elements failed (this error tell
the called function it will have to free the node)
otherwise return MAP_SUCCESS*/
static MapResult fillNode(Map map, Node node, MapKeyElement keyElement, MapDataElement dataElement,
                     Node next)
{
    assert((map) && (node) && (keyElement) && (dataElement));
    node->data = map->copy_data_elements(dataElement);
    if(node->data==NULL) 
    {
        return MAP_ERROR;
    }
    node->key = map->copy_key_elements(keyElement);
    if(node->key==NULL)
    {
        map->free_data(node->data);
        node->data=NULL;
        return MAP_ERROR;
    }
    node->next = next;
    return MAP_SUCCESS;
}

/*find the first location of an element with keyElement as key in the map and return it,
if it doesn't exist return NULL*/
static Node findLocationOfKey(Map map, MapKeyElement keyElement)
{
    assert((map) && (keyElement));
    Node current = map->first;
    while (current != NULL)
    {
        if (map->compare_key_element(current->key, keyElement) == 0)
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}
/*find the first element in the map with keyElement as key (we know the element exists in the map)
and change the value of the data to dataElement*/
static MapResult changeDataByKey(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    assert((map) && (keyElement) && (dataElement));
    Node key_location = findLocationOfKey(map, keyElement); 
    MapDataElement new_data=map->copy_data_elements(dataElement);
    if(new_data==NULL)
    {
        return MAP_ERROR; //there was a problem with coping the dataElement
    }
    map->free_data(key_location->data); //the copy succedded -> we can free the data now 
    key_location->data = new_data; //changed the data
    return MAP_SUCCESS;                                        
}

/*add an element to the map as the first element
error type: MAP_OUT_OF_MEMORY in case of allocation fail
            MAP_ERROR in case there was a problem in allocation while using fillNode
            function, because of the copies funcftion recived from the user- in that
            case the callled function will have to free the node
otherwise return MAP_SUCCESS*/
static MapResult addElementAsFirst(Map map, MapKeyElement keyElement, MapDataElement dataElement,Node next)
{
    assert( (map) && (keyElement) && (dataElement) );
    Node first=malloc(sizeof(*first));
    if(first==NULL)
    {
        return MAP_OUT_OF_MEMORY;
    }
    if(fillNode(map,first,keyElement,dataElement,next)!=MAP_SUCCESS)
    {
        free(first); 
        first=NULL;
        return MAP_OUT_OF_MEMORY;
    }
    map->first=first;
    map->head->next=first;
    return MAP_SUCCESS;
}

/*add an element to the map as the last element
error type: MAP_OUT_OF_MEMORY in case of allocation fail
            MAP_ERROR if there was a problem filling the node via the user's copies function- the
            called function will have to free the node
otherwise return MAP_SUCCESS*/
static MapResult addElementAsLast(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    assert( (map) && (keyElement) && (dataElement) );
    Node last=malloc(sizeof(*last));
    if(last==NULL)
    {
        return MAP_OUT_OF_MEMORY;
    }
    if(fillNode(map,last,keyElement,dataElement,NULL)!=MAP_SUCCESS)
    {
        free(last);
        last=NULL;
        return MAP_OUT_OF_MEMORY;
    }
    Node before_last=map->first;
    while(before_last->next != NULL)
    {
        before_last = before_last->next;
    }
    before_last->next=last;
    return MAP_SUCCESS;
}
/*insert an element to the map between two element or at the end (map include at least two elements)
error type: MAP_OUT_OF_MEMORY in case of allocation fail
            MAP_ERROR if fill node failed- the called function will have to free the node
otherwise return MAP_SUCCESS*/
static MapResult insertElementBetweenOrAtEnd(Map map,MapKeyElement keyElement, MapDataElement dataElement)
{
    assert( (map) && (keyElement) && (dataElement) );
    Node element=malloc(sizeof(*element));
    if(!element)
    {
        return MAP_OUT_OF_MEMORY;
    }
    if(fillNode(map,element,keyElement,dataElement,NULL)!=MAP_SUCCESS)
    {
        free(element);
        element=NULL;
        return MAP_OUT_OF_MEMORY;
    }
    Node current = map->first->next;
    Node before_current=map->first;
    while(current->next!=NULL)
    {
        if (map->compare_key_element(current->key,keyElement)<=0)
        {
            before_current = current;
            current=current->next;   
        }
        else  //element is between elements
        {
            element->next=current;
            before_current->next=element;
            return MAP_SUCCESS;
        }   
    }
    //add between elements , before the last element in the map (could happen if map include only 2 elements)
    if(map->compare_key_element(current->key,keyElement)>0)
    {
        element->next=current;
        before_current->next=element;
        return MAP_SUCCESS;
    }
    else //add at the end
    {
    current->next=element;
    element->next = NULL;
    return MAP_SUCCESS;
    }
}

/*add new element to the map
error type:MAP_OUT_OF_MEMORY in case of allocation fail
otherwise return MAP_SUCCESS*/
static MapResult addNewElementToMap(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    assert( (map) && (keyElement) && (dataElement) );
    if(map->first==NULL) //the map is empty
    {
        return addElementAsFirst(map,keyElement,dataElement,NULL);
    }
    if(map->first->next==NULL) // there is only one element in the map
    {
        if (map->compare_key_element(map->first->key,keyElement) >= 0)
        {
            return addElementAsFirst(map,keyElement,dataElement,map->first);
        }
        else
        {
            return addElementAsLast(map,keyElement,dataElement);
        }
    }
    //there are 2 elements or more in the map
    if( map->compare_key_element(map->first->key, keyElement)>=0 ) //smaller or equall to first 
    {
        return addElementAsFirst(map,keyElement,dataElement,map->first);
    }
    return insertElementBetweenOrAtEnd(map,keyElement,dataElement);  
}

/*deallocate the memory used for the node*/
static void freeNode(Node node,freeMapKeyElements free_key, freeMapDataElements free_data)
{
    free_key(node->key);
    node->key=NULL;
    free_data(node->data);
    node->data=NULL;
    free(node);
    node=NULL;
    return;
}

//////////////////////////////implementaion of functions from map.h///////////////////////////////

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements)
{
    if ((!copyDataElement) || (!copyKeyElement) || (!freeDataElement) || (!freeKeyElement) ||
        (!compareKeyElements))
    {
        return NULL;
    }
    Map map = malloc(sizeof(*(map)));
    if (map == NULL)
    {
        return NULL;
    }
    map->copy_data_elements = copyDataElement;
    map->copy_key_elements = copyKeyElement;
    map->free_data = freeDataElement;
    map->free_key = freeKeyElement;
    map->compare_key_element = compareKeyElements;
    map->first = NULL;
    map->iterator = NULL;
    map->head = malloc(sizeof(*(map->head)));
    if (!map->head)
    {
        free(map);
        map=NULL;
        return NULL;
    }
    map->head->data = NULL;
    map->head->key = NULL;
    map->head->next = NULL;
    return map;
}

void mapDestroy(Map map)
{
    if ((!map) || (!map->head))
    {
        return;
    }
    if (!(map->head->next)) //map is empty
    {
        free(map->head);
        map->head=NULL;
        free(map);
        map=NULL;
        return;
    }
    Node current = map->first;
    while (current != NULL)
    {
        map->free_data(current->data);
        map->free_key(current->key);
        Node to_free = current;
        current = current->next;
        free(to_free);
        to_free=NULL;
    }
    map->head->next = NULL;
    map->first=NULL;
    free(map->head);
    map->head=NULL;
    free(map);
    map=NULL;
    return;
}

Map mapCopy(Map map)
{
    if (!map)
    {
        return NULL;
    }
    map->iterator=NULL; //iterator is undefined after the copy function.
    Map duplicate = mapCreate(map->copy_data_elements, map->copy_key_elements, map->free_data,
                              map->free_key, map->compare_key_element);
    if (duplicate == NULL)
    {
        return NULL;
    }
    fillSomeCopyMapFields(map,duplicate,map->copy_key_elements,map->copy_data_elements,map->compare_key_element,
                                                                        map->free_key,map->free_data,NULL);
    Node current_in_duplicate = duplicate->head; 
    Node current_in_map = map->first;
    while (current_in_map != NULL)
    {
        current_in_duplicate->next = malloc(sizeof(*current_in_duplicate)); 
        if ( current_in_duplicate->next == NULL) 
        {
            mapDestroy(duplicate);
            return NULL;
        }                                    
        if(fillNode(map, current_in_duplicate->next,current_in_map->key,current_in_map->data,NULL)
                                                                                !=MAP_SUCCESS)
        {
            free( current_in_duplicate->next); 
            current_in_duplicate->next=NULL; 
            mapDestroy(duplicate);
            return NULL;
        }
        current_in_map = current_in_map->next;
        current_in_duplicate = current_in_duplicate->next;
    }
    duplicate->first=duplicate->head->next;
    return duplicate;
}

int mapGetSize(Map map)
{
    if (!map)
    {
        return (-1);
    } 
    int elements_counter = 0;
    Node current = map->first;
    while (current != NULL)
    {
        elements_counter++;
        current = current->next;
    }
    return (elements_counter);
}

bool mapContains(Map map, MapKeyElement element)
{
    if ((!map) || (!element))
    {
        return false;
    }
    if (map->first == NULL) //map is empty
    {
        return false;
    }
    Node current = map->first;
    while (current != NULL)
    {
        if (map->compare_key_element(current->key, element) == 0)
        {
            return true;
        }
        current = current->next;
    }
    return false;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if(!map)
    {
        return MAP_NULL_ARGUMENT;
    }
    if ((!keyElement) || (!dataElement))
    {
        map->iterator=NULL;
        return MAP_NULL_ARGUMENT;
    }
    map->iterator = NULL;
    if (mapContains(map, keyElement))
    {
        return changeDataByKey(map, keyElement, dataElement);
    }
    return addNewElementToMap(map, keyElement, dataElement);
}

MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    if ((!map) || (!keyElement) || (!mapContains(map, keyElement)))
    {
        return NULL;
    }
    Node key_location = findLocationOfKey(map, keyElement);
    return key_location->data;
}

MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    if (!map)
    {
        return MAP_NULL_ARGUMENT;
    }
    map->iterator=NULL; //iterator is undefined after this function
    if(!keyElement)
    {
        return MAP_NULL_ARGUMENT;
    }
    if (!mapContains(map, keyElement))
    {
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    if (map->first->next == NULL) //the element we want to remove is the only element in the map
    {
        map->head->next = NULL;
        freeNode(map->first,map->free_key,map->free_data);
        map->first = NULL;
        return MAP_SUCCESS;
    }
    Node key_location = findLocationOfKey(map, keyElement);
    if (key_location == map->first) //there are more than one element in the map and we want to remove the first
    {
        map->first = map->first->next;
        map->head->next = map->first;
        freeNode(key_location,map->free_key,map->free_data);
        return MAP_SUCCESS;
    }
    Node before_key = map->first;
    while (before_key->next != key_location) //the one we want  to remove is not the first
    {
        before_key = before_key->next;
    }
    before_key->next = key_location->next;
    freeNode(key_location,map->free_key,map->free_data);
    return MAP_SUCCESS;
}

MapKeyElement mapGetFirst(Map map)
{
    if ((!map) || (!map->head))
    {
        return NULL;
    }
    if (map->first == NULL) //map is empty
    {
        return NULL;
    }
    MapKeyElement smallest_key=map->copy_key_elements(map->first->key);  
    if(smallest_key==NULL)
    {
        return NULL; //there was a problem copying the key using the user's function 
    }
    map->iterator = map->first;
    return smallest_key;
}

MapKeyElement mapGetNext(Map map)
{
    if ((!map) || (map->iterator == NULL) )
    {
        return NULL;
    }
    if((map->iterator->next == NULL)) //reached last element
    {
        map->iterator=NULL;
        return NULL;
    }
    while(map->iterator->next!=NULL)
    {
        if(map->compare_key_element(map->iterator->key,map->iterator->next->key)==0)
        {
         map->iterator=map->iterator->next;   
        }
        else
        {
            map->iterator=map->iterator->next;
            MapKeyElement next_key=map->copy_key_elements(map->iterator->key);
            if(next_key==NULL) //there was a problem copying using the user function for copying keys
            {
                return NULL;
            }
            return next_key;
        }
    }
    map->iterator=NULL;
    return NULL;
}

MapResult mapClear(Map map)
{
    if (!map)
    {
        return MAP_NULL_ARGUMENT;
    }
    if (map->head->next == NULL) //map is empty
    {
        map->iterator = NULL;
        return MAP_SUCCESS;
    }
    Node current = map->first;
    while (current != NULL)
    {
        Node to_free = current;
        current = current->next;
        map->free_key(to_free->key);
        map->free_data(to_free->data);
        free(to_free);
        to_free=NULL;
    }
    map->head->next=NULL;
    map->first=NULL;
    map->iterator = NULL;
    return MAP_SUCCESS;
}