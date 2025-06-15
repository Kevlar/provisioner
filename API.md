
/VXLAN/create
- returns VNI
/VXLAN/delete/{VNI}
- returns status
/VXLAN/associate/{VNI}/{endpointID}/{overlayIP}
- returns status 
/endpoint/{endpointID}
- returns endpoint information (IP, subnet, mac addres)

* Status codes
200:
- success
404:
- tenant not found
- VNI not found
- VM not found
- VTEP not found
409:
- could not associate
- could not modify/delete overlay

