# Example Code: Routing Service Administration

## Concept

*Routing Service* uses the common Remote Administration Platform (see [Remote
Administration
Platform](https://community.rti.com/static/documentation/connext-dds/6.1.0/doc/manuals/connext_dds_professional/services/routing_service/common/remote_admin_platform.html))
This example shows how to use the platform to send remote commands to a running
instance of Routing Service.

## Example Description

This example shows a one-shot application that can be used to send one remote
command to a running application of Routing Service (you can also try to use
it with Routing Service). The application will prepare a command request based
on the command-line parameters provided in the application invocation. After
sending that request out to the service, it will wait for a command reply to be
sent by the service. The application will then exit.

### Routing Service configuration

Here you can find the Routing Service configuration files.

-   `routing_remote_admin.xml`: this is a configuration file for Routing
    Service that enables remote administration, and uses the QoS profiles
    defined in the XML QoS profiles file (`USER_QOS_PROFILES.xml`). By using
    the same QoS profiles in both the application and the *Routing Service* we
    ensure the QoS matching and the reliability of the communication.
-   `domain_route_creation.xml`: this xml has the configuration about how to
    create a new domain route that routes from domain 1 to domain 3. It is used
    by the Requester application to CREATE the new route.

## Running some examples

We describes here some examples of how to use the Remote Administration
Platform.

First of all, you have to build the Requester application. So, please follow
this [guide](c++11/README.md).

After building the Requester application, we will open two ShapesDemo:

- One have to run in domain 1 and publish `Squares`.
- The other have to run in domain 2 and subscribe `Circles`.

After that, we have to run Routing Service so the second ShapeDemo starts
receiving `Circles`:

``` bash
 rtiroutingservice -cfgFile routing_remote_admin.xml -cfgName remote_admin
```

### Deleting a Domain Route

To delete a Domain Route, just execute the Requester application located in the
build directory using the following command:

``` bash
./Requester DELETE /routing_services/remote_admin/domain_routes/MyRoute
```

### Creating a Domain Route

We will use the `domain_route_creation.xml` file to describe the new
Domain Route. Just execute the Requester application located in the
build directory using the following command:

``` bash
./Requester CREATE /routing_services/remote_admin file://domain_route_creation.xml
```

Please, note that the file has to be present in the same location where Routing
Service is being execute. Routing Service will try to read the file from there.

Run another ShapeDemo that runs on Domain 3 and subscribes to `Triangles`.
You will see how `Squares` are now being route as `Triangles` from
Domain 1 to 3.

### Getting the Domain Route information

If you want to see the configuration of a specific Domain Route, Just execute
the Requester application located in the build directory using the following
command:

``` bash
./Requester GET /routing_services/remote_admin/domain_routes/MyRoute
```

## Requirements

To run this example you will need:

- RTI Connext Professional version 6.1.0 or higher.
- CMake version 3.11 or higher
- A target platform supported by *RTI* *RoutingService*.
