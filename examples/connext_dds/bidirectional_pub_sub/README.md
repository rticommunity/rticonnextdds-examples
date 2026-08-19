# Bidirectional Pub/Sub Example (Python)

This example demonstrates **bi-directional publish/subscribe communication** using
RTI Connext DDS. Two processes each act as both a publisher and a subscriber,
exchanging data across two topics in a ping-pong pattern.

## Concept

```
Process A                          Process B
----------                         ----------
Publisher  --> [ topic_b ] -->  Subscriber
Subscriber <-- [ topic_a ] <--  Publisher
```

Both processes run simultaneously. Each publishes on one topic and subscribes on
the other, creating a fully bidirectional data flow — without using the
Request-Reply pattern.

## Requirements

- RTI Connext DDS 7.x
- Python 3
- Connext Python API: `pip install rti.connext`

## Running the Example

Open **two terminals** and run each process simultaneously:

**Terminal 1:**
```bash
python3 publisher.py
```

**Terminal 2:**
```bash
python3 subscriber.py
```

### Expected Output

**Process A terminal:**
```
[Process A] Started on domain 0
[Process A] Publishing to 'topic_b', subscribing from 'topic_a'
[Process A] Waiting for Process B to come online...

[Process A] >> Sent:     [0] 'Hello from A'
[Process A] << Received: [0] 'Hello from B' from Process_B
[Process A] >> Sent:     [1] 'Hello from A'
[Process A] << Received: [1] 'Hello from B' from Process_B
...
```

**Process B terminal:**
```
[Process B] Started on domain 0
[Process B] Publishing to 'topic_a', subscribing from 'topic_b'
[Process B] Waiting for Process A to come online...

[Process B] >> Sent:     [0] 'Hello from B'
[Process B] << Received: [0] 'Hello from A' from Process_A
...
```

### Optional Arguments

Both scripts accept the same arguments:

| Argument | Default | Description |
|----------|---------|-------------|
| `--domain-id` | `0` | DDS Domain ID |
| `--count` | `0` (infinite) | Number of samples to send |

**Example with arguments:**
```bash
python3 publisher.py --domain-id 1 --count 10
python3 subscriber.py --domain-id 1 --count 10
```

## Key Design Decisions

- **Single participant per process** — each process creates one `DomainParticipant`
  that owns both its publisher and subscriber. This is the recommended DDS pattern.
- **WaitSet for receiving** — instead of polling in a busy loop, a `WaitSet` with a
  `StatusCondition` is used to efficiently wait for incoming data with a timeout.
- **Plain pub/sub, not Request-Reply** — this example intentionally avoids the
  `rpc` / RequestReply pattern to show that bidirectional communication can be
  achieved with standard topics.
- **Shared Message type** — both processes define the same `Message` IDL struct
  (`sender`, `content`, `count`), which DDS matches automatically via topic name
  and type compatibility.

## Notes on Timing

Both processes publish and subscribe concurrently. Because DDS discovery takes a
moment when participants first come online, the very first few samples may not be
received by the other side. This is expected behaviour and not a bug. For
production use cases requiring guaranteed delivery of early samples, consider
setting `TRANSIENT_LOCAL` durability QoS on the DataWriter and DataReader.