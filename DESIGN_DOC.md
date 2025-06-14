
/network/{tenantId}
- returns non-virtual networks owned by tenant

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

API Design Rationale
Focused on making small loosely-coupled data items that could be stored for quick retrieval.
Tenants - TenantIds (implemented as UUIDs) map to a tenants resources - networks, VMs, VXLANs, etc
various endpoints would take a tenantId and return the respective resource, such as:
/network/<tenantId> - return all the networks owned by a tenant
/VXLAN/<tenantId> - return all vxlans owned by a tenant
/VMs/<tenantId> - return all VMs owned by a tenant

These resouces would also have UUIDs that map them to respective related resources - memory/storage cost of maintaing mappings is vastly prefered to costs of searching data structures at scale.  A few examples would be:
/VXLAN/getEndpoints/<VNI> - returns a vector of metadata objects related to systems that would let systems tear down VXLANs with no additional requests
/VM/getNetworks/<VMId> - get IP/subnets/mac addresses/etc for a VM
/network/getVMs/<networkId> - get a vector of VMIds for all VMs connected to a network

Implementation Choices:
* Used C++ (over C) for access to robust standard library of abstractions allowing for more concise and readable code.  I personally prefer C++ when working with many higher level abstractions.  Rust would offer similar abstractions, but I haven't had time to come up to speed with Rust prior to this exercise, so that made the choice for me.
* Used uWebSockets for its reputation for performance 
* Getting deeper into scalability (beyond using hashmaps to implement the endpoints above), I would leverage in-memory storage for the global network state to keep latency low (something like a Redis caching layer or Apache Ignite) vs traditional relational database (would still have behind Redis if that was used for caching)

Scalability:
In the interest of low latency (vs higher concurrency) at scale, I'd implement a pub-sub architecture.
VM servers would have agents that would subscribe to to topics that the main server would publish, they would pull the global network state from a distributed, in-memory store (Redis, Ignite, or similar) and carry out the various commands to set up or tear down VXLANs

Performance:
DPDK and hardware offloading would be used where appropriate for encapsulation, decapsulation, inspection/checksum encryption, etc.  For example, with VXLAN, DPDK is able to hardware resources (when supported by NICs), reduce interrupts compared to traditional interrupt based handling, and do other optimizations vs the traditional network stack.

Security Considerations:
Tenants need to be isolated to the resources they a provisioned for, from other tenants, and from infrastructure/access that could violate that isolation (don't want a trusted tenant to be able to tunnel outside their boundaries).  Beyond just securing access to the APIs, we'd need to carefully design the data storage layer to prevent security violations and side-channel attacks.  Some data (like VNI) need to be provisioned to prevent conflicts accross tenants, but other data (VMs, endpoints, etc and mappings between VNIs and those objects) need to be restricted.

Control plane integration:
I manually populated the FDB, but ideally, BGP EVPN could be used to populate mappings

Assumptions & Trade-Offs:
Assumptions:
- Linux environment with access to tools like ip, bridge, etc
- Not VXLANS accross heterogenous hardware/VM/nested VM environments
- Only running single instance of server (multiple could cause VNI conflicts)
- VMs only on a single host (wasn't able to finish fdb append implementation)
- Only running single VXLANS for a host (I wasn't able to finish mapping out all the state to deconflict bridges, VNIs, overlay IPs, etc) 

There's numerous tradeoffs in the design:
- Latency vs consistency with using pub-sub vs something like message passing.  Networks design is fundamentally robust and occasional temporary loss of consitency should rarely have significant impact vs latency gains
- Network state objects have tradeoffs on size vs coupling vs number of requests.  I took a stab at a balance of these in the API design, but should definitely be POC-ed and performance tested 
