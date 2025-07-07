import socket
import sys
import struct

# The following methods are used to simulate a moving shape (not used in this version)
def initialize_position():
    x = random.randint(lower_bound, upper_bound_x)
    y = random.randint(lower_bound, upper_bound_y)
    direction_x = random.choice([-1, 1])
    direction_y = random.choice([-1, 1])
    return x, y, direction_x, direction_y


def move_position(x, y, direction_x, direction_y):
    x += direction_x * step_size
    y += direction_y * step_size

    if x <= lower_bound or x >= upper_bound_x:
        direction_x = -direction_x
        x = max(lower_bound, min(x, upper_bound_x))

    if y <= lower_bound or y >= upper_bound_y:
        direction_y = -direction_y
        y = max(lower_bound, min(y, upper_bound_y))

    return x, y, direction_x, direction_y


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
