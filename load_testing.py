import socket
import threading
import time
import struct

def send_command(host, port, client_id):
    """Single client that connects and sends 'set name abc'"""
    try:
        sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        sock.connect((host, port))
        
        # Command to send
        command = "set name abc"
        command_bytes = command.encode('utf-8')
        length = len(command_bytes)
        
        # Send length-prefixed message
        start_time = time.time()
        message = struct.pack('<I', length) + command_bytes
        sock.send(message)
        
        # Read response
        response_len_bytes = sock.recv(4)
        if len(response_len_bytes) == 4:
            response_len = struct.unpack('<I', response_len_bytes)[0]
            sock.recv(response_len)
        
        end_time = time.time()
        response_time = (end_time - start_time) * 1000  # Convert to ms
        
        sock.close()
        return response_time
        
    except Exception as e:
        print(f"Client {client_id} error: {e}")
        return None

def run_load_test(num_clients=1000):
    """Run load test with specified number of clients"""
    print(f"Testing with {num_clients} clients...")
    print("Each client sends: 'set name abc'")
    print()
    
    response_times = []
    threads = []
    
    def worker(client_id):
        response_time = send_command('localhost', 6379, client_id)
        if response_time is not None:
            response_times.append(response_time)
    
    start_time = time.time()
    
    # Create and start all threads
    for i in range(num_clients):
        thread = threading.Thread(target=worker, args=(i,))
        threads.append(thread)
        thread.start()
    
    # Wait for all to complete
    for thread in threads:
        thread.join()
    
    end_time = time.time()
    total_time = end_time - start_time
    
    # Print results
    successful = len(response_times)
    failed = num_clients - successful
    
    print(f"Results:")
    print(f"  Total requests: {num_clients}")
    print(f"  Successful: {successful}")
    print(f"  Failed: {failed}")
    print(f"  Total time: {total_time:.2f} seconds")
    print(f"  Requests/second: {num_clients/total_time:.2f}")
    
    if response_times:
        avg_response = sum(response_times) / len(response_times)
        print(f"  Average response time: {avg_response:.2f} ms")

if __name__ == "__main__":
    # Test with different loads
    print("Simple Redis Load Test")
    clients = int(input("Number of clients (default 1000): ") or "1000")
    run_load_test(clients)