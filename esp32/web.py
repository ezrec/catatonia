
try:
    import usocket as socket
except:
    import socket


# from machine import Pin

# p0 = Pin(2, Pin.OUT)

header_404 = [
    'HTTP/1.1 404 Not Found',
    'Context-Type: text/html',
    'Connection: close'
]
content_404 = '<html><body><h1>Nothing to see here... Move along...</h1></body></html>'

header_400 = [
    'HTTP/1.1 400 Bad Request',
    'Context-Type: text/html',
    'Connection: close'
]
content_400 = '<html><body><h1>HTTP/1.1 command not supported</h1></body></html>'

def parse_request(text='', root="/flash/www"):
    request_method = ""
    path = ""
    request_version = ""
    if text == '':
        return header_404, content_404

    request_line = text.split("\r\n")[0]
    request_line = request_line.split()
    # Break down the request line into components
    (request_method,  # GET
     path,            # /hello
     request_version  # HTTP/1.1
     ) = request_line
    if request_method == "POST":
        return header_400, content_400
    if request_method == "GET":
        if "?" in path:
            filename, value_list = path.strip('/').split('?')
            values = value_list.split('&')
            if 'led=on' in values:
                # p0.low()
                pass
            else:
                # p0.high()
                pass
        else:
            filename = path.strip('/')
        if filename == '':
            filename = 'index.html'
        fileext = filename.split('.')[1]

        try:
            f = open(root + "/" + filename, 'r')
            content = f.read()
            f.close()
        except Exception:
            return header_404, content_404

        header = [
            'HTTP/1.1 200 OK',
        ]

        if fileext == 'html' or \
               fileext == 'css' or \
               fileext == 'js':
            header += [ 'Content-Type: text/' + fileext ]
            return header, content
        if fileext == 'png' or \
                fileext == 'jpg' or \
                fileext == 'gif' or \
                fileext == 'png' or \
                fileext == 'ico':
            header += [ 'Content-Type: image/' + fileext ]
        return header, content


def Server(port=80, use_stream=False):
    s = socket.socket()

    # Binding to all interfaces - server will be accessible to other hosts!
    ai = socket.getaddrinfo("0.0.0.0", port)
    addr = ai[0][-1]

    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(addr)
    s.listen(5)

    while True:
        res = s.accept()
        client_s = res[0]
        client_addr = res[1]
        if use_stream:
            # MicroPython socket objects support stream (aka file) interface
            # directly.
            print (client_addr)
            header, content = parse_request(client_s.recv(4096).decode('utf-8'))
            if header != '':
                client_s.write(header)
                totalsent = 0
                while totalsent < len(content):
                    sent = client_s.write(content)
                    totalsent += len(sent)
        else:
            header, content = parse_request(client_s.recv(4096).decode('utf-8'))
            header += [ 'Content-Length: ' + str(len(content)) ]
            client_s.send("\r\n".join(header))
            client_s.send("\r\n")
            client_s.send(content)
        client_s.close()
