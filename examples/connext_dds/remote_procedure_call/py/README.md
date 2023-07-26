# Example Code: Remote Procedure Calls

:warning: **Note**: For an overview of this feature, see the
[Remote Procedure Calls](https://community.rti.com/static/documentation/connext-dds/7.2.0/doc/api/connext_dds/api_python/rpc.html#remote-procedure-calls)
section of the *Connext* Python API Reference.

## Running the Example

This example is composed of two applications: a service and a client.

In one command prompt window run the service application:

```sh
$ python inventory_service.py
```

The client application can add or remove items to the inventory. On a new
window run the following commands:

```sh
$ python inventory_client.py --add bananas
$ python inventory_client.py --remove apples --quantity 99
```

(Use ``-h`` to see the full list of arguments.)


## Expected output

The expected client output will be:

```sh
$ python inventory_client.py --add bananas

Initial inventory:  [Item(name='apples', quantity=100), Item(name='oranges', quantity=50)]
Add 1 bananas
Updated inventory:  [Item(name='apples', quantity=100), Item(name='oranges', quantity=50), Item(name='bananas', quantity=1)]
```

```sh
$ python inventory_client.py --remove apples --quantity 99

Initial inventory:  [Item(name='apples', quantity=100), Item(name='oranges', quantity=50), Item(name='bananas', quantity=1)]
Remove 99 apples
Updated inventory:  [Item(name='apples', quantity=1), Item(name='oranges', quantity=50), Item(name='bananas', quantity=1)]
```

## Exception propagation

Trying to remove an item that is not in the inventory will propagate an
`UnknownItemError` exception to the client:

```sh
$ python inventory_client.py -r pears

...
Unknown item: pears
...
```
## Concurrency

The example also demonstrates the service concurrency model. The service
application can be run with the `--delay <seconds>` argument. This makes the
*add* and *remove* operations sleep for the specified number of seconds,
simulating a long running operation.

Close the service from the previous section and start a new one:

```sh
$ python inventory_service.py --delay 10
```

On another window run a client:

```sh
./Inventory_publisher --add bananas

Initial inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50]}]
```

And on another window run another client:

```sh
./Inventory_publisher --add bananas

Initial inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50]}]
```

Note how the second client was able to get the inventory while the *add*
operation from the first client is still running.

After 10 seconds, the first client will finish:

```sh
Add 1 bananas
Updated inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50], [name: bananas, quantity: 1]}]
```

Followed by the second client:

```sh
Add 1 bananas
Updated inventory: [items: {[name: apples, quantity: 100], [name: oranges, quantity: 50], [name: bananas, quantity: 2]}]
```

For a description of the concurrency model, see the
[Remote Procedure Calls](https://community.rti.com/static/documentation/connext-dds/7.2.0/doc/api/connext_dds/api_python/rpc.html#remote-procedure-calls)
section of the Connext Python API Reference.
