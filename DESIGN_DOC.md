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
* Getting deeper into scalability (beyond using hashmaps to implement the endpoints above), I would leverage in-memory storage to keep latency low (something like a Redis caching layer or Apache Ignite) vs traditional relational database (would still have behind Redis if that was used for caching)

Scalability:
In the interest of low latency (vs higher concurrency) at scale, I'd implement a pub-sub architecture.
VM servers would have agents that would subscribe to to topics that the main server would publish, they would pull network information from a distributed, in-memory store (Redis, Ignite, or similar) and carry out the various commands to set up or tear down VXLANs

Performance:

Security Considerations:
Obviously, tenants need to be isolated to the resources they a provisioned for, from other tenants, and from infrastructure/access that could violate that isolation (don't want a trusted tenant to be able to tunnel outside their boundaries).  Beyond just securing access to the APIs, we'd need to carefully design the data storage layer to prevent security violations and side-channel attacks.  Some data (like VNI) need to be provisioned to prevent conflicts accross tenants, but other data (VMs, endpoints, etc and mappings between VNIs and those objects) need to be restricted.

Control plane integration:
