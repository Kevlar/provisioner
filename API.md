### Allocate VXLAN [POST /VXLAN/create]
Returns an unused VNI
- Response 200
```json
{
  "VNI": 4097
}
```
- Response 507
```json
{
  "error": "No VNI available"
}
```

### Delete VXLAN [DELETE /VXLAN/delete/{VNI}]
Remove all VXLAN devices associated with VNI
- Response 200
- Response 404
```json
{
  "error": "VNI not found"
}
```

### Associate VXLAN with interface [PUT /VXLAN/associate/{VNI}/{endpointID}/{overlayNetwork}]
Creates VXLAN device for VNI at endpoint identified by endPointID and assigns it with overlayNetwork(ip and subnet)
- Response 200
- Response 404
```json
{
  "error": "VNI not found"
}
```
- Response 404
```json
{
  "error": "endPointID not found"
}
```

### Get endpoints for VXLAN [GET /VXLAN/endpoints/{VNI}]
Get the UUIDs representing the endpoints for VXLAN with VNI
- Response 200
```json
{
  "endpoints": [
    "00000000-0000-0000-0000-000000000000",
    "FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF"
  ]
}
```
- Response 404
```json
{
  "error": "VNI not found"
}
```

### Get endpoint information [GET /endpoint/{endpointID}]
Gets endpoint information - device/ip information as well as optional items like VNI/VTEP ip, etc if associated with a VXLAN
- Response 200
```json
{
  "localIp":"10.0.0.1",
  "dev": "eth0",
  "VNI": 4096,  (optional)
  "VTEP IP": "192.168.1.1/24", (optional)
  "V-MAC": "00:00:00:00:00:01" (optional)
}
```
- Response 404
```json
{
  "error": "endpointID not found"
}
```

### Update endpoint [PUT /endpoint/update/{endpointID}]
Updates an endpoint via JSON
Content-Type: application/json
```json
{
  "VNI": 4096,  (optional)
  "VTEP IP": "192.168.1.1/24", (optional)
  "V-MAC": "00:00:00:00:00:01" (optional)
}
```
-Response 200
- Response 404
```json
{
  "error": "endpointID not found"
}
```
