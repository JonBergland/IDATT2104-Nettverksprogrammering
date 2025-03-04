const net = require('net');
const crypto = require('crypto');

const clients = []

const wsServer = net.createServer((connection) => {
  console.log('Client connected');
  clients.push(connection);

  connection.once('data', (data) => {
    console.log('Data received from client:', data.toString());
    
    // Separating the headers
    const headers = data.toString().split('\r\n'); 
    console.log('Headers from client: ', headers);
    
    const webSocketKey = headers.find(header => header.startsWith('Sec-WebSocket-Key')).split(': ')[1]; 
    
    console.log('Socket key from client: ', webSocketKey);
    

    // Create the acceptance key by encoding the client key with SHA1 +
    // constant from rfc6445 standard
    const webSocketAcceptKey = crypto.createHash('sha1')
    .update(webSocketKey + '258EAFA5-E914-47DA-95CA-C5AB0DC85B11')
    .digest('base64');

    console.log('Socket key from server: ', webSocketAcceptKey);
    

    // Send the handshake response to Client
    const responseHeaders = [
        'HTTP/1.1 101 Switching Protocols',
        'Upgrade: websocket',
        'Connection: Upgrade',
        `Sec-WebSocket-Accept: ${webSocketAcceptKey}`,
        '\r\n'
      ];

    console.log('Response from Server: ', responseHeaders);
    

    connection.write(responseHeaders.join('\r\n'));

    connection.on('data', (data) => {
        console.log('WebSocket message received:', data);
  
        const frame = decodeWebSocketFrame(data);
        console.log('Decoded WebSocket message:', frame.payloadData.toString());
  
        const responseFrame = encodeWebSocketFrame(frame.payloadData);

        clients.forEach(client => {
          if (client !== connection) {
            console.log(client);
            
            client.write(responseFrame);
          }
        });
    });
  });

  connection.on('end', () => {
    console.log('Client disconnected');
    const index = clients.indexOf(connection);
    if (index === -1) {
      clients.splice(index, 1);
    }
  });
});
wsServer.on('error', (error) => {
  console.error('Error:', error);
});
wsServer.listen(3001, () => {
  console.log('WebSocket server listening on port 3001');
});

function decodeWebSocketFrame(data) {
    const firstByte = data[0];
    const secondByte = data[1];
  
    const fin = firstByte & 0x80;
    const opcode = firstByte & 0x0f;
    const masked = secondByte & 0x80;
    let payloadLength = secondByte & 0x7f;
  
    let offset = 2;
    if (payloadLength === 126) {
      payloadLength = data.readUInt16BE(offset);
      offset += 2;
    } else if (payloadLength === 127) {
      payloadLength = data.readUInt32BE(offset + 4);
      offset += 8;
    }
  
    let maskingKey;
    if (masked) {
      maskingKey = data.slice(offset, offset + 4);
      offset += 4;
    }
  
    const payloadData = data.slice(offset, offset + payloadLength);
    if (masked) {
      for (let i = 0; i < payloadData.length; i++) {
        payloadData[i] ^= maskingKey[i % 4];
      }
    }
  
    return {
      fin,
      opcode,
      masked,
      payloadLength,
      maskingKey,
      payloadData
    };
}
  
function encodeWebSocketFrame(data) {
    const fin = 0x80;
    const opcode = 0x01; 
    const payloadLength = data.length;
  
    let frame = Buffer.alloc(2 + (payloadLength >= 126 ? 2 : 0) + payloadLength);
    frame[0] = fin | opcode;
    if (payloadLength < 126) {
      frame[1] = payloadLength;
    } else if (payloadLength < 65536) {
      frame[1] = 126;
      frame.writeUInt16BE(payloadLength, 2);
    } else {
      throw new Error('Payload too large');
    }
  
    data.copy(frame, frame.length - payloadLength);
    return frame;
}