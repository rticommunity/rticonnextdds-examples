# Example Code: Batching example

## Concept
Batching can be used to decrease the amount of communication overhead associated
with the transmission and (in the case of reliable communication)
acknowledgement of small samples, in order to increase throughput.

It specifies and configures the mechanism that allows *Connext* to collect
multiple user data samples to be sent in a single network packet, to take
advantage of the efficiency of sending larger packets and thus increase
effective throughput.

Batching can be used to increase effective throughput dramatically for small
data samples. Throughput for small samples (size < 2048 bytes) is typically
limited by CPU capacity and not by network bandwidth. Batching many smaller
samples to be sent in a single large packet will increase network utilization
and thus throughput in terms of samples per second.

*Turbo Mode* is an experimental feature that uses an intelligent algorithm
that adjusts the number of bytes in a batch at runtime according to
current system conditions, such as write speed (or write frequency) and
sample size. This intelligence is what gives it the ability to increase
throughput at high message rates and avoid negatively impacting message
latency at low message rates

## Example description
This example shows the differences between *batching* and *turbo mode*.
Both options will send batches. This means that instead of sending each sample
out on the wire with its own header, the middleware will store the data in a
batch until it is full and then will send the entire batch as one data packet.

The publisher application has two configurations (two different profiles):
- The first one uses the *Batching QoS* setting. We have included a package
  capture you can analyze using *wireshark*: `batchin_10_samples.pcap file
  package no. 98.`

- The second one uses *Turbo Mode*. We have included a package capture you can
  analyze using *wireshark*: `turbo_mode.pcap file package no. 76.`
