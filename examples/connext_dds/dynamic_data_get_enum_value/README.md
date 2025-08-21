# Dynamic Data API: access to complex member example

## Concept

*Dynamic Data API* allows to create topic samples in a programmatically manner
without defining an IDL in compile time.

This example shows how to access enum ordinal values by name from a
dynamic data object.

## Example description

In this example, we show three different scenarios to get the ordinal value of
an enum member by using `DynamicData` and `DynamicType` elements.

The first scenario shows how to get the enum member ordinal value from a
`DynamicType`. In this case, you have to specify the enum field name and
the enum element string. In this case, there is a translation from the
`DynamicType` into a `StructType` and then the element into an `EnumType`.

In the second scenario, a for loop iterates through all elements and check
which one is an `ENUMERATION_TYPE`. Then, if this enum is the type that we are
looking for, then it prints the specified values.

In the third scenario, the `EnumType` is gotten from the a DynamicData by using
the `member_type` API.
