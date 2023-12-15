# Example Code: Skip Data Serialization for Data Recording

## Running the Example

In one command prompt run the recorder:

```sh
python record.py --record data.bin
```

In a second command prompt run the publisher:

```sh
python record.py --publish
```

The recorder application will print a message each time a sample is recorded.

Now kill the recorder and run the replay application. A file called `data.bin`
will have been created in the current directory.

Now we will run a subscriber and a reply application.

To subscribe to the data we will simply use **rtiddsspy**:

```sh
<Connext installation>/bin/rtiddsspy -printSample
```

Now run the application that replays the data recorded in  `data.bin`:

```sh
python record.py --replay data.bin
```

The subscriber will print the data that is being replayed.
