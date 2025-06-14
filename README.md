
1. API Design Rationale:
* Resources
- interface(endpoint)
- 
* Endpoints
* HTTP methods
* Request/response formats
* Status codes
- tenant not found
- overlay not found
- could not create overlay
- could not modify/delete overlay


/tenant/getTenants
- no arguments (maybe scope this eventually)
- returns tenantIds and other tenant data
/tenant/getNetworks
- takes tenantId
- returns overlayIds for all relevant networks
/overlay/getInfo
- takes overlayId
- returns endpoints (ip, devices, ports), subnets, and other network data
/overlay/create
- takes ?
- returns overlayId
/overlay/update
- takes overlayId and ?
- returns status - try and think if there's useful network data to return post-update
/overlay/delete
- takes overlayId ... and ? (think just overlayId)
- returns status
/interface/add
- takes device info, overlayId

2. Implementation Choices:
* Used C++ (over C) for access to robust standard library of abstractions allowing for more concise and readable code.  I personally prefer C++ when working with many higher level abstractions.  Rust would offer similar abstractions, but I haven't had time to come up to speed with Rust prior to this exercise, so that made the choice for me.
* Used uWebSockets for its reputation for performance (
* Would leverage in-memory storage to keep latency low (something like a Redis caching layer or Apache Ignite) vs traditional relational database (would still have behind Redis if that was used for caching)

Instructions:
Install dependencies:
 libssl
 zlib

For Ubuntu, apt install:
 libssl-dev
 zlib1g-dev

To build (WARNING this will clone uWebSockets and all its submodules and build everything)
run:
 make 
