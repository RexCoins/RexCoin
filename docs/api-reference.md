# RexCoin API Reference

## JSON-RPC API (Port 8332)

### getblockcount
Returns current block height.
```json
Request:  {"method":"getblockcount","params":[],"id":1}
Response: {"id":1,"result":12345,"error":null}
```

### getnetworkinfo
Returns network information.
```json
Request:  {"method":"getnetworkinfo","params":[],"id":1}
Response: {"id":1,"result":{"name":"Rex Network","version":"0.1.0","coin":"RXC"},"error":null}
```

### getbalance
Returns balance for an address.
```json
Request:  {"method":"getbalance","params":["RXCabc123..."],"id":1}
Response: {"id":1,"result":50.0,"error":null}
```

### sendtoaddress
Sends RXC to an address.
```json
Request:  {"method":"sendtoaddress","params":["RXCabc123...",10.5],"id":1}
Response: {"id":1,"result":"txhash_here","error":null}
```

### getmininginfo
Returns mining information.
```json
Request:  {"method":"getmininginfo","params":[],"id":1}
Response: {"id":1,"result":{"difficulty":4,"reward":50},"error":null}
```

---

## REST API (Port 8080)

| Method | Endpoint        | Description              |
|--------|----------------|--------------------------|
| GET    | /blocks         | List recent blocks       |
| GET    | /blocks/{hash}  | Get block by hash        |
| GET    | /tx/{hash}      | Get transaction by hash  |
| GET    | /mempool        | Get pending transactions |
| GET    | /stats          | Network statistics       |
| GET    | /address/{addr} | Get address info         |

---

## WebSocket API (Port 8765)

### Subscribe to events

```javascript
const ws = new WebSocket('ws://localhost:8765');

// Subscribe to new blocks
ws.send(JSON.stringify({subscribe: "NEW_BLOCK"}));

// Subscribe to new transactions
ws.send(JSON.stringify({subscribe: "NEW_TX"}));

// Event format
ws.onmessage = (e) => {
  const event = JSON.parse(e.data);
  console.log(event.type, event.data);
};
```

### Event Types
- `NEW_BLOCK` — fired when a new block is mined
- `NEW_TX` — fired when a new transaction enters mempool
- `MEMPOOL_UPDATE` — mempool size changed
