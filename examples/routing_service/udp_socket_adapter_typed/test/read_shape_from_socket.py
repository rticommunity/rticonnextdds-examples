import socket
import sys
import struct

# Receive and parse data from the socket
def receive_data(sock):
    # Receive data from the socket
    data, addr = sock.recvfrom(1024)  # Buffer size of 1024 bytes
    # Unpack the data as 3 int types (x, y, shapesize)
    x, y, size = struct.unpack("iii", data)
    return x, y, size, addr


def main():
    if len(sys.argv) != 2:
        print(
            "Usage: python3 read_shape_from_socket.py <port>"
        )
        return

    # Input arguments
    port = int(sys.argv[1])

    samples_received = 0

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        try:
            # Bind the socket to listen on the specified port
            sock.bind(('', port))
            print(f"Listening for shape data on port {port}...")

            while True:
                # Receive data from the socket
                x, y, size, addr = receive_data(sock)
                
                # Print the received shape data
                samples_received += 1
                print(f"Sample #{samples_received} from {addr}: x={x}, y={y}, size={size}")

        except KeyboardInterrupt:
            print("\nExiting...")
        except Exception as e:
            print(f"Error: {e}")


if __name__ == "__main__":
    main()
