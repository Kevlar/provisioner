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


To start:
./server


To use:
To get an unused VNI
curl http://localhost:9001/VXLAN/create

To associate a VNI with an endpoint (not quite what was in API doc)
curl http://localhost:9001/VXLAN/associate/{VNI}/{localDevice}/{localIP}

To list all endpoints associated with a VNI
curl http://localhost:9001/VXLAN/{VNI}
