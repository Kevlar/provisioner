#include <App.h>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <set>

class VNIManager {
private:
    std::set<int> allocatedVNIs;
    int nextVNI;

public:
    VNIManager(int startVNI = 4096, int maxVNI = 16777214) : nextVNI(startVNI) {}

    int create() {
        while (allocatedVNIs.find(nextVNI) != allocatedVNIs.end()) {
            nextVNI++; // Find the next available VNI
            if (nextVNI > 16777214) {
                std::cerr << "Error: No available VNI left!\n";
                return -1;
            }
        }
        allocatedVNIs.insert(nextVNI);
        return nextVNI++;
    }

    void deleteVNI(int vni) {
        if (allocatedVNIs.erase(vni)) {
            std::cout << "VNI " << vni << " has been released.\n";
            if (vni < nextVNI) {
                nextVNI = vni; // Reuse lowest possible VNI
            }
        } else {
            std::cerr << "Error: VNI " << vni << " was not allocated!\n";
        }
    }
};

struct Endpoint {
    std::string dev;
    std::string ip;
    int cidr;
    int vni;

    Endpoint(std::string dev, std::string ip, int cidr, int vni) 
        : dev(std::move(dev)), ip(std::move(ip)), cidr(cidr), vni(vni) {}
};

int main() {
    VNIManager vniManager;
    std::unordered_map<int, std::vector<Endpoint>> vxlans;

    uWS::App().get("/VXLAN/create", [&vniManager](auto *res, auto *req) {
        int vni = vniManager.create();
        if (vni != -1) {
            res->end("Allocated VNI: " + std::to_string(vni));
        } else {
            res->end("Error: No available VNIs.");
        }
    }).get("/VXLAN/associate/:vni/:deviceId", [](auto *res, auto *req) {
        std::string vniStr = std::string(req->getParameter(0)); // Convert string_view to string
        std::string deviceId = std::string(req->getParameter(1)); // Convert string_view to string
        int vni;

        try {
            vni = std::stoi(vniStr); // Convert to integer
        } catch (const std::exception &e) {
            res->end("Error: Invalid VNI format.");
            return;
        }

        // Placeholder: Verify if VNI and deviceID exist
        bool vniExists = true; // Replace with actual verification logic
        bool deviceExists = true; // Replace with actual verification logic

        if (vniExists && deviceExists) {
	    //  ip link add vxlan100 type vxlan id <VNI> local <localIP> remote <remoteIP> dstport 4789 dev eth0
	    //  ip addr add <VXLANIP>/<CIDR> dev vxlan100
	    //  ip link set vxlan100 up
	    // add this device to the vxlans so we track needed info to tear down
	    vxlans[vni].emplace_back(
            res->end("Success: VNI " + std::to_string(vni) + " and Device " + deviceID + " are valid.");
        } else {
            res->end("Error: VNI or Device does not exist.");
        }
    }).get("/VXLAN/delete/:vni", [&vniManager](auto *res, auto *req) {
        int vni = std::stoi(std::string(req->getParameter(0)));
	// look at to see what endpoints have this VNI and remove the overlay
	// for endpoint in vxlans[vni]
	//    std::cout << "ip link set vxlan" << endpoint.vni << " down" << endl;
	//    std::cout << "ip addr del " << endpoint.ip << "/" << endpoint.cidr << " dev " << endpoint.dev << endl;
	//    std::cout << "ip link del vxlan" << endpoint.vni << endl;
        vniManager.deleteVNI(vni);
        res->end("Released VNI: " + std::to_string(vni));
    }).listen(9001, [](auto *token) {
        if (token) {
            std::cout << "Server listening on port 9001\n";
        }
    }).run();

    return 0;
}


