import socket
import sys

HOST = "127.0.0.1"
PORT = int(sys.argv[1]) if len(sys.argv) > 1 else 9001


def send_request(request):
    with socket.create_connection((HOST, PORT), timeout=5) as sock:
        sock.settimeout(5)
        sock.sendall((request + "\n").encode())

        response = b""
        while b"\n" not in response:
            chunk = sock.recv(1024)
            if not chunk:
                break
            response += chunk

        return response.decode().strip()


tests = [
    ("CALL_SETUP|102|Alice|Bob", "CALL_ACCEPTED|102"),
    ("HELLO", "ERROR|UNKNOWN_MESSAGE"),
    ("CALL_RELEASE|102", "CALL_RELEASED|102"),
]

failed = 0

for request, expected in tests:
    try:
        actual = send_request(request)
        if actual == expected:
            print(f"PASS: {request} -> {actual}")
        else:
            print(f"FAIL: {request} -> {actual}; expected {expected}")
            failed += 1
    except Exception as exc:
        print(f"FAIL: {request} -> {exc}")
        failed += 1

if failed:
    sys.exit(f"{failed} network test(s) failed")

print(f"All {len(tests)} network tests passed.")
