const httpServerHtml = require('fs').readFileSync('./http_server.html', 'utf8');

const net = require('net');

const port = process.argv[2] || 3000;

// Simple HTTP server responds with a simple WebSocket client test
const httpServer = net.createServer((connection) => {
  connection.on('data', () => {
    connection.write('HTTP/1.1 200 OK\r\nContent-Length: ' + httpServerHtml.length + '\r\n\r\n' + httpServerHtml);
  });
});
httpServer.listen(port, () => {
  console.log('HTTP server listening on port ', port);
});