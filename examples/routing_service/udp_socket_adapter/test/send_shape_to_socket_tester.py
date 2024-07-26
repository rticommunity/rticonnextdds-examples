import socket
import sys
import time
import struct
import random

shapesize = 30
step_size = 5

lower_bound = shapesize // 2
upper_bound_x = 235 - (shapesize // 2)
upper_bound_y = 265 - (shapesize // 2)

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

def send_data(sock, server_address, port, x, y):
    data = struct.pack('iii', x, y, shapesize)
    sock.sendto(data, (server_address, port))

def main():
    if len(sys.argv) != 3:
        print("Usage: python send_shape_to_socket_tester.py <server_address> <port>")
        return

    server_address = sys.argv[1]
    port = int(sys.argv[2])

    x, y, direction_x, direction_y = initialize_position()

    samples_sent = 0

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        try:
            print("Sending 16 sample/s...")

            while True:
                x, y, direction_x, direction_y = move_position(x, y, direction_x, direction_y)
                send_data(sock, server_address, port, x, y)
                
                samples_sent += 1
                if (samples_sent % 100 == 0):
                    print(f"Samples sent: {samples_sent}")

                time.sleep(1/16)

        except KeyboardInterrupt:
            print("\nExiting...")

if __name__ == "__main__":
    main()
