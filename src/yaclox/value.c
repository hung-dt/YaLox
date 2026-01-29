#include "value.h"
#include "memory.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/

void initValueArray(ValueArray* array)
{
  array->capacity = 0;
  array->size = 0;
  array->values = NULL;
}

/*---------------------------------------------------------------------------*/

void appendValueArray(ValueArray* array, Value value)
{
  if ( array->capacity < array->size + 1 ) {
    int oldCap = array->capacity;
    array->capacity = GROW_CAPACITY(oldCap);
    array->values = GROW_ARRAY(Value, array->values, oldCap, array->capacity);
  }

  array->values[array->size] = value;
  array->size++;
}

/*---------------------------------------------------------------------------*/

void freeValueArray(ValueArray* array)
{
  FREE_ARRAY(Value, array->values, array->capacity);
}

/*---------------------------------------------------------------------------*/

void printValue(Value value)
{
  printf("%g", value);
}
