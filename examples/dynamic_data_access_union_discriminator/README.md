# Example Code: Access Union Discriminators in Dynamic Data

## Concept
Unions in IDL are mapped to structs in C and C++, so that *Connext* will not
have to dynamically allocate memory for unions containing variable-length
fields such as strings or sequences. To be efficient, the entire struct
(or class in C++/CLI) is not sent when the union is published. Instead,
Connext uses the discriminator field of the struct to decide what field
in the struct is actually sent on the wire.

## Example Description
In this example we illustrate how to access the discriminator of a union
using the *Dynamic Data API*.

First, we create the *TypeCode* for the union. Then, we create a *Dynamic Data*
instance of it and finally, we show how to access the discriminator.
