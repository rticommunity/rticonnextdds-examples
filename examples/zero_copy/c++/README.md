# Example Code to demonstrate the usage of zero copy

## Building C++ Example

Before compiling or running the example, make sure the environment variable 
NDDSHOME is defined and it refers to your RTI Connext DDS installation directory. 

Run rtiddsgen with the -example option and the target architecture of your 
choice.
 
Do not use the -replace option. Do not replace the following files included 
with this example:  

- zero_copy_publisher.cxx
- zero_copy_subscriber.cxx
- Frame.h
- FrameSupport.h

For example, assuming you want to generate an example for the architecture 
x64Linux3gcc4.8.2, run:

	rtiddsgen -example  x64Linux3gcc4.8.2 -ppDisable zero_copy.idl

After running rtiddsgen, you will see messages that look like this:

	WARN com.rti.ndds.nddsgen.emitters.FileEmitter File exists and will not 
        be overwritten : <example_dir>\zero_copy_publisher.cxx
	...

This is the expected normal output and is only informing you that the specified 
files have not been replaced, which is the desired behavior.
	
To build this example for the x64Linux3gcc4.8.2 architecture, type the following in a command
shell:

	gmake -f makefile_zero_copy_x64Linux3gcc4.8.2
  
## Running the example applications


To run the example applications you can use the generated makefile. In one
terminal situated in the example directory, run:

### Publisher application

For x64Linux3gcc4.8.2, run: 

	./objs/x64Linux3gcc4.8.2/zero_copy_publisher [options]

Where [options]:  
	-h                  Shows this page    
	-d  [domain_id]     Sets the domain id **Default**: 0  
	-fc [frame count]   Sets the total number of frames to be mapped in the shared memory queue **Default**: frame rate  
	-fr [frame rate]    Sets the rate at which frames are written **Default**: 60fps  
	-sc [sample count]  Sets the number of frames to send **Default**: 1200  
	-k  [key]	           Sets the key for shared memory segment **Default**: obtained automatically   
	-s  [buffer size]   Sets payload size of the frame in bytes **Default**: 1048576 (1MB)  
	-rc [dr count]      Expected number of DataReaders that will receive frames **Default**: 1  
	-v                  Displays the checksum for each frame 

### Subscriber application

For x64Linux3gcc4.8.2, run: 

   ./objs/x64Linux3gcc4.8.2/zero_copy_subscriber [options]  
   
Where [options]:  
    -h                  Shows this page  
    -d  [domain_id]     Sets the domain id **Default**: 0  
    -sc [sample count]  Sets the number of frames to receive **Default**: 1200  
    -s  [buffer size]   Sets the payload size of the frame in bytes **Default**: 1048576 (1MB)  
    -v                  Displays checksum and computed latency of each received frame 
  
## Publisher Output

Running using:  
  Domain ID:  0  
  Frame Count in SHMEM segment: 60  
  Frame Rate: 60 fps  
  Sample Count: 1200  
  SHMEM Key: 1371756096  
  Frame size: 1048576 bytes  
  Expected DataReader Count: 1  
Waiting until 1DataReaders are discovered  
Going to send 1200frames at 60 fps  
Writen 60 frames  
Writen 120 frames  
Writen 180 frames  
Writen 240 frames

## Subscriber Output

Running using:  
  Domain ID:  0  
  Sample Count: 1200  
  Frame size: 1048576 bytes  
ZeroCopy subscriber waiting to receive samples...  
Average latency: 249 microseconds  
Average latency: 225 microseconds  
Average latency: 223 microseconds  
Average latency: 219 microseconds
  
